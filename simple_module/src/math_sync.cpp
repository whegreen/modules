/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include "service/pxcsessionservice.h"
#include "service/pxcschedulerservice.h"
#include "math_sync.h"

/* This is the implementation of the PXCMathSync machine */
class MathSync:public PXCBaseImpl<PXCMathSync> { /* The implementation class must derive from PXCBaseImpl<PXCMathSync> */
public:

    virtual pxcStatus PXCAPI Invert(pxcF32 v, pxcF32 *o);

    /* This is the constructor */
    static  pxcStatus PXCAPI CreateInstance(PXCSession *session, PXCScheduler *sch, PXCAccelerator *accel, PXCSessionService::DLLExportTable *table, pxcUID cuid, PXCBase **instance);

    /* This is the Destructor function. Override this function if you need to do anything special. 
    virtual pxcStatus PXCAPI Release(void);
    */
};

pxcStatus MathSync::Invert(pxcF32 v, pxcF32 *o) {
    if (!v) return PXC_STATUS_PARAM_UNSUPPORTED;
    *o=(pxcF32)1.0/v;
    return PXC_STATUS_NO_ERROR;
}

/* If you override Release, don't forget to call the base class Release
void MathSync::Release(void) {
    return PXCBaseImpl<PXCMath>::Release();
}
*/

pxcStatus MathSync::CreateInstance(PXCSession *, PXCScheduler *, PXCAccelerator *, PXCSessionService::DLLExportTable *, pxcUID, PXCBase **instance) {
    *instance=new MathSync;
    if (!(*instance)) return PXC_STATUS_ALLOC_FAILED;
    return PXC_STATUS_NO_ERROR;
}

/* This table declares the algorithm to the PXC session */
extern PXCSessionService::DLLExportTable mathAsync;
PXCSessionService::DLLExportTable __declspec(dllexport) mathSync = {
    &mathAsync,
    MathSync::CreateInstance,
    PXCSessionService::SUID_DLL_EXPORT_TABLE, 
    {
      PXCSession::IMPL_GROUP_USER,        /* group */
      PXC_UID('M','T','S','Y'),           /* subgroup */
      PXC_UID('I','N','V','T'),           /* algorithm */
      PXC_UID('M','T','S','Y'),           /* iuid */
      { 1, 0 },                           /* version */
      0xffffffff,                         /* acceleration */
      100,                                /* merit */
      PXC_UID('M','O','R','G'),           /* vendor */
	  { PXCMathSync::CUID, 0, 0 },	      /* cuids */
      L"Example Math Module (Sync)",
      {0}                                 /* reserved */
    },
};

