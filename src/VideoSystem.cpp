#pragma once

#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

VideoSystem::VideoSystem() {
	printf("VideoSystem::VideoSystem()\n");
	settings.threshold		= 80,
	settings.dv_min_error	= 0,
	settings.dv_max_error	= 550,
	settings.dv_scale		= 1.0,
	settings.bLearnBg		= true,	
	settings.max_iter		= 20,
	settings.win_size		= 10,
	settings.eps			= 0.03;
	settings.max_features	= 50;
	settings.pyr_levels		= 5;
	settings.draw_flow		= true;
	settings.calc_flow		= true;
}
VideoSystem::~VideoSystem() {
	printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

VideoSystem& VideoSystem::setup() {
#ifdef _USE_LIVE_VIDEO
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(VIDEO_SIZE);
#else
	vidPlayer.loadMovie("fingers.mov");
	vidPlayer.play();
#endif
	
    colorImg.allocate(VIDEO_SIZE);
	grayImg.allocate(VIDEO_SIZE);
	grayPrev.allocate(VIDEO_SIZE);
	grayBg.allocate(VIDEO_SIZE);
	grayDiff.allocate(VIDEO_SIZE);
	
    velx.allocate(VIDEO_SIZE);
    vely.allocate(VIDEO_SIZE);
    dvX.allocate(VIDEO_SIZE);
    dvY.allocate(VIDEO_SIZE);
    dv.allocate(VIDEO_SIZE);
	
	opticalFlowLK.allocate(VIDEO_SIZE);
	
	cornersImg = new CvPoint2D32f[ MAX_CORNERS ]; 
	cornersPrev = new CvPoint2D32f[ MAX_CORNERS ]; 
	
	img_sz = cvGetSize( grayImg.getCvImage() ); 
	IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 ); 
	IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
	
	pyr_sz = cvSize( grayImg.getCvImage()->width+8, grayPrev.getCvImage()->height/3 ); 
	pyrImg = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 ); 
	pyrPrev = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
	
}

void VideoSystem::update() {
	bool bNewFrame = false;
	
#ifdef _USE_LIVE_VIDEO
	vidGrabber.grabFrame();
	bNewFrame = vidGrabber.isFrameNew();
#else
	vidPlayer.idleMovie();
	bNewFrame = vidPlayer.isFrameNew();
#endif
	
	if (bNewFrame) {
#ifdef _USE_LIVE_VIDEO
		colorImg.setFromPixels(vidGrabber.getPixels(), VIDEO_SIZE);
#else
		colorImg.setFromPixels(vidPlayer.getPixels(), VIDEO_SIZE);
#endif
		colorImg.mirror( false, true );
        grayImg = colorImg;
		if (settings.bLearnBg == true){
			grayBg = grayImg;
			settings.bLearnBg = false;
		}
		
		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImg);
		grayDiff.threshold(settings.threshold);
		
		dv.set(0);
		corner_count = settings.max_features;
		if (settings.calc_flow) {
			cvGoodFeaturesToTrack(grayImg.getCvImage(),
								  eig_image,
								  tmp_image,
								  cornersImg,
								  &corner_count,
								  0.01,
								  5.0,
								  0,
								  3,
								  0,
								  0.04
								  ); 
			cvFindCornerSubPix(grayImg.getCvImage(),
							   cornersImg,
							   corner_count,
							   cvSize(settings.win_size,settings.win_size),
							   cvSize(-1,-1),
							   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
							   );
			
			cvCalcOpticalFlowPyrLK(grayImg.getCvImage(),
								   grayPrev.getCvImage(),
								   pyrImg,
								   pyrPrev,
								   cornersImg,
								   cornersPrev,
								   corner_count,
								   cvSize( settings.win_size,settings.win_size ),
								   settings.pyr_levels,
								   features_found,
								   feature_errors,
								   cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
												  settings.max_iter,
												  settings.eps ),
								   0//CV_LKFLOW_PYR_B_READY | CV_LKFLOW_INITIAL_GUESSES
								   );
			float x,y,  dx,dy;
			for( int i=0; i<corner_count; i++ ) { 
				if( features_found[i]
				   && feature_errors[i]>settings.dv_min_error
				   && feature_errors[i]<settings.dv_max_error ) { 
					x = cornersImg[i].x/VIDEO_WIDTH;
					y = cornersImg[i].y/VIDEO_HEIGHT;
					dx = x - cornersPrev[i].x/VIDEO_WIDTH;
					dy = y - cornersPrev[i].y/VIDEO_HEIGHT;
					myApp->physicsSystem.fluidSolver.addForceAtPos(x, y,
																   dx*settings.dv_scale,
																   dy*settings.dv_scale);
				}
			} 
			IplImage* swp_pyr = pyrPrev;
			pyrPrev = pyrImg;
			pyrImg = swp_pyr;
		}
		grayPrev = grayImg;
	}
	contourFinder.findContours(grayImg, 20, (VIDEO_WIDTH*VIDEO_HEIGHT)/3, 10, false);
}

void VideoSystem::draw() {
	if (settings.draw_flow) {
		ofSetColor(255,0,0);
		ofFill();
		for( int i=0; i<corner_count; i++ ) { 
			if( features_found[i] && feature_errors[i]<550 ) { 
				ofLine(cornersImg[i].x*myApp->window.width/VIDEO_WIDTH,cornersImg[i].y*myApp->window.height/VIDEO_HEIGHT,
					   cornersPrev[i].x*myApp->window.width/VIDEO_WIDTH,cornersPrev[i].y*myApp->window.height/VIDEO_HEIGHT);
			} 
		} 
	}
	CvPoint2D32f* swp_f = cornersPrev;
	cornersPrev = cornersImg;
	cornersImg = swp_f;
}

void VideoSystem::destroy() {
	printf("VideoSystem::destroy()\n");
}
