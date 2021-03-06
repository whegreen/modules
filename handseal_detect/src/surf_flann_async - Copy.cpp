/*******************************************************************************



*******************************************************************************/

#include "service/pxcsessionservice.h"
#include "service/pxcschedulerservice.h"
#include "service/pxcsmartasyncimpl.h"
#include "surf_flann_async.h"

#include <stdio.h>
#include <iostream>

//#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

using namespace std;
using namespace cv;

/* This is the implementation class */
class SURFFLANNAsync:public PXCBaseImpl<PXCSURFFLANNAsync> { /* The implementation class must derive from .h file */
public:
    SURFFLANNAsync(PXCScheduler *scheduler) { this->scheduler=scheduler; }
	
	//define void untuk dipanggil dari luar file DLL
	virtual pxcStatus PXCAPI HandsealDetectAsync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o, PXCScheduler::SyncPoint **sp);
	virtual pxcStatus PXCAPI HandsealDetectAsync2(PXCImage *frame, Mat *oFrame, PXCScheduler::SyncPoint **sp);

    static  pxcStatus PXCAPI CreateInstance(PXCSession *session, PXCScheduler *sch, PXCAccelerator *accel, PXCSessionService::DLLExportTable *table, pxcUID cuid, PXCBase **instance);
    
	//define void internal untuk dipanggil setelah async siap
	virtual pxcStatus PXCAPI HandsealDetectSync(PXCImage *frame, pxcF32 *templateIdx, pxcF32 *o);
	virtual pxcStatus PXCAPI HandsealDetectSync2(PXCImage *frame, Mat *oFrame);

protected:

    PXCScheduler *scheduler;
};

/*
input  : 
	hasil image depth stream dalam format opencv::mat
	location file image template handseal
output : 
	ada result mirip atau ngga, disimpan didalam array
*/

