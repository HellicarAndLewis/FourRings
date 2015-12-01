#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "ParticleSystemGPU.h"

#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxIntegrator.h"

#include "element.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
    
        void drawPointCloud();
	
		float					time;
	
		ParticleSystemGPU		particles;
	
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
    
        ofxPanel                gui;
        ofParameter<int>        input;
        ofParameter<int>        output;
	
        ofDirectory elementsDir;

		bool					drawGui;
    
        enum INPUT {
            KINECT,
            CAMERA,
            NUM_INPUTS
        };
        
        enum OUTPUT {
            FIRE,
            WATER,
            EARTH,
            AIR,
            NUM_OUTPUTS
        };
        
        ofxKinect kinect;
        
        ofVideoGrabber* grabber;
        
        ofImage img;
    
        ofxCv::FlowFarneback flowFinder;
            
        int lastOutput;
        
        vector<element> elements;
};