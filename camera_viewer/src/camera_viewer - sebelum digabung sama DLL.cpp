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

	Mat frame;
	/* create a session */
	PXCSmartPtr<PXCSession> session;
	PXCSession_Create(&session);

	//load handseal_detect.DLL file
	session->LoadImplFromFile(TEXT("handseal_detect.dll"));

	/*
	//whe call void from dll
	pxcF32 estafetValue;
	PXCSmartSPArray sp(2);
	PXCSmartPtr<PXCSURFFLANNAsync> SURFFLANNAsync;
    pxcStatus sts=session->CreateImpl<PXCSURFFLANNAsync>(&SURFFLANNAsync);

	pxcF32 v=2.0F;
	pxcF32 v2=0.3F, o;

	//proses 1
    if (sts>=PXC_STATUS_NO_ERROR) {
		SURFFLANNAsync->HandsealDetectAsync();
	    //sp->Synchronize();
		_tprintf_s(TEXT("Located PXCMathASync 1 \n 1.0/%f=%f\n"),v,estafetValue);
    }
	
	sp.SynchronizeEx();
	
	_tprintf_s(TEXT("----------AFTER SYNC----------\n"));
	_tprintf_s(TEXT("Located PXCMathASync 1 \n 1.0/%f=%f\n"),v,estafetValue);
	_tprintf_s(TEXT("Located PXCMathASync 2 \n %f/%f=%f\n"),estafetValue,v2,o);


	//ehw
	*/
	
	/* request a color stream */
	PXCCapture::VideoStream::DataDesc req;
	memset(&req,0,sizeof(req));
	
	req.streams[0].format=PXCImage::COLOR_FORMAT_DEPTH;
	//req.streams[0].format=PXCImage::IMAGE_TYPE_DEPTH;
	req.streams[0].sizeMin.width=req.streams[0].sizeMax.width=320;
	req.streams[0].sizeMin.height=req.streams[0].sizeMax.height=240;
	
	/*
	req.streams[0].format=PXCImage::COLOR_FORMAT_RGB32;
	req.streams[0].sizeMin.width=req.streams[0].sizeMax.width=640;
	req.streams[0].sizeMin.height=req.streams[0].sizeMax.height=480;
	*/

	UtilCapture uc(session);
	uc.LocateStreams(&req);

	/* stream data */
	for (;;) {
	   PXCSmartPtr<PXCImage> image;
	   PXCSmartSP sp;
	   if (uc.QueryVideoStream(0)->ReadStreamAsync(&image,&sp)<PXC_STATUS_NO_ERROR) continue;
	   sp->Synchronize();
	   // image is a PXCImage instance
		
		PXCImage::ImageData data;
		image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);
		IplImage* colorimg = cvCreateImage(cvSize(320, 240), 16, 1);
		cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
		Mat frame = cvarrToMat(colorimg);
		
		/*
		image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);
		IplImage* colorimg = cvCreateImage(cvSize(640, 480), 8, 3);
		cvSetData(colorimg, (uchar*)data.planes[0], 640*3*sizeof(uchar));
		Mat frame = cvarrToMat(colorimg);
		*/
		
		//Mat frame(colorimg, true);
	
		std::vector<Rect> faces;
		Mat frame_gray;

		//bikin gray
		//cvtColor( frame, frame_gray, COLO );
		//cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
		
		//baca lagi normalisasi histogram!!
		//equalizeHist( frame, frame );
	
		//todo: call handseal_detect here
		imshow( "test capture", frame );
	
		//-- bail out if escape was pressed
		int c = waitKey(10);
		if( (char)c == 27 ) { break; }
	
		image->ReleaseAccess(&data);
		//cvReleaseImage(&colorimg);
		//delete colorimg;
	}
}//close main void