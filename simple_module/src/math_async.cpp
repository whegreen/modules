/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include "service/pxcsessionservice.h"
#include "service/pxcschedulerservice.h"
#include "service/pxcsmartasyncimpl.h"
#include "math_async.h"

//#include "stdio.h"

/* This is the implementation of the PXMathAsync machine */
class MathAsync:public PXCBaseImpl<PXCMathAsync> { /* The implementation class must derive from PXCBaseImpl<PXCMathAsync> */
public:

    MathAsync(PXCScheduler *scheduler) { this->scheduler=scheduler; }

    virtual pxcStatus PXCAPI InvertAsync(pxcF32 *v, pxcF32 *o, PXCScheduler::SyncPoint **sp);
	virtual pxcStatus PXCAPI InvertAsync2(pxcF32 *v1, pxcF32 *v2, pxcF32 *o, PXCScheduler::SyncPoint **sp);

    static  pxcStatus PXCAPI CreateInstance(PXCSession *session, PXCScheduler *sch, PXCAccelerator *accel, PXCSessionService::DLLExportTable *table, pxcUID cuid, PXCBase **instance);
    virtual pxcStatus PXCAPI InvertSync(pxcF32 *v, pxcF32 *o);
	virtual pxcStatus PXCAPI InvertSync2(pxcF32 *v1, pxcF32 *v2, pxcF32 *o);

protected:

    PXCScheduler *scheduler;
};

pxcStatus MathAsync::InvertSync(pxcF32 *v, pxcF32 *o) {
    if (!(*v)) return PXC_STATUS_PARAM_UNSUPPORTED;
	//printf("------call sync method----------");
	//Sleep(1000);
    *o= (pxcF32)1.0/(*v);
    return PXC_STATUS_NO_ERROR;
}

pxcStatus MathAsync::InvertAsync(pxcF32 *v, pxcF32 *o, PXCScheduler::SyncPoint **sp) {
    //printf("------call async method----------");
	return PXCSmartAsyncImpl<MathAsync,pxcF32,pxcF32>::SubmitTask(v,o,sp,this,scheduler,&MathAsync::InvertSync);
}

pxcStatus MathAsync::InvertSync2(pxcF32 *v1, pxcF32 *v2, pxcF32 *o) {
    if (!(*v1 && *v2)) return PXC_STATUS_PARAM_UNSUPPORTED;
    *o= (pxcF32)*v1/(*v2);
    return PXC_STATUS_NO_ERROR;
}

pxcStatus MathAsync::InvertAsync2(pxcF32 *v1, pxcF32 *v2, pxcF32 *o, PXCScheduler::SyncPoint **sp) {
	return PXCSmartAsyncImplI2O1<MathAsync,pxcF32,pxcF32,pxcF32>::SubmitTask(v1,v2,o,sp,this,scheduler,&MathAsync::InvertSync2);
}

pxcStatus MathAsync::CreateInstance(PXCSession *, PXCScheduler *sch, PXCAccelerator *, PXCSessionService::DLLExportTable *, pxcUID, PXCBase **instance) {
    *instance=new MathAsync(sch);
    if (!(*instance)) return PXC_STATUS_ALLOC_FAILED;
    return PXC_STATUS_NO_ERROR;
}

/* This table declares the algorithm to the PXC session */
//extern PXCSessionService::DLLExportTable mathPower;
PXCSessionService::DLLExportTable __declspec(dllexport) mathAsync = {
    //&mathPower,
    0,
	MathAsync::CreateInstance,
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
	  { MathAsync::CUID, 0, 0 },	      /* cuids */
      L"Example Math Module (Async)",
      {0}                                 /* reserved */
    },
};


