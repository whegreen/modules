/*******************************************************************************
	whe_green@yahoo.co.id
	ninjutsu showdown project
	september 2013
*******************************************************************************/


#ifndef PXCUPIPELINE_HEADERS
#define PXCUPIPELINE_HEADERS
#include <vector>
#include "ippi.h"
#include "ippcore.h"
#include "pxcprojection.h"
#include "pxcupipeline.h"
#include "pxcmetadata.h"
#include "util_pipeline.h"
#endif

#ifndef PXCUPIPELINE_INSTANCE
#define PXCUPIPELINE_INSTANCE
struct _PXCUPipeline_Instance {
	_PXCUPipeline_Instance(pxcCHAR *filename=NULL):pipeline(filename) {}

	class UtilPipelinePXCU: public UtilPipeline {
    public:
        UtilPipelinePXCU(pxcCHAR *filename):UtilPipeline(NULL, filename) { memset(&m_vrec_pinfo,0,sizeof(m_vrec_pinfo)); }
    } pipeline;

    std::vector<std::wstring>  strings;
	PXCSmartPtr<PXCProjection> projection;
};
#endif

PXCUPipeline_Instance __stdcall PXCUPipeline_Create() {
    ippInit();
	return new _PXCUPipeline_Instance(NULL);
}

PXCUPipeline_Instance __stdcall PXCUPipeline_CreateFile(pxcCHAR *filename) {
    ippInit();
	return new _PXCUPipeline_Instance(filename);
}

void __stdcall PXCUPipeline_Destroy(PXCUPipeline_Instance instance) {
    delete instance;
}

static bool Init(PXCUPipeline_Instance instance, PXCUPipeline mode) {
	if (mode&PXCU_PIPELINE_COLOR_VGA)           instance->pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32,640,480);
	if (mode&PXCU_PIPELINE_COLOR_WXGA)          instance->pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB24,1280,720);
	if (mode&PXCU_PIPELINE_DEPTH_QVGA)          instance->pipeline.EnableImage(PXCImage::COLOR_FORMAT_DEPTH,320,240);
	if (mode&PXCU_PIPELINE_GESTURE)             instance->pipeline.EnableGesture();
	if (mode&PXCU_PIPELINE_DEPTH_QVGA_60FPS) {
		instance->pipeline.EnableImage(PXCImage::COLOR_FORMAT_DEPTH,320,240);
		PXCSizeU32 size={0,0};
		instance->pipeline.QueryCapture()->SetFilter(PXCImage::IMAGE_TYPE_DEPTH,size,60);
	}
    return instance->pipeline.Init();
}

#ifndef PXCUPIPELINE_INIT
#define PXCUPIPELINE_INIT
bool __stdcall PXCUPipeline_Init(PXCUPipeline_Instance instance, PXCUPipeline mode) {
	return Init(instance,mode);
}
#endif

bool __stdcall PXCUPipeline_AcquireFrame(PXCUPipeline_Instance instance, bool wait) {
	return instance->pipeline.AcquireFrame(wait);
}

void __stdcall PXCUPipeline_ReleaseFrame(PXCUPipeline_Instance instance) {
	instance->pipeline.ReleaseFrame();
}

bool __stdcall PXCUPipeline_IsDisconnected(PXCUPipeline_Instance instance) {
	return instance->pipeline.IsDisconnected();
}

bool __stdcall PXCUPipeline_QueryRGB(PXCUPipeline_Instance instance, unsigned char *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
    if (!image) return false;

    PXCImage::ImageInfo info;
	image->QueryInfo(&info);
    PXCImage::ImageData data2;
	pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,PXCImage::COLOR_FORMAT_RGB32,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    IppiSize roi={ info.width, info.height };
    int dstOrder[3]={2,1,0};
    ippiSwapChannels_8u_AC4R(data2.planes[0],data2.pitches[0],data,data2.pitches[0],roi,dstOrder);
    image->ReleaseAccess(&data2);
    return true;
}

bool __stdcall PXCUPipeline_QueryRGBNative(PXCUPipeline_Instance instance, unsigned char *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
    PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,PXCImage::COLOR_FORMAT_RGB32,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    IppiSize roi={ info.width*4, info.height };
    ippiCopy_8u_C1R(data2.planes[0],data2.pitches[0],data,data2.pitches[0],roi);
    image->ReleaseAccess(&data2);
    return true;
}

bool __stdcall PXCUPipeline_QueryRGBSize(PXCUPipeline_Instance instance, int *width, int *height) {
	return instance->pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_COLOR,*(pxcU32*)width,*(pxcU32*)height);
}

bool __stdcall PXCUPipeline_QueryDepthMapSize(PXCUPipeline_Instance instance, int *width, int *height) {
    return instance->pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH,*(pxcU32*)width,*(pxcU32*)height);
}

bool __stdcall PXCUPipeline_QueryIRMapSize(PXCUPipeline_Instance instance, int *width, int *height) {
    return instance->pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH,*(pxcU32*)width,*(pxcU32*)height);
}

