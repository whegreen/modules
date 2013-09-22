/*******************************************************************************
	whe_green@yahoo.co.id
	ninjutsu showdown project
	september 2013
*******************************************************************************/
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
#include "surf_flann_async.h"

#include "ippi.h"
#include "ippcore.h"

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

	pxcF32 o2;
	o2 = 0;

	PXCSmartSPArray sp(3);
	PXCSmartPtr<PXCSURFFLANNAsync> SURFFLANNAsync;
	pxcStatus sts=session->CreateImpl<PXCSURFFLANNAsync>(&SURFFLANNAsync);
	
	PXCSmartSP sps;

	/* stream data */
	PXCSmartPtr<PXCImage> image;
	PXCImage::ImageInfo info;
	PXCImage::ImageData data;
	pxcU32* idx = 0;
	
	string name = "tes";

	//init sync proses
	uc.QueryVideoStream(0)->ReadStreamAsync(&image,&sp[0]);	
	SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, &sp[1]);

	bool record = false;

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

		int c = waitKey(5);
		if( (char)c == 27 ) { break; }
		else if ((char)c == 32){ record = true; }

		image->ReleaseAccess(&data);

  		uc.QueryVideoStream(0)->ReadStreamAsync(image.ReleaseRef(),sp.ReleaseRef(0));
		  
		if (sp[1]->Synchronize(0)<PXC_STATUS_NO_ERROR) {printf("error"); continue;};
		SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, sp.ReleaseRef(1));
		
		/*
		//ready countdown to capture
		IplImage ii = IplImage();
		SURFFLANNAsync->HandsealDetectAsync(image, &templateIdx, &o, sp.ReleaseRef(2));
		//SURFFLANNAsync->RecordSampleAsync(image, &o2, &sp[2]);
		sp[2]->Synchronize(0);

		int i = 0;
		if(record){
			record = false;
			while(i<3){
				i++;
				cout << "counter : " << i << endl;
				Sleep(1000);
			}
			//if (sp[2]->Synchronize(0)<PXC_STATUS_NO_ERROR) {printf("error"); continue;};
			
			
			
			
		}
		*/
		
		
	}
	sp.SynchronizeEx();
	//delete ptrName;
}//close main void