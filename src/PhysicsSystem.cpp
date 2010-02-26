#pragma once

#include "PhysicsSystem.h"

#include "testApp.h"
extern testApp* myApp;

PhysicsSystem::PhysicsSystem() {
	printf("PhysicsSystem::PhysicsSystem()\n");
	settings.drawFluid = true;
	settings.drawParticles = true;
	settings.renderUsingVA = true;
}

PhysicsSystem::~PhysicsSystem() {
	printf("PhysicsSystem::~PhysicsSystem()\n");
	destroy();
}

PhysicsSystem& PhysicsSystem::setup() {
	// setup fluid stuff
	fluidSolver.setup(FLUID_WIDTH, FLUID_WIDTH / myApp->window.aspectRatio);
    fluidSolver
		.enableRGB(true)
		.setFadeSpeed(0.002)
		.setDeltaT(0.5)
		.setVisc(0.00015)
		.setColorDiffusion(0);
	fluidDrawer.setup(&fluidSolver);
	fluidDrawer.setDrawMode(FLUID_DRAW_VECTORS);
}

void PhysicsSystem::update() {
	fluidSolver.update();
	if(settings.drawParticles) {
		particleSystem.update();
	}
}

void PhysicsSystem::draw() {
	if(settings.drawFluid) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, myApp->window.width, myApp->window.height);
	}
	if(settings.drawParticles) {
		particleSystem.draw();
	}
}

void PhysicsSystem::destroy() {
	printf("PhysicsSystem::destroy()\n");
}

#pragma mark Custom methods
void fadeToColor(float r, float g, float b, float speed) {
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, speed);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(myApp->window.width, 0);
    glVertex2f(myApp->window.width, myApp->window.height);
    glVertex2f(0, myApp->window.height);
    glEnd();
}

// add force and dye to fluid, and create particles
void PhysicsSystem::addToFluid(float x, float y, float dx, float dy, bool addColor, bool addForce) {
	// balance the x and y components of speed with the screen aspect ratio
    float speed = dx * dx  + dy * dy * myApp->window.aspectRatio2;
	
    if(speed > 0) {
        if(x<0) x = 0; 
        else if(x>1) x = 1;
        if(y<0) y = 0; 
        else if(y>1) y = 1;
		
        float colorMult = 50;
        float velocityMult = 30;
		
        int index = fluidSolver.getIndexForNormalizedPosition(x, y);
		
		if(addColor) {
			msaColor drawColor;
			int hue = lroundf((x + y) * 180 + ofGetFrameNum()) % 360;
			drawColor.setHSV(hue, 1, 1);
			
			fluidSolver.r[index]  += drawColor.r * colorMult;
			fluidSolver.g[index]  += drawColor.g * colorMult;
			fluidSolver.b[index]  += drawColor.b * colorMult;
			
			if(settings.drawParticles)
				particleSystem.addParticles(x * myApp->window.width, y * myApp->window.height, 10);
		}
		
		if(addForce) {
			fluidSolver.u[index] += dx * velocityMult;
			fluidSolver.v[index] += dy * velocityMult;
		}
		
		if(!settings.drawFluid && ofGetFrameNum()%5 ==0)
			fadeToColor(0, 0, 0, 0.1);
    }
}
