//
//  DepthFinder.h
//  audiParticles
//
//  Created by James Bentley on 12/10/15.
//
//

#ifndef audiParticles_DepthFinder_h
#define audiParticles_DepthFinder_h

#include "ofMain.h"
#include "ofxOpenCv.h"

struct DepthFinder {
    float currentAverageDepth;
    float runningAverageDepth;
    int farClip;
    int nearClip;
    ofxCvGrayscaleImage image;
    list<float> vals;
    
    void setup(int width, int height) {
        currentAverageDepth = 0;
        runningAverageDepth = 0;
        image.allocate(width, height);
    }
    
    float getCurrentAverageDepth() {
        return ofMap(currentAverageDepth, 0, 255, 1, 0);
    }
    
    float getRunningAverageDepth() {
        return ofMap(runningAverageDepth, 0, 255, 1, 0);
    }
    
    void updateAverageDepth(ofxKinect* kinect) {
        image.setFromPixels(kinect->getDepthPixelsRef());
        ofPixels pixels = image.getPixelsRef();
        float average = 0;
        for(int i = 0; i < pixels.size(); i++) {
            average += pixels[i];
        }
        average /= pixels.size();
        currentAverageDepth = average;
        vals.push_back(average);
        if(vals.size() > 200) {
            vals.pop_front();
        }
        float timeAverage = 0;
        for(auto it = vals.begin(); it != vals.end(); it++) {
            timeAverage += *it;
        }
        timeAverage /= vals.size();
        
        runningAverageDepth = timeAverage;
    }
    
    void debugDraw(int x, int y, int width, int height) {
        image.draw(x, y, width, height);
        ofDrawBitmapString(ofToString(currentAverageDepth), x, y - 10);
    }
    
    void debugDraw(int x, int y) {
        image.draw(x, y);
        ofDrawBitmapString(ofToString(currentAverageDepth), x, y - 10);
    }
    
};

#endif
