#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

#include "settings.h"

class VideoSystem : public ofBaseUpdates {
public:	
	VideoSystem();
	virtual ~VideoSystem();
	
	VideoSystem& setup();
	void update();
	void draw();
	
	void destroy();
	
#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber 		vidGrabber;
#else
	ofVideoPlayer 		vidPlayer;
#endif
	
	ofxCvColorImage			colorImg;
	
	ofxCvGrayscaleImage 	grayImg;
	ofxCvGrayscaleImage 	grayPrev;
	ofxCvGrayscaleImage 	grayBg;
	ofxCvGrayscaleImage 	grayDiff;

	ofxCvGrayscaleImage		dvX;
	ofxCvGrayscaleImage		dvY;
	ofxCvColorImage			opticalFlowLK;

	ofxCvContourFinder		contourFinder;
	
	struct VideoSettings {
		int		threshold;
		int		dv_min_error;
		int		dv_max_error;
		float	dv_scale;
		bool	bLearnBg;
		int		max_iter;
		int		win_size;
		float	eps;
		int		max_features;
		int		pyr_levels;
		bool	draw_flow;
		bool	calc_flow;
	} settings;
	
protected:
	ofxCvFloatImage			velx;
	ofxCvFloatImage			vely;
	ofxCvGrayscaleImage		dv;
	
	CvSize					img_sz;
	
	IplImage*				eig_image;
	IplImage*				tmp_image;
	
	CvPoint2D32f*			cornersImg;
	CvPoint2D32f*			cornersPrev;
	CvSize					pyr_sz;
	IplImage*				pyrImg;
	IplImage*				pyrPrev;
	
	int						corner_count;
	char					features_found[ MAX_CORNERS ];
	float					feature_errors[ MAX_CORNERS ];

};
