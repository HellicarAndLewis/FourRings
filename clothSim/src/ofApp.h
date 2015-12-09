#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "Particle.h"
#include "Spring.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxIntegrator.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    vector<Particle> ps;
    vector<Spring> sp;
    
    ofLight light;
    
    ofEasyCam cam;;
    
    ofxKinect kinect;
    
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCv::FlowFarneback flow;
    
    ofMesh mesh;
    
    float noisePoint, noiseStep;
    
    ofxPanel gui;
    ofParameter<float>  nearClip;
    ofParameter<float>  farClip;
    ofParameter<int>    imageDuration;
    
    Integrator<float> xWind;
    Integrator<float> yWind;
    
    ofImage backgroundImgs[2];
    vector<string> backgroundImgNames;
    int backgroundIndex;
    ofFbo fadePass;
    bool drawGui;
    
    int currentTime;
    
    float fadeAmnt;
    
    ofShader fade;
};

#endif