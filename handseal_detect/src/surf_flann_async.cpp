/*******************************************************************************



*******************************************************************************/

#include "service/pxcsessionservice.h"
#include "service/pxcschedulerservice.h"
#include "service/pxcsmartasyncimpl.h"
#include "surf_flann_async.h"

#include <stdio.h>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include "ippi.h"
#include "ippcore.h"

using namespace std;
using namespace cv;

/* This is the implementation class */
class SURFFLANNAsync:public PXCBaseImpl<PXCSURFFLANNAsync> { /* The implementation class must derive from .h file */
public:
    SURFFLANNAsync(PXCScheduler *scheduler) { this->scheduler=scheduler; }
	
	//define void untuk dipanggil dari luar file DLL
	virtual pxcStatus PXCAPI HandsealDetectAsync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp);
	virtual pxcStatus PXCAPI HandsealDetectAsync2(PXCImage *frame, IplImage *oFrame, PXCScheduler::SyncPoint **sp);

    static  pxcStatus PXCAPI CreateInstance(PXCSession *session, PXCScheduler *sch, PXCAccelerator *accel, PXCSessionService::DLLExportTable *table, pxcUID cuid, PXCBase **instance);
    
	//define void internal untuk dipanggil setelah async siap
	virtual pxcStatus PXCAPI HandsealDetectSync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o);
	virtual pxcStatus PXCAPI HandsealDetectSync2(PXCImage *frame, IplImage *oFrame);

	//save & load pattern
	virtual pxcStatus PXCAPI SavePattern( Mat img, String name );
	virtual pxcStatus PXCAPI LoadPattern( Mat img, String name );

protected:
    PXCScheduler *scheduler;
};

/*
input  : 
	hasil image depth stream dalam format opencv::mat
	location file image template handseal
output : 
	ada result mirip atau ngga, disimpan didalam array
*/

pxcStatus SURFFLANNAsync::HandsealDetectSync2(PXCImage *image, IplImage *oFrame) {
	if (!(image)) return PXC_STATUS_PARAM_UNSUPPORTED;
	return PXC_STATUS_NO_ERROR;
}

//passign parameter mat descriptor yang di load dari main void
pxcStatus SURFFLANNAsync::HandsealDetectSync(PXCImage *image, pxcF32 *templateIdx, pxcF32 *o) {
	if (!(image)) return PXC_STATUS_PARAM_UNSUPPORTED;
	PXCImage::ImageInfo info;
    image->QueryInfo(&info);
	
	PXCImage::ImageData data;
	image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);		
		
	//IplImage *colorimg = cvCreateImage(cvSize(320, 240), 16, 1);

	short* Idata=new short[240*320*1];
	IppiSize roi={ info.width, info.height };
	ippiCopy_16u_C1R((Ipp16u*)data.planes[0],data.pitches[0],(Ipp16u*)Idata,data.pitches[0],roi);
	
	//cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
	//cvSetData(colorimg, &data2, 320*sizeof(short)*1);
	
	Mat newFrame(240, 320, CV_16UC1, Idata);
	Mat bayer8Bit, gray;
	//newFrame.convertTo(bayer8Bit, CV_8UC1, 0.0625);
	newFrame.convertTo(bayer8Bit, CV_8UC1, 0.2, 0);

	gray = bayer8Bit;
	//cvtColor(bayer8Bit, gray, CV_BayerRG2GRAY); 
	
	double minVal; 
    double maxVal; 
    Point minLoc; 
    Point maxLoc;

    minMaxLoc( gray, &minVal, &maxVal, &minLoc, &maxLoc );

    //cout << "min val : " << minVal << endl; // yang diambil buat penanda paling item atau paling depan
    //cout << "max val: " << maxVal << endl; // nunjukin pixel putih , ga penting
	
	int batas = 20;

	Mat res;
	threshold(gray, res, (batas + minVal), 255 , THRESH_TOZERO_INV);
	addWeighted(res, 2.5, res, -0.5, 0, res);
	
	//-----------------------whe surf flann
	Mat img_1 = res;
	//Mat img_2 = imread( "sample3.png", IMREAD_GRAYSCALE );

	//if( !img_1.data || !img_2.data )
	if( !img_1.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return PXC_STATUS_PARAM_UNSUPPORTED; }

	//default
	//int minHessian = 400;
	int minHessian = 800;
	SurfFeatureDetector detector( minHessian );
	std::vector<KeyPoint> keypoints_1, keypoints_2;

	detector.detect( img_1, keypoints_1 );
	//detector.detect( img_2, keypoints_2 );
		  
	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;

	Mat img_keypoints_1;
	drawKeypoints( img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	
	//extractor.compute( img_1, keypoints_1, descriptors_1 );
	//extractor.compute( img_2, keypoints_2, descriptors_2 );

	//SavePattern( descriptors_1, "descriptor1.yml" );


	//matiin matcher flann algorithm
	/*
	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_1, descriptors_2, matches );

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_1.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist );
	printf("-- Min dist : %f \n", min_dist );

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	 	  
	for( int i = 0; i < descriptors_1.rows; i++ )
	{ if( matches[i].distance <= 2*min_dist )
	{ 
		good_matches.push_back( matches[i]); 
	}
	}
	

	float o = 100*good_matches.size()/descriptors_2.rows;

	printf("-- good matches : %i \n", good_matches.size());
	printf("-- deskriptor 1 : %i \n", descriptors_1.rows);
	printf("-- deskriptor 2 : %i \n", descriptors_2.rows);
	printf("-- akurasi : %f \n", o);
	*/

	//-----------------------ehw surf flann
	
	//imshow( "test capture", gray );
	//imshow( "test capture", bayer8Bit );

	imshow( "test capture", img_keypoints_1 );
	waitKey(7);
	
	image->ReleaseAccess(&data);

	return PXC_STATUS_NO_ERROR;
}

