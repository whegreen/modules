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
	//PXCImage::ImageData data;

	IplImage *colorimg = cvCreateImage(cvSize(320, 240), 16, 1);

	/* stream data */
	for (;;) {
		PXCSmartPtr<PXCImage> image;
		
		if (uc.QueryVideoStream(0)->ReadStreamAsync(&image,&sp[0])<PXC_STATUS_NO_ERROR) continue;
		
		//SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, &sp[1]);  
		SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, &sp[1]);
		sp.SynchronizeEx();
		
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
		//----------AFTER SYNC----------
		//imshow( "test capture", frame );

		//todo: dipindah ke dll
		//image->ReleaseAccess(&data);
		//ehw
		//-- bail out if escape was pressed
		//int c = waitKey(10);
		//if( (char)c == 27 ) { break; }
	}
}//close main void