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
    
    foreground.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    background.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

    //    gui.setup("Element", xmlSettingsPath);
//    
//    gui.add( particleMaxAge.set("Particle_Max_Age", ofToFloat(xml.getValue("Particle_Max_Age")), 0.0f, 20.0f));
//    gui.add( particleMaxAge.set("Particle Max Age", 10.0f, 0.0f, 20.0f) );
//    gui.add( noiseMagnitude.set("Noise Magnitude", ofToFloat(xml.getValue("Noise_Magnitude")), 0.01f, 2.0f) );
//    gui.add( noisePositionScale.set("Noise Position Scale", ofToFloat(xml.getValue("Noise_Position_Scale")), 0.01f, 10.0f) );
//    gui.add( noiseTimeScale.set("Noise Time Scale", ofToFloat(xml.getValue("Noise_Time_Scale")), 0.001f, 1.0f) );
//    gui.add( noisePersistence.set("Noise Persistence", ofToFloat(xml.getValue("Noise_Persistence")), 0.001f, 1.0f) );
//    
//    gui.add( baseSpeed.set("Wind", ofVec3f(ofToFloat(baseSpeedSplit[0]), ofToFloat(baseSpeedSplit[1]), ofToFloat(baseSpeedSplit[2])), ofVec3f(-2,-2,-2), ofVec3f(2,2,2)) );
//    
//    gui.add( startColor.set("Start Color", ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3])), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
//    
//    gui.add( endColor.set("End Color", ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3])), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
//    
//    gui.add( particleSize.set("Particle Size", ofToFloat(xml.getValue("Particle_Size")), 0.0001f, 0.05f) );
    
    particleMaxAge = ofToFloat(xml.getValue("Particle_Max_Age"));
    noiseMagnitude = ofToFloat(xml.getValue("Noise_Magnitude"));
    noisePositionScale = ofToFloat(xml.getValue("Noise_Position_Scale"));
    noisePersistence = ofToFloat(xml.getValue("Noise_Persistence"));
    noiseTimeScale = ofToFloat(xml.getValue("Noise_Time_Scale"));
    particleSize = ofToFloat(xml.getValue("Particle_Size"));
    timeStep = ofToFloat(xml.getValue("Time_Step"));
    
    vector<string> baseSpeedSplit = ofSplitString(xml.getValue("Wind"), ",");
    baseSpeed = ofVec3f(ofToFloat(baseSpeedSplit[0]), ofToFloat(baseSpeedSplit[1]), ofToFloat(baseSpeedSplit[2]));
    
    vector<string> colorSplit = ofSplitString(xml.getValue("Start_Color"), ",");
    startColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));
    
    colorSplit = ofSplitString(xml.getValue("End_Color"), ",");
    endColor = ofColor(ofToFloat(colorSplit[0]), ofToFloat(colorSplit[1]), ofToFloat(colorSplit[2]), ofToFloat(colorSplit[3]));
//
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
    xml.save(xmlSettingsPath);
}

void element::setFromCurrentSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam) {
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
}

void element::setToParticleSystem(ParticleSystemGPU* particleSystem, ofxFirstPersonCamera* cam) {
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
}