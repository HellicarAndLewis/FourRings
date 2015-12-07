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
    string name;
    ofVec2f baseSpeedInfluence;
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
    bool kinectSpawn;
    float spawnFidelity;
    float vignetteOffset;
    
    ofImage foreground;
    ofImage background;
        
    void loadFromFile(string xmlSettingsPath);
    
    void saveToFile(string xmlSettingsPath);
    
    void setFromCurrentSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam, ofParameter<bool>* _kinectSpawn, ofParameter<float>* _vignetteOffset);
    
    void setToParticleSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam, ofParameter<bool>* _kinectSpawn, ofParameter<float>* _vignetteOffset);
    
    void updateFromFlow(ofVec3f flow);
};


