/*******************************************************************************

*******************************************************************************/

#ifndef __SURF_FLANN_ASYNC_H__
#define __SURF_FLANN_ASYNC_H__
//#include "pxcbase.h"
#include "pxcscheduler.h"
//#include "opencv2/core/core.hpp"

class PXCSURFFLANNAsync:public PXCBase {  /* an interface definition needs to derive from PXCBase */
public:
	PXC_CUID_OVERWRITE(0x22101989);

    /* a virtual function interfaces. No implementation, static functions or data definitions */
	virtual pxcStatus PXCAPI HandsealDetectAsync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp)=0;
	//virtual pxcStatus PXCAPI HandsealDetectAsync2(PXCImage *frame, PXCImage **oFrame, PXCScheduler::SyncPoint **sp)=0;
	virtual pxcStatus PXCAPI HandsealDetectAsync2(PXCImage *frame, IplImage *oFrame, PXCScheduler::SyncPoint **sp)=0;
};

#endif
