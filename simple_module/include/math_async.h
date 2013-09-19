/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#ifndef __MATH_ASYNC_H__
#define __MATH_ASYNC_H__
//#include "pxcbase.h"
#include "pxcscheduler.h"

/* The magic PXCMathAsync interface that inverts */
class PXCMathAsync:public PXCBase {  /* an interface definition needs to derive from PXCBase */
public:
	PXC_CUID_OVERWRITE(0x22101989);

    /* PXCMathAsync must be a virtual function interfaces. No implementation, static functions or data definitions */
    virtual pxcStatus PXCAPI InvertAsync(pxcF32 *v, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;

	/* PXCMathAsync must be a virtual function interfaces. No implementation, static functions or data definitions */
    virtual pxcStatus PXCAPI InvertAsync2(pxcF32 *v1, pxcF32 *v2, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;
};

#endif
