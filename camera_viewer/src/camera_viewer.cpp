#include <stdio.h>
#include <iostream>

#include "pxcsession.h"
#include "pxcsmartptr.h"
#include "pxccapture.h"
#include "util_capture.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <windows.h>
#include <tchar.h>
//#include "pxcpowerstate.h"
//#include "pxcsession.h"
//#include "pxcsmartptr.h"
#include "surf_flann_async.h"

#include "ippi.h"
#include "ippcore.h"

using namespace std;
using namespace cv;

/** Function Headers */

/** Global variables */
String window_name = "Capture - Face detection";


int wmain() {

	//Mat frame;
	Mat frame;
	/* create a session */
	PXCSmartPtr<PXCSession> session;
	PXCSession_Create(&session);

	//load handseal_detect.DLL file
	session->LoadImplFromFile(TEXT("handseal_detect.dll"));

	/* request a color stream */
	PXCCapture::VideoStream::DataDesc req;
	memset(&req,0,sizeof(req));
	
	req.streams[0].format=PXCImage::COLOR_FORMAT_DEPTH;
	//req.streams[0].format=PXCImage::IMAGE_TYPE_DEPTH;
	req.streams[0].sizeMin.width=req.streams[0].sizeMax.width=320;
	req.streams[0].sizeMin.height=req.streams[0].sizeMax.height=240;
	
	UtilCapture uc(session);
	uc.LocateStreams(&req);

	pxcF32 templateIdx;
	templateIdx = 1;

	pxcF32 o;
	o = 0;

	PXCSmartSPArray sp(2);
	//PXCSmartSP sp;

	PXCSmartPtr<PXCSURFFLANNAsync> SURFFLANNAsync;
	pxcStatus sts=session->CreateImpl<PXCSURFFLANNAsync>(&SURFFLANNAsync);
	
	/* stream data */
	PXCSmartPtr<PXCImage> image;
	PXCImage::ImageInfo info;
	PXCImage::ImageData data;
	pxcU32* idx = 0;
		
	//init sync proses
	uc.QueryVideoStream(0)->ReadStreamAsync(&image,&sp[0]);	
	SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, &sp[1]);
	for (;;) {
		
		sp.SynchronizeEx(idx);
		
		if (sp[0]->Synchronize(0)<PXC_STATUS_NO_ERROR) continue;
		
		
		image->QueryInfo(&info);
		image->AcquireAccess(PXCImage::ACCESS_READ,&data);
		//cout << "info : " << info.width << endl;
		short* Idata=new short[240*320*1];
		IppiSize roi={ info.width, info.height };
		ippiCopy_16u_C1R((Ipp16u*)data.planes[0],data.pitches[0],(Ipp16u*)Idata,data.pitches[0],roi);
		Mat frame(240, 320, CV_16UC1, Idata);
		imshow( "test capture 1", frame );

		int c = waitKey(7);
		if( (char)c == 27 ) { break; }

		image->ReleaseAccess(&data);
		

		uc.QueryVideoStream(0)->ReadStreamAsync(image.ReleaseRef(),sp.ReleaseRef(0));
		
		if (sp[1]->Synchronize(0)<PXC_STATUS_NO_ERROR) {printf("error"); continue;};

		SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, sp.ReleaseRef(1));
			
			
		/*
		PXCImage::ImageData data;
		image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);		
		
		
		cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
		image->ReleaseAccess(&data);

		Mat newFrame = cvarrToMat(colorimg,true);
		
		*/
		//printf("------- inside DLL -------");

		/*
		PXCImage::ImageData data;
		image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);		
		IplImage *colorimg = cvCreateImage(cvSize(320, 240), 16, 1);
		cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
		image->ReleaseAccess(&data);
		Mat newFrame = cvarrToMat(colorimg,true);
		imshow( "test capture", newFrame );
		waitKey(1);
		*/

		// todo: dipindah ke dalam dll
	   
		/*
		image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);
		IplImage* colorimg = cvCreateImage(cvSize(320, 240), 16, 1);
		cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
		Mat frame = cvarrToMat(colorimg);
		*/

		//todo: call handseal_detect here
		//whe call void from dll
		
		/*
		//proses 1
		if (sts>=PXC_STATUS_NO_ERROR) {
			SURFFLANNAsync->HandsealDetectAsync(&frame, &templateIdx, &o, &sp[1]);  
		}
		*/
	
		//sp.SynchronizeEx();
		
	}
	sp.SynchronizeEx();
}//close main void