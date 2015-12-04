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

#include "ofxWaterRipple.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
//        void mouseDragged(int x, int y, int button);
    
        void setSpawnPointsPointCloud();
        void setSpawnPointsContours();
	
		float					time;
	
		ParticleSystemGPU		particles;
	
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
    
        ofxPanel                gui;
        ofParameter<int>        input;
        ofParameter<int>        output;
        ofParameter<bool>       flowControl;
        ofParameter<bool>       kinectSpawn;
        ofParameter<int>        nearClip;
        ofParameter<int>        farClip;
        ofParameter<float>      contourFidelity;
	
        ofDirectory elementsDir;

		bool					drawGui;
        bool                    drawCam;
    
        enum INPUT {
            KINECT,
            CAMERA,
            NUM_INPUTS
        };
        
        ofxKinect kinect;
    
        ofxWaterRipple agua;
    
        ofVideoGrabber* grabber;
    
        vector<ofVec3f> spawnPoints;

        ofImage img;
    
        ofxCvGrayscaleImage grayImage;
        ofxCvGrayscaleImage grayThreshNear;
        ofxCvGrayscaleImage grayThreshFar;
    
        ofxCvContourFinder contourFinder;
    
        ofxCv::FlowFarneback flowFinder;
    
        ofFbo fbo;
    
        int lastOutput;
        
        vector<element> elements;
};