pxcStatus SURFFLANNAsync::HandsealDetectSync2(PXCImage *image, Mat *oFrame) {
	if (!(image)) return PXC_STATUS_PARAM_UNSUPPORTED;
	//if (!(templateIdx)) return PXC_STATUS_PARAM_UNSUPPORTED;
	
	//handseal detect code here
	//Mat img_1 = imread( "full2.png", IMREAD_GRAYSCALE );
	PXCImage::ImageData data;

	image->AcquireAccess(PXCImage::ACCESS_READ_WRITE,&data);
	IplImage* colorimg = cvCreateImage(cvSize(320, 240), 16, 1);
	cvSetData(colorimg, (short*)data.planes[0], 320*sizeof(short)*1);
	Mat img_ = cvarrToMat(colorimg);
	
	//Mat img_1 = frame;
	Mat img_1 = imread( "full2.png", IMREAD_GRAYSCALE );
	Mat img_2 = imread( "sample2.png", IMREAD_GRAYSCALE );

	if( !img_1.data || !img_2.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return PXC_STATUS_PARAM_UNSUPPORTED; }

		//printf("image udah ke load ");
	  //-- Step 1: Detect the keypoints using SURF Detector
	  //-- Makin tinggi makin sedikit keypoint, noise makin berkurang
	  //-- todo: set best point untuk minHessian 
	  
	  int minHessian = 400;
	
	  SurfFeatureDetector detector( minHessian );

	  std::vector<KeyPoint> keypoints_1, keypoints_2;

	  detector.detect( img_1, keypoints_1 );
	  detector.detect( img_2, keypoints_2 );

	  //-- Step 2: Calculate descriptors (feature vectors)
	  SurfDescriptorExtractor extractor;

	  Mat descriptors_1, descriptors_2;

	  extractor.compute( img_1, keypoints_1, descriptors_1 );
	  extractor.compute( img_2, keypoints_2, descriptors_2 );

	  //-- Step 3: Matching descriptor vectors using FLANN matcher
	  FlannBasedMatcher matcher;
	  std::vector< DMatch > matches;
	  matcher.match( descriptors_1, descriptors_2, matches );

	  double max_dist = 0; double min_dist = 100;

	  //-- Quick calculation of max and min distances between keypoints
	  for( int i = 0; i < descriptors_1.rows; i++ )
	  { double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	  }

	  printf("-- Max dist : %f \n", max_dist );
	  printf("-- Min dist : %f \n", min_dist );

	  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
	  //-- PS.- radiusMatch can also be used here.
	  std::vector< DMatch > good_matches;
	  
	  
	  for( int i = 0; i < descriptors_1.rows; i++ )
	  { if( matches[i].distance <= 2*min_dis   SCCA   b  G I T - S H - I 1 8 N - - E N V S U B S T . E X E   @��    7)E�    �      �  7  $  �  �     z        �2.��                                                                                                          L   L       2      �    L   )   )   f   5      @J    u         �   9      e7    v         F  7      >J    �   '   '   �  3      ��    �           �      �     �         P  5      �    �   *   *   �  3      �$    
        $  4      f@            �  3      [�       	   	   �  �      s    )  	   	   �  3      �    2        `  0      �>       Z      z      �      �      �                2  	   :  
   J     R     Z     j     r     �     �     �     �     �     �     �     �     �     �     �     �     �          
                "  !   *  "   2  #   :  $   B  %   J  &   b  '   �  (   �  )   �  *   �  +   �  ,   �  -   �  .   �  /   �  0   �  1   �  2   �  3   �  4   �  5     6   
  7     8     9   "  :   *  ;   2  <   :  =   J  >   Z  ?   b  @   j  A   r  B   z  C   �  D   �  E   �  F   �  G   �  H   �  I   �  J   �  K   �  �����  M       N      O      P   �  Q   �  R   �  S   �  T   �  U   (  V   @  W   H  X   `  Y   h  Z   p  [   x  \   �  ]   �  ^   �  _   �  `   �  a   P  b     c   @  d   �  e   �  f   �  g   �  h   �  i   �  j   �  k   �  l   �  m   �  n   �  o      p     q     r     s      t   (  ����0  ����   w       x      y   
   z      {      |   "   }   *   ~   2      :   �   B   �   J   �   R   �   Z   �   b   �   j   �   r   �   z   �   �   �   �   �   �   �   �   �     �   
  �     �     �     ����%  �       �      �      �      �       �   (   �   0   �   8   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �      �     �     �     �       �   `  �   h  �   p  �   x  �   �  �   �  �   �  �   �  �   �  �   �  �   �  �   �  �      �     �     ����  �      �   
   �      �      �   *   �   :   �   B   �   J   �   Z   �   b   �   j   �   �  �   �  �   �  �   �  �   v  �   y  �����  �       �      �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �     �   #  �   +  �   s  �   {  �   �  �   �  �   �  �   �  �����  �       �      �      �      �      �   $   �   ,   �   4   �   <   �   D   �   L   �   T   �   \   �   d   �   t   �   �   �   �   �   �   �   �   �   �   �   �   �   �    �   �   �   �   �   �   �     �   ,   �   4   �   <  �     �   T      �    $    �     �    �    �              	    ����)                    K     �     �     �   �����                �     �    �    �    �    �    �    �    �    �    �  �����  !      "     #  �   $  �   %  �   &  �  '  �  (  �  ����  *      +     ,     -  k   .  �   /  #  0  +  1  2  ����5  3      4     5  
   6     ����   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ N T D L L . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ K E R N E L 3 2 . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ A P I S E T S C H E M A . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ K E R N E L B A S E . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ L O C A L E . N L S   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ U S E R S \ G R E E N \ A P P D A T A \ L O C A L \ G I T H U B \ P O R T A B L E G I T _ 0 1 5 A A 7 1 E F 1 8 C 0 4 7 C E 8 5 0 9 F F B 2 F 9 E 4 B B 0 E 3 E 7 3 F 1 3 \ L I B E X E C \ G I T - C O R E \ G I T - S H - I 1 8 N - - E N V S U B S T . E X E   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ A D V A P I 3 2 . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ M S V C R T . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \ S E C H O S T . D L L   \ D E V I C E \ H A R D D I S K V O L U M E 2 \ W I N D O W S \ S Y S T E M 3 2 \