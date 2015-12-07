#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxGui.h"

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
    
    ofEasyCam camera;
    ofxBulletBox*				ground;
    
    ofxPanel gui;
    ofParameter<ofVec3f> camPos;
    ofParameter<float> bodyWidth;
    ofParameter<float> bodyHeight;
    ofParameter<float> bodyDepth;


    
    vector< shared_ptr<ofxBulletRigidBody> > rigidBodies;
    vector< shared_ptr<ofxBulletSoftTriMesh> > bunnies;
    
    ofMesh mesh;
    
    ofLight light;
		
};