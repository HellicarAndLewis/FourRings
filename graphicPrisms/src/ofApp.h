#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"

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
    
    int                         lastSpawn;
    bool                        drawGui;
    
    ofxKinect kinect;
    ofImage vignette;
    
    vector< shared_ptr<ofxBulletRigidBody> >    rigidBodies;
    vector< shared_ptr<ofxBulletSoftTriMesh> >  bunnies;
    
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCvContourFinder contourFinder;
    
    ofFbo fbo;
    
    ofMesh mesh;
    
    ofLight light;
};
