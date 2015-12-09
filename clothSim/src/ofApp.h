#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "Particle.h"
#include "Spring.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    vector<Particle> ps;
    vector<Spring> sp;
    
    ofLight light;
    
//    ofCamera cam;
    ofEasyCam cam;;
    
    ofMesh mesh;
};

#endif