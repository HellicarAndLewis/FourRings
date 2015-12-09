#pragma once

#include "ofMain.h"
#include "ofxWaterRipple.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void disturbOnContours(vector<ofxCvBlob> blobs, ofxWaterRipple* agua);
        void disturbOnCentroid(vector<ofxCvBlob> blobs, ofxWaterRipple* agua);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        bool drawGui;
        bool drawCam;
    
		ofxWaterRipple agua;
    
        int backgroundIndex, currImg;
    
        int currentTime;
    
        float fadeAmnt;
    
        ofxPanel gui;
        ofParameter<int> nearClip;
        ofParameter<int> farClip;
        ofParameter<int> profundidad;
        ofParameter<int> radio;
        ofParameter<bool> useCentroid;
        ofParameter<bool> useContours;
        ofParameter<ofColor> col1;
        ofParameter<int> imageDuration;
    
        ofImage backgroundImgs[2];
        vector<string> backgroundImgNames;
    
        ofShader overLay, fade;
    
        ofFbo flatBackground;
        ofFbo fadePass;
    
        ofImage img;
        ofxCvGrayscaleImage grayImage, grayThreshNear, grayThreshFar;
    
        ofxKinect kinect;
        ofxCvContourFinder contourFinder;
};
