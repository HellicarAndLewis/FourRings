//
//  Element.cpp
//  audiParticles
//
//  Created by James Bentley on 11/30/15.
//
//

#include "Element.h"

void element::loadFromFile(string xmlSettingsPath) {
    ofXml xml;
    xml.load(xmlSettingsPath);
    
    name = ofSplitString(xmlSettingsPath, "/")[2];
    
    foreground.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    background.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    
    particleMaxAge = ofToFloat(xml.getValue("Particle_Max_Age"));
    noiseMagnitude = ofToFloat(xml.getValue("Noise_Magnitude"));
    noisePositionScale = ofToFloat(xml.getValue("Noise_Position_Scale"));
    noisePersistence = ofToFloat(xml.getValue("Noise_Persistence"));
    noiseTimeScale = ofToFloat(xml.getValue("Noise_Time_Scale"));
    particleSize = ofToFloat(xml.getValue("Particle_Size"));
    timeStep = ofToFloat(xml.getValue("Time_Step"));
    vignetteOffset = ofToFloat(xml.getValue("Vignette_Offset"));
    
    
    vector<string> baseSpeedSplit = ofSplitString(xml.getValue("Wind"), ",");
    baseSpeed = ofVec3f(ofToFloat(baseSpeedSplit[0]), ofToFloat(baseSpeedSplit[1]), ofToFloat(baseSpeedSplit[2]));
    
    vector<string> colorSplit = ofSplitString(xml.getValue("Start_Color"), ",");
    startColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));
    
    colorSplit = ofSplitString(xml.getValue("End_Color"), ",");
    endColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));

    colorSplit = ofSplitString(xml.getValue("Background_Color"), ",");
    backgroundColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));
    
    colorSplit = ofSplitString(xml.getValue("Foreground_Color"), ",");
    foregroundColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));
    
    vector<string> cameraPosSplit = ofSplitString(xml.getValue("Camera_Pos"), ",");
    cameraPos = ofVec3f(ofToFloat(cameraPosSplit[0]), ofToFloat(cameraPosSplit[1]), ofToFloat(cameraPosSplit[2]));
    
    vector<string> influenceSplit = ofSplitString(xml.getValue("Wind_Influence"), ",");
    baseSpeedInfluence = ofVec2f(ofToFloat(influenceSplit[0]), ofToFloat(influenceSplit[1]));
    
    foregroundLoaded = foreground.loadImage(xml.getValue("Foreground_Path"));
    backgroundLoaded = background.loadImage(xml.getValue("Background_Path"));
    
    spawnFidelity = ofToFloat(xml.getValue("Spawn_Fidelity"));
    
    kinectSpawn = ofToBool(xml.getValue("Kinect_Spawn"));
}

void element::saveToFile(string xmlSettingsPath) {
    ofXml xml;
    xml.load(xmlSettingsPath);
    xml.setValue("Particle_Max_Age", ofToString(particleMaxAge));
    xml.setValue("Noise_Magnitude", ofToString(noiseMagnitude));
    xml.setValue("Noise_Position_Scale", ofToString(noisePositionScale));
    xml.setValue("Noise_Persistence", ofToString(noisePersistence));
    xml.setValue("Noise_Time_Scale", ofToString(noiseTimeScale));
    xml.setValue("Particle_Size", ofToString(particleSize));
    xml.setValue("Wind", ofToString(baseSpeed));
    xml.setValue("Start_Color", ofToString(startColor));
    xml.setValue("End_Color", ofToString(endColor));
    xml.setValue("Camera_Pos", ofToString(cameraPos));
    xml.setValue("Time_Step", ofToString(timeStep));
    xml.setValue("Foreground_Color", ofToString(foregroundColor));
    xml.setValue("Background_Color", ofToString(backgroundColor));
    xml.setValue("Wind_Influence", ofToString(baseSpeedInfluence));
    xml.setValue("Spawn_Fidelity", ofToString(spawnFidelity));
    xml.setValue("Kinect_Spawn", ofToString(kinectSpawn));
    xml.setValue("Vignette_Offset", ofToString(vignetteOffset));
    xml.save(xmlSettingsPath);
}

void element::setFromCurrentSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam, ofParameter<bool>* _kinectSpawn, ofParameter<float>* _vignetteOffset) {
    particleMaxAge = particleSystem->particleMaxAge;
    noiseMagnitude = particleSystem->noiseMagnitude;
    noisePositionScale = particleSystem->noisePositionScale;
    noiseTimeScale = particleSystem->noiseTimeScale;
    noisePersistence = particleSystem->noisePersistence;
    baseSpeed = particleSystem->baseSpeed;
    startColor = particleSystem->startColor;
    endColor = particleSystem->endColor;
    particleSize = particleSystem->particleSize;
    timeStep = particleSystem->timeStep;
    cameraPos = cam->getPosition();
    foregroundColor = particleSystem->foregroundColor;
    backgroundColor = particleSystem->backgroundColor;
    baseSpeedInfluence = particleSystem->baseSpeedInfluence;
    spawnFidelity = particleSystem->spawnFidelity;
    kinectSpawn = *_kinectSpawn;
    vignetteOffset = *_vignetteOffset;
}

void element::setToParticleSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam, ofParameter<bool>* _kinectSpawn, ofParameter<float>* _vignetteOffset) {
    particleSystem->particleMaxAge = particleMaxAge;
    particleSystem->noiseMagnitude = noiseMagnitude;
    particleSystem->noisePositionScale = noisePositionScale;
    particleSystem->noiseTimeScale = noiseTimeScale;
    particleSystem->noisePersistence = noisePersistence;
    particleSystem->baseSpeed = baseSpeed;
    particleSystem->startColor = startColor;
    particleSystem->endColor = endColor;
    particleSystem->particleSize = particleSize;
    particleSystem->timeStep = timeStep;
    cam->setPosition(cameraPos);
    particleSystem->foregroundColor = foregroundColor;
    particleSystem->backgroundColor = backgroundColor;
    particleSystem->baseSpeedInfluence = baseSpeedInfluence;
    particleSystem->spawnFidelity = spawnFidelity;
    *_kinectSpawn = kinectSpawn;
    *_vignetteOffset = vignetteOffset;
}