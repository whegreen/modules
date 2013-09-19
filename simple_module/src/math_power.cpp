/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include "service/pxcsessionservice.h"
#include "service/pxcschedulerservice.h"
#include "service/pxcsmartasyncimpl.h"
#include "pxcpowerstate.h"
#include "math_async.h"

class MathPower:public PXCBaseImpl2<PXCMathAsync,PXCPowerState> {
public:

    virtual pxcStatus PXCAPI QueryState(State *state, State *istate);
    virtual pxcStatus PXCAPI SetState(State state);
    virtual pxcStatus PXCAPI InvertAsync(pxcF32 *v, pxcF32 *o, PXCScheduler::SyncPoint **sp);

    static  pxcStatus PXCAPI CreateInstance(PXCSession *session, PXCScheduler *sch, PXCAccelerator *accel, PXCSessionService::DLLExportTable *table, pxcUID cuid, PXCBase **instance);
    virtual pxcStatus PXCAPI InvertSync(pxcF32 *v, pxcF32 *o);

protected:

    PXCScheduler *scheduler;
    PXCPowerState::State state, istate;
};

pxcStatus MathPower::QueryState(State *state, State *istate) {
    *state=this->state;
    *istate=this->istate;
    return PXC_STATUS_NO_ERROR;
}

pxcStatus MathPower::SetState(State state) {
    this->state=state;
    istate=(state==PXCPowerState::STATE_ADAPTIVE)?PXCPowerState::STATE_C0:state;
    return PXC_STATUS_NO_ERROR;
}

pxcStatus MathPower::InvertAsync(pxcF32 *v, pxcF32 *o, PXCScheduler::SyncPoint **sp) {
    return PXCSmartAsyncImpl<MathPower,pxcF32,pxcF32>::SubmitTask(v,o,sp,this,scheduler,&MathPower::InvertSync);
}

pxcStatus MathPower::InvertSync(pxcF32 *v, pxcF32 *o) {
    if (!(*v)) return PXC_STATUS_PARAM_UNSUPPORTED;
    /* Newton–Raphson division: we run more iterations if power state allows */
    int nloops=(5-(int)istate);
    *o=48.0F/17.0F-(pxcF32)(*v)*32.0F/17.0F;
    for (int i=0;i<nloops;i++) {
        *o=(*o)*(2-(*v)*(*o));
    }
    return PXC_STATUS_NO_ERROR;
}

pxcStatus MathPower::CreateInstance(PXCSession *, PXCScheduler *sch, PXCAccelerator *, PXCSessionService::DLLExportTable *, pxcUID, PXCBase **instance) {
    MathPower *math=new MathPower;
    if (!math) return PXC_STATUS_ALLOC_FAILED;
    math->scheduler=sch;
    *instance=(PXCMathAsync*)math;
    return PXC_STATUS_NO_ERROR;
}

/* This table declares the algorithm to the PXC session */
PXCSessionService::DLLExportTable mathPower = {
    0,
    MathPower::CreateInstance,
    PXCSessionService::SUID_DLL_EXPORT_TABLE, 
    {
      PXCSession::IMPL_GROUP_USER,        /* group */
      PXC_UID('M','T','P','W'),           /* subgroup */
      PXC_UID('I','N','V','T'),           /* algorithm */
      PXC_UID('M','T','P','W'),           /* iuid */
      { 1, 0 },                           /* version */
      0xffffffff,                         /* acceleration */
      100,                                /* merit */
      PXC_UID('M','O','R','G'),           /* vendor */
	  { PXCMathAsync::CUID, PXCPowerState::CUID, 0 },	      /* cuids */
      L"Example Math Module with Power Management",
      {0}                                 /* reserved */
    },
};

