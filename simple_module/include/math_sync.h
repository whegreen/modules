/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#ifndef __MATH_SYNC_H__
#define __MATH_SYNC_H__
#include "pxcbase.h"

/* The magic PXCMathSync interface that inverts */
class PXCMathSync:public PXCBase {  /* an interface definition needs to derive from PXCBase */
public:
	PXC_CUID_OVERWRITE(0x12345678);

    /* PXCMathSync must be a virtual function interfaces. No implementation, static functions or data definitions */
    virtual pxcStatus PXCAPI Invert(pxcF32 v, pxcF32 *o)=0;
};

#endif
