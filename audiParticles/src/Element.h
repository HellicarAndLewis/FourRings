//
//  Element.h
//  audiParticles
//
//  Created by James Bentley on 11/30/15.
//
//

#pragma once

#include "ofMain.h"
#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "ParticleSystemGPU.h"

struct element {
    ofParameter<float> particleMaxAge;
    float noiseMagnitude;
    float noisePositionScale;
    float noiseTimeScale;
    float noisePersistence;
    ofVec3f baseSpeed;
    ofColor startColor, endColor, backgroundColor = ofColor(255, 255, 255), foregroundColor = ofColor(255, 255, 255);
    float particleSize;
    ofVec3f cameraPos;
    
    ofImage foreground;
    ofImage background;
    
    ofxPanel gui;
    
    void loadFromFile(string xmlSettingsPath);
    
    void saveToFile(string xmlSettingsPath);
    
    void setFromCurrentSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam);
    
    void setToParticleSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam);
};


