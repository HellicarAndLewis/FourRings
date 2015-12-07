#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ofxKinect.h"

class ofApp : public ofBaseApp{
	public:
    struct glow {
        int lifeTime;
        int birthday;
        ofColor col;
        ofVec2f loc;
        ofVec2f offSet;
        float noiseStartX, noiseStartY;
        float noiseStep;
        float maxWobble;
        int rad;
        int maxRad = 100;
        int alpha;
        bool alive;
        void update() {
            if(ofGetElapsedTimeMillis() - birthday > lifeTime) {
                alpha--;
            } else if(alpha < 127) {
                alpha+=10;
            }
            if(alpha <= 0) {
                alive = false;
            }
            noiseStartX += noiseStep;
            noiseStartY += noiseStep;
            offSet.x = ofNoise(noiseStartX);
            offSet.y += 0.08; //ofNoise(noiseStartY);
        };
        void emerge(ofVec2f _loc, ofColor _col) {
            loc = _loc;
            col = _col;
            birthday = ofGetElapsedTimeMillis();
            lifeTime = ofRandom(1.5*1000, 4*1000);
            rad = ofRandom(30, maxRad);
            alive = true;
            noiseStartX = ofRandom(100000);
            noiseStartY = ofRandom(100000);
            maxWobble = ofRandom(1, 2);
            offSet.x = ofNoise(noiseStartX);
            offSet.y = 1.0;//ofNoise(noiseStartY);
            noiseStep = 0.05;
        };
        
        void draw(ofImage* img) {
            ofPushStyle();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(col.r, col.g, col.b, alpha);
//            float xBump = ofRandom(-1, 1);
//            float yBump = ofRandom(-1, 1);
//            ofTranslate(loc.x, loc.y);
//            ofTranslate(xBump, yBump);
//            ofRotate(offSet.y, 0, 0, 1);
//            ofTranslate(-xBump, -yBump);
            float xBump = 0.0;//ofMap(offSet.x, 0, 1.0, -maxWobble, maxWobble);
            float yBump = ofMap(offSet.y, 0, 1.0, 0, maxWobble);
            img->draw(loc.x + xBump, loc.y + yBump);
            //ofCircle(loc.x + xBump, loc.y + yBump, rad);
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofPopStyle();
        }
    };
		void setup();
		void update();
		void draw();
    void drawOnContours(vector<ofxCvBlob> blobs);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxPanel gui;
    ofParameter<int> nearClip;
    ofParameter<int> farClip;
    
    ofxKinect kinect;
    
    bool drawGui;
    bool drawCam;
    ofImage img;
    ofxAutoReloadedShader bokeh;
    
    ofxCvGrayscaleImage grayImage, grayThreshNear, grayThreshFar;
    ofxCvContourFinder contourFinder;
    
    ofFbo background;
    
    ofImage glowImg;
    
    vector<glow> glows;
};
