#pragma once

#include "ofMain.h"

#include "settings.h"
#include "ofxMSAFluid.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "msaColor.h"

#define FLUID_WIDTH 150

class PhysicsSystem : public ofBaseUpdates {
public:	
	PhysicsSystem();
	virtual ~PhysicsSystem();

	struct PhysicsSettings {
		bool	drawFluid;
		bool	drawParticles;
		bool	renderUsingVA;
	} settings;
	
	PhysicsSystem& setup();
	void update();
	void draw();

	void destroy();

	
	void addToFluid(float x, float y, float dx, float dy, bool addColor = true, bool addForce = true);

	ofxMSAFluidSolver	fluidSolver;
	ofxMSAFluidDrawer	fluidDrawer;	

	ParticleSystem		particleSystem;

};
