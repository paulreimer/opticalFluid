#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void
testApp::setup()
{
	// initialize stuff according to current window size
	windowResized(ofGetWidth(), ofGetHeight());	
	
	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(true);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

#ifdef USE_VIDEO
	videoSystem.setup();
#endif

#ifdef USE_PHYSICS
	physicsSystem.setup();
#endif
	
#ifdef USE_GUI
	gui.setup();	
	//	gui.addPage("Fluid");

	gui.addToggle("Draw Fluid",
				  physicsSystem.settings.drawFluid);
	gui.addSlider("fs.viscocity",
				  physicsSystem.fluidSolver.viscocity, 0.0, 0.0002, 0.5);
	gui.addSlider("fs.colorDiffusion",
				  physicsSystem.fluidSolver.colorDiffusion, 0.0, 0.0003, 0.5);
	gui.addSlider("fs.fadeSpeed",
				  physicsSystem.fluidSolver.fadeSpeed, 0.0, 0.1, 0.5);
	gui.addSlider("fs.solverIterations",
				  physicsSystem.fluidSolver.solverIterations, 1, 20);
	gui.addSlider("fd.drawMode",
				  physicsSystem.fluidDrawer.drawMode, 0, FLUID_DRAW_MODE_COUNT-1);
	gui.addToggle("fs.doRGB",
				  physicsSystem.fluidSolver.doRGB);
	gui.addToggle("fs.doVorticityConfinement",
				  physicsSystem.fluidSolver.doVorticityConfinement);
	gui.addToggle("drawFluid",
				  physicsSystem.settings.drawFluid);
	gui.addToggle("drawParticles",
				  physicsSystem.settings.drawParticles); 
	gui.addToggle("renderUsingVA",
				  physicsSystem.settings.renderUsingVA); 

	gui.addPage("Camera");
	gui.addContent("Color Image",
				   videoSystem.colorImg, (float)-1);
	gui.addContent("Gray Diff",
				   videoSystem.grayDiff, (float)-1);
	gui.addContent("Vector Field [x]",
				   videoSystem.dvX, (float)-1);
	gui.addContent("Vector Field [y]",
				   videoSystem.dvY, (float)-1);
	
	gui.addSlider("Threshold",
				  videoSystem.settings.threshold, 0, 255);
	gui.addButton("Capture Bg",
				  videoSystem.settings.bLearnBg);
	
	gui.addPage("Optical Flow");
	gui.addToggle("Calc. Flow",
				  videoSystem.settings.calc_flow);
	gui.addToggle("Draw Flow",
				  videoSystem.settings.draw_flow);
	gui.addSlider("Max Features",
				  videoSystem.settings.max_features, 0, MAX_CORNERS);
	gui.addSlider("Dist. Vec. Scale",
				  videoSystem.settings.dv_scale, 0.0, 10.0, 0.1);
	gui.addSlider("Dist. Vec. Threshold",
				  videoSystem.settings.dv_min_error, 0, 512);
	gui.addSlider("Dist. Vec. Threshold",
				  videoSystem.settings.dv_max_error, 512, 1024);	
	
	gui.addContent("Contours", videoSystem.contourFinder);
#endif
}

//--------------------------------------------------------------
void
testApp::update()
{
#ifdef USE_VIDEO
	videoSystem.update();
#endif
	
#ifdef USE_PHYSICS
	physicsSystem.update();
#endif

	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;
}

//--------------------------------------------------------------
void
testApp::draw()
{
#ifdef USE_PHYSICS
	physicsSystem.draw();
#endif

#ifdef USE_GUI
	gui.draw();
#endif

#ifdef USE_VIDEO
	videoSystem.draw();
#endif
}

void
testApp::windowResized(int w, int h)
{
	printf("TEST windowResized(%i, %i)\n", w, h);
	window.width		= w;
	window.height		= h;
	
	window.invWidth		= 1.0f/window.width;
	window.invHeight	= 1.0f/window.height;
	window.aspectRatio	= window.width * window.invHeight;
	window.aspectRatio2 = window.aspectRatio * window.aspectRatio;
}


#pragma mark Input callbacks

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
    switch(key) {
#ifdef USE_GUI
		case ' ':
			gui.toggleDraw();
			glClear(GL_COLOR_BUFFER_BIT);
			break;
#endif			
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			static char fileNameStr[255];
			sprintf(fileNameStr, "output_%0.4i.png", ofGetFrameNum());
			static ofImage imgScreen;
			imgScreen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			printf("Saving file: %s\n", fileNameStr);
			imgScreen.saveImage(fileNameStr);
			break;
		case '1':
			videoSystem.vidGrabber.videoSettings();
			break;
	}
}

//--------------------------------------------------------------
void
testApp::mouseMoved(int x, int y)
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;

    physicsSystem.addToFluid(mouseNormX, mouseNormY, mouseVelX, mouseVelY, true);
}

//--------------------------------------------------------------
void
testApp::mouseDragged(int x, int y, int button)
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
	
	physicsSystem.addToFluid(mouseNormX, mouseNormY, mouseVelX, mouseVelY, false);
}