pxcStatus SURFFLANNAsync::HandsealDetectAsync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp) {
    //panggil void sync untuk dimasukkan ke scheduler
	return PXCSmartAsyncImplI2O1<SURFFLANNAsync,PXCImage,pxcF32,pxcF32>::SubmitTask(frame, templateIdx, o, sp, this, scheduler, &SURFFLANNAsync::HandsealDetectSync);
}

pxcStatus SURFFLANNAsync::HandsealDetectAsync2(PXCImage *frame, IplImage *oFrame, PXCScheduler::SyncPoint **sp) {
    //panggil void sync untuk dimasukkan ke scheduler
	return PXCSmartAsyncImpl<SURFFLANNAsync,PXCImage,IplImage>::SubmitTask(frame, oFrame, sp, this, scheduler, &SURFFLANNAsync::HandsealDetectSync2);
}

pxcStatus SURFFLANNAsync::CreateInstance(PXCSession *, PXCScheduler *sch, PXCAccelerator *, PXCSessionService::DLLExportTable *, pxcUID, PXCBase **instance) {
    *instance=new SURFFLANNAsync(sch);
    if (!(*instance)) return PXC_STATUS_ALLOC_FAILED;
    return PXC_STATUS_NO_ERROR;
}

pxcStatus SURFFLANNAsync::SavePattern( Mat img, String name = "test.yml" ){
	//write
	cv::FileStorage storage(name, cv::FileStorage::WRITE);
	storage << "img" << img;
	storage.release();
	return PXC_STATUS_NO_ERROR;
}

pxcStatus SURFFLANNAsync::LoadPattern( Mat img, String name = "test.yml" ){
	//read
	cv::FileStorage storage(name, cv::FileStorage::READ);
	storage["img"] >> img;
	storage.release();
	return PXC_STATUS_NO_ERROR;
}
	  
/* This table declares the algorithm to the PXC session */
PXCSessionService::DLLExportTable __declspec(dllexport) mathAsync = {
    0,
	SURFFLANNAsync::CreateInstance,
    PXCSessionService::SUID_DLL_EXPORT_TABLE, 
    {
      PXCSession::IMPL_GROUP_USER,        /* group */
      PXC_UID('S','F','A','S'),           /* subgroup */
      PXC_UID('H','S','D','T'),           /* algorithm */
      PXC_UID('S','F','A','S'),           /* iuid */
      { 1, 0 },                           /* version */
      0xffffffff,                         /* acceleration */
      101,                                /* merit */
      PXC_UID('M','O','R','G'),           /* vendor */
	  { PXCSURFFLANNAsync::CUID, 0, 0 },	      /* cuids */
      L"Example SURFFLANN Module (Async)",
      {0}                                 /* reserved */
    },
};


