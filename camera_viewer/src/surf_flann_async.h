/*******************************************************************************
	whe_green@yahoo.co.id
	ninjutsu showdown project
	september 2013
*******************************************************************************/

#ifndef __SURF_FLANN_ASYNC_H__
#define __SURF_FLANN_ASYNC_H__
#include "pxcscheduler.h"

class PXCSURFFLANNAsync:public PXCBase {  /* an interface definition needs to derive from PXCBase */
public:
	PXC_CUID_OVERWRITE(0x22101989);

    /* a virtual function interfaces. No implementation, static functions or data definitions */
	virtual pxcStatus PXCAPI HandsealDetectAsync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;
	virtual pxcStatus PXCAPI HandsealDetectAsync2(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;
	virtual pxcStatus PXCAPI RecordSampleAsync(PXCImage *frame, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;
};

#endif
