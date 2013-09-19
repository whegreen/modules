/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include <windows.h>
#include <tchar.h>
#include "pxcpowerstate.h"
#include "pxcsession.h"
#include "pxcsmartptr.h"
#include "math_async.h"

int _tmain(int, _TCHAR*[]) {
    
	//coba async start
	//todo: stream depth + surf flann ma
	/*
	PXCSmartArray<PXCImage> images;
	PXCSmartSPArray sps(2);
	stream->ReadStreamAsync(&images,&sps[0]);
	face->ProcessImageAsync(images,&sps[1]);
	sps.SynchronizeEx();
	*/
	
	// new session to access SDK
	PXCSmartPtr<PXCSession> session;
    PXCSession_Create(&session);

	//load DLL
    session->LoadImplFromFile(TEXT("simple_math.dll"));

	pxcF32 estafetValue;
	PXCSmartSPArray sp(2);
	PXCSmartPtr<PXCMathAsync> mathAsync;
    pxcStatus sts=session->CreateImpl<PXCMathAsync>(&mathAsync);

	pxcF32 v=2.0F;
	pxcF32 v2=0.3F, o;

	//proses 1
    if (sts>=PXC_STATUS_NO_ERROR) {
		mathAsync->InvertAsync(&v,&estafetValue,&sp[0]);
        //sp->Synchronize();
		_tprintf_s(TEXT("Located PXCMathASync 1 \n 1.0/%f=%f\n"),v,estafetValue);
    }
	
	//proses 2
    if (sts>=PXC_STATUS_NO_ERROR) {
		mathAsync->InvertAsync2(&estafetValue, &v2, &o, &sp[1]);
		//sp->Synchronize();
		_tprintf_s(TEXT("Located PXCMathASync 2 \n %f/%f=%f\n"),estafetValue,v2,o);
	}

	sp.SynchronizeEx();
	_tprintf_s(TEXT("----------AFTER SYNC----------\n"));
	_tprintf_s(TEXT("Located PXCMathASync 1 \n 1.0/%f=%f\n"),v,estafetValue);
	_tprintf_s(TEXT("Located PXCMathASync 2 \n %f/%f=%f\n"),estafetValue,v2,o);

    return 0;
}