bool __stdcall PXCUPipeline_QueryUVMapSize(PXCUPipeline_Instance instance, int *width, int *height) {
	return instance->pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH,*(pxcU32*)width,*(pxcU32*)height);
}

bool __stdcall PXCUPipeline_QueryLabelMapSize(PXCUPipeline_Instance instance, int *width, int *height) {
    return instance->pipeline.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH,*(pxcU32*)width,*(pxcU32*)height);
}

bool __stdcall PXCUPipeline_QueryDepthMap(PXCUPipeline_Instance instance, short *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    if (data2.planes[0]) {
        IppiSize roi={ info.width, info.height };
        ippiCopy_16u_C1R((Ipp16u*)data2.planes[0],data2.pitches[0],(Ipp16u*)data,data2.pitches[0],roi);
        image->ReleaseAccess(&data2);
        return true;
    } else {
        image->ReleaseAccess(&data2);
        return false;
    }
}

bool __stdcall PXCUPipeline_QueryIRMap(PXCUPipeline_Instance instance, short *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
    PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    if (data2.planes[1]) {
        IppiSize roi={ info.width, info.height };
        ippiCopy_16u_C1R((Ipp16u*)data2.planes[1],data2.pitches[1],(Ipp16u*)data,data2.pitches[1],roi);
        image->ReleaseAccess(&data2);
        return true;
    } else {
        image->ReleaseAccess(&data2);
        return false;
    }
}

bool __stdcall PXCUPipeline_QueryUVMap(PXCUPipeline_Instance instance, float *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
    PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    if (data2.planes[2]) {
        IppiSize roi={ info.width*2, info.height };
        ippiCopy_32f_C1R((Ipp32f*)data2.planes[2],data2.pitches[2],data,data2.pitches[2],roi);
        image->ReleaseAccess(&data2);
        return true;
    } else {
        image->ReleaseAccess(&data2);
        return false;
    }
}

bool __stdcall PXCUPipeline_QueryLabelMap(PXCUPipeline_Instance instance, unsigned char *data, int *labels) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	PXCGesture *gesture=instance->pipeline.QueryGesture();
	if (!gesture) return false;

    PXCSmartPtr<PXCImage> image;
    pxcStatus sts=gesture->QueryBlobImage(PXCGesture::Blob::LABEL_SCENE,0,&image);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    IppiSize roi={ info.width, info.height };
    ippiCopy_8u_C1R((Ipp8u*)data2.planes[0],data2.pitches[0],(Ipp8u*)data,data2.pitches[0],roi);
    image->ReleaseAccess(&data2);

	if (labels) {
		PXCGesture::Blob bdata;
		sts=gesture->QueryBlobData(PXCGesture::Blob::LABEL_SCENE,0,&bdata);
		if (sts<PXC_STATUS_NO_ERROR) return false;
		labels[0]=bdata.labelBackground;
		labels[1]=bdata.labelLeftHand;
		labels[2]=bdata.labelRightHand;
	}
    return true;
}

bool __stdcall PXCUPipeline_QueryDeviceProperty(PXCUPipeline_Instance instance, PXCCapture::Device::Property pty, int npty, float *data) {
	if (instance->pipeline.IsDisconnected()) return false;
	UtilCapture *capture=instance->pipeline.QueryCapture();
	if (!capture) return false;
	PXCCapture::Device *device=capture->QueryDevice();
	if (!device) return false;
	for (int i=0;i<npty;i++,pty=(PXCCapture::Device::Property)(pty+1),data++)
		if (device->QueryProperty(pty,data)<PXC_STATUS_NO_ERROR) return false;
	return true;
}

bool __stdcall PXCUPipeline_SetDeviceProperty(PXCUPipeline_Instance instance, PXCCapture::Device::Property pty, int npty, float *data) {
	if (instance->pipeline.IsDisconnected()) return false;
	UtilCapture *capture=instance->pipeline.QueryCapture();
	if (!capture) return false;
	PXCCapture::Device *device=capture->QueryDevice();
	if (!device) return false;
	for (int i=0;i<npty;i++,pty=(PXCCapture::Device::Property)(pty+1),data++)
		if (device->SetProperty(pty,*data)<PXC_STATUS_NO_ERROR) return false;
	return true;
}

bool __stdcall PXCUPipeline_QueryGeoNode(PXCUPipeline_Instance instance, PXCGesture::GeoNode::Label body, PXCGesture::GeoNode *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	PXCGesture *gesture=instance->pipeline.QueryGesture();
	if (!gesture) return false;
    return gesture->QueryNodeData(0,body,data)>=PXC_STATUS_NO_ERROR;
}

bool __stdcall PXCUPipeline_QueryGesture(PXCUPipeline_Instance instance, PXCGesture::GeoNode::Label body, PXCGesture::Gesture *data) {
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	PXCGesture *gesture=instance->pipeline.QueryGesture();
	if (!gesture) return false;
	return gesture->QueryGestureData(0,body,0,data)>=PXC_STATUS_NO_ERROR;
}

