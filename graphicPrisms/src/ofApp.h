#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxIntegrator.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    ofxBulletWorldSoft			world;
    
    ofEasyCam                   camera;
    ofxBulletBox*				ground;
    
    ofxPanel                    gui;
    ofParameter<ofVec3f>        camPos;
    ofParameter<float>          bodyWidth;
    ofParameter<float>          bodyHeight;
    ofParameter<float>          bodyDepth;
    ofParameter<float>          spawnRate;
    ofParameter<ofVec3f>        gravity;
    ofParameter<float>          offSet;
    ofParameter<int>            nearClip;
    ofParameter<int>            farClip;
    ofParameter<float>          thresh;
    ofParameter<ofColor>        foregroundCol;
    ofParameter<float>          lightXRange;
    ofParameter<float>          lightYRange;
    ofParameter<float>          spawnSpread;
    ofParameter<int>            imageDuration;
    ofParameter<float>          attraction;
    ofParameter<int>            maxTorque;
    ofParameter<float>          damping;
    
    int                         lastSpawn;
    bool                        drawGui;
    
    ofxKinect kinect;
    ofImage vignette;
    
    vector< shared_ptr<ofxBulletRigidBody> >    rigidBodies;
    vector< shared_ptr<ofxBulletCustomShape> >  parallelograms;
    
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCvContourFinder contourFinder;
    
    ofImage backgroundImgs[2];
    vector<string> backgroundImgNames;
    int backgroundIndex;
    
    int currentTime;
    
    float fadeAmnt;
    
    ofxCv::FlowFarneback flow;
    
    ofShader shader, fade;
    
    ofFbo fbo, fadePass;
    
    ofMesh mesh;
    
    vector<ofLight*> lights;
    vector< Integrator<ofVec2f> > lightLocs;
    Integrator< float > xParalax;
};
