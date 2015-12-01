//
//  Element.h
//  audiParticles
//
//  Created by James Bentley on 11/30/15.
//123
//

#pragma once

#include "ofMain.h"
#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "ParticleSystemGPU.h"

struct element {
    ofVec3f noiseMagnitudeInfluence;
    float particleMaxAge;
    float noiseMagnitude;
    float noisePositionScale;
    float noiseTimeScale;
    float noisePersistence;
    ofVec3f baseSpeed;
    ofColor startColor, endColor, backgroundColor, foregroundColor;
    bool backgroundLoaded, foregroundLoaded;
    float particleSize;
    float timeStep;
    ofVec3f cameraPos;
    
    ofImage foreground;
    ofImage background;
        
    void loadFromFile(string xmlSettingsPath);
    
    void saveToFile(string xmlSettingsPath);
    
    void setFromCurrentSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam);
    
    void setToParticleSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam);
    
    void updateFromFlow(ofVec3f flow);
};