bool __stdcall PXCUPipeline_QueryMatchHandseal(PXCUPipeline_Instance instance, int *handsealIdx, bool *matchResult){
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	
	short *data;

	PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    if (data2.planes[0]) {
        IppiSize roi={ info.width, info.height };
        ippiCopy_16u_C1R((Ipp16u*)data2.planes[0],data2.pitches[0],(Ipp16u*)data,data2.pitches[0],roi);

		//potong bagian terdepan yang kedeteksi kamera

		//panggil surf

		//load modul surf berdasarkan idx

		//compare pakai flann

        image->ReleaseAccess(&data2);
        return true;
    } else {
        image->ReleaseAccess(&data2);
        return false;
	}
	
    return true;
}

bool __stdcall PXCUPipeline_QueryHandseal(PXCUPipeline_Instance instance, int *handsealIdx){
	if (instance->pipeline.IsDisconnected()) return false;
    if (!instance->pipeline.IsImageFrame()) return false;
	
	short *data;

	PXCImage *image=instance->pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
    if (!image) return false;

    PXCImage::ImageInfo info;
    image->QueryInfo(&info);
    PXCImage::ImageData data2;
    pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ,&data2);
    if (sts<PXC_STATUS_NO_ERROR) return false;

    if (data2.planes[0]) {
        IppiSize roi={ info.width, info.height };
        ippiCopy_16u_C1R((Ipp16u*)data2.planes[0],data2.pitches[0],(Ipp16u*)data,data2.pitches[0],roi);

		//potong bagian terdepan yang kedeteksi kamera

		//panggil surf

		//looping sebanyak 12 handseal
			//load modul surf berdasarkan idx
			//compare pakai flann
			//compare untuk dapat nilai maksimum hasil flann

        image->ReleaseAccess(&data2);
        return true;
    } else {
        image->ReleaseAccess(&data2);
        return false;
	}
	
    return true;
}

void __stdcall PXCUPipeline_Close(PXCUPipeline_Instance instance) {
	instance->projection.ReleaseRef();
	instance->pipeline.Close();
    instance->strings.clear();
}

static void GetProjectionInterface(PXCUPipeline_Instance instance) {
	UtilCapture *capture=instance->pipeline.QueryCapture();
	if (!capture) return;
	PXCCapture::Device *device=capture->QueryDevice();
	if (!device) return;
	pxcUID pid=0;
	pxcStatus sts=device->QueryPropertyAsUID(PXCCapture::Device::PROPERTY_PROJECTION_SERIALIZABLE,&pid);
	if (sts<PXC_STATUS_NO_ERROR) return;
	PXCSession *session=instance->pipeline.QuerySession();
	if (!session) return;
	PXCMetadata *metadata=session->DynamicCast<PXCMetadata>();
	if (!metadata) return;
	metadata->CreateSerializable<PXCProjection>(pid,&instance->projection);
}

bool __stdcall PXCUPipeline_ProjectImageToRealWorld(PXCUPipeline_Instance instance, pxcU32 npoints, PXCPoint3DF32 *pos2d, PXCPoint3DF32 *pos3d) {
	if (!instance->projection.IsValid()) {
		GetProjectionInterface(instance);
		if (!instance->projection.IsValid()) return false;
	}
	return instance->projection->ProjectImageToRealWorld(npoints,pos2d,pos3d)>=PXC_STATUS_NO_ERROR;
}

bool __stdcall PXCUPipeline_ProjectRealWorldToImage(PXCUPipeline_Instance instance, pxcU32 npoints, PXCPoint3DF32 *pos3d, PXCPointF32 *pos2d) {
	if (!instance->projection.IsValid()) {
		GetProjectionInterface(instance);
		if (!instance->projection.IsValid()) return false;
	}
	return instance->projection->ProjectRealWorldToImage(npoints,pos3d,pos2d)>=PXC_STATUS_NO_ERROR;
}

bool __stdcall PXCUPipeline_MapDepthToColorCoordinates(PXCUPipeline_Instance instance, pxcU32 npoints, PXCPoint3DF32 *pos2d, PXCPointF32 *posc) {
	if (!instance->projection.IsValid()) {
		GetProjectionInterface(instance);
		if (!instance->projection.IsValid()) return false;
	}
	return instance->projection->MapDepthToColorCoordinates(npoints,pos2d,posc)>=PXC_STATUS_NO_ERROR;
}

static void PauseFrame(PXCUPipeline_Instance instance, PXCUPipeline mode, bool pause) {
	if (mode&PXCU_PIPELINE_GESTURE)             instance->pipeline.PauseGesture(pause);
}

#ifndef PXCUPIPELINE_PAUSEFRAME
#define PXCUPIPELINE_PAUSEFRAME
void __stdcall PXCUPipeline_PauseFrame(PXCUPipeline_Instance instance, PXCUPipeline mode, bool pause) {
	PauseFrame(instance,mode,pause);
}
#endif

