#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    //Setup the gui panel
    string xmlSettingsPath = "Settings/Main.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add( nearClip.set("Near Clip", 230, 0, 255) );
    gui.add( farClip.set("Far Clip", 200, 0, 255) );
    
    drawGui = false;
    drawCam = false;
    
    ofDirectory imagesDir;
    imagesDir.listDir("Images");
    for(int i = 0; i < imagesDir.numFiles(); i++) {
        backgroundImgNames.push_back(imagesDir.getPath(i));
    }
    
    if(backgroundImgNames.size() >= 2) {
        backgroundImgs[backgroundIndex].loadImage(backgroundImgNames[backgroundIndex]);
        backgroundIndex++;
        backgroundIndex%=backgroundImgNames.size();
        backgroundImgs[backgroundIndex].loadImage(backgroundImgNames[backgroundIndex]);
        backgroundIndex++;
        backgroundIndex%=backgroundImgNames.size();
    }
    
    //Setup our Bokeh Shader
    bokeh.ofShader::load("Shaders/bokehVert.glsl", "Shaders/bokehFrag.glsl");
    
    //setup the kinect
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    //load the image and allocate CV images
    img.loadImage("nyDusk.jpg");
    img.resize(ofGetWidth(), ofGetHeight());
    grayImage.allocate(kinect.getWidth(), kinect.getHeight());
    grayThreshNear.allocate(kinect.getWidth(), kinect.getHeight());
    grayThreshFar.allocate(kinect.getWidth(), kinect.getHeight());
    
//    for (int i = 0;)
    //draw initial FBO
    background.allocate(ofGetWidth(), ofGetHeight());
        background.begin();
            bokeh.begin();
            bokeh.setUniformTexture("tex", img, 0);
            bokeh.setUniform1f("max_radius", 30.0);
            img.draw(0, 0, ofGetWidth(), ofGetHeight());
        bokeh.end();
    background.end();
    
    fadePass.allocate(ofGetWidth(), ofGetHeight());
    
    fade.ofShader::load("Shaders/DummyVert.glsl", "Shaders/FadeFrag.glsl");
    
    glowImg.loadImage("bokeh-circle-3-02.png");
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayImage.mirror(false, true);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearClip, true);
        grayThreshFar.threshold(farClip);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        grayImage.resize(ofGetWidth(), ofGetHeight());
        //contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        vector<ofxCvBlob> blobs = contourFinder.blobs;
    }
    vector<int> glowsToDelete;
    for(int i = 0; i < glows.size(); i++) {
        glows[i].update();
        if(!glows[i].alive) {
            glowsToDelete.push_back(i);
        }
    }
    for(int i = glowsToDelete.size() - 1; i > 0; i--) {
        glows.erase(glows.begin() + glowsToDelete[i]);
    }
//    for(vector<int>::iterator glowToDelete = glowsToDelete.end(); glowToDelete != glowsToDelete.begin(); glowToDelete--) {
//        glows.erase(glows.begin() + *glowToDelete);
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw(0, 0);
    kinect.update();
    //if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayImage.mirror(false, true);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearClip, true);
        grayThreshFar.threshold(farClip);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        //grayImage.resize(ofGetWidth(), ofGetHeight());
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        vector<ofxCvBlob> blobs = contourFinder.blobs;
        drawOnContours(blobs);
        for(int i = 0; i < glows.size(); i++) {
            glows[i].draw(&glowImg);
        }
    //}
//    if(kinect.isFrameNew()) {
//        bokeh.begin();
//            bokeh.setUniformTexture("tex", img, 0);
//            //bokeh.setUniformTexture("blurAreas", grayImage, 1);
//            bokeh.setUniform1f("max_radius", 30.0);
//    grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
//            img.draw(0, 0, ofGetWidth(), ofGetHeight());
//        bokeh.end();
//    }
    if( drawGui ) {
        gui.draw();
        ofPushMatrix();
        ofScale(-1, 1);
        grayImage.draw(-kinect.getWidth()/2 - kinect.getWidth(), ofGetHeight() - kinect.getHeight() / 2, kinect.getWidth()/2, kinect.getHeight()/2);
//        contourFinder.draw(-kinect.getWidth()/2 - kinect.getWidth(), ofGetHeight() - kinect.getHeight() / 2, kinect.getWidth()/2, kinect.getHeight()/2);
        ofPopMatrix();
    }
    if( drawCam ) {
        ofPushMatrix();
        ofScale(-1, 1);
        kinect.draw(-kinect.getWidth(), ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
        ofPopMatrix();
    }
//    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10, 0);
}

//--------------------------------------------------------------
void ofApp::drawOnContours(vector<ofxCvBlob> blobs) {
    vector<ofVec2f> points;
    for(int i = 0; i < contourFinder.nBlobs; i++) {
        for(int j = 0; j < blobs[i].nPts; j++) {
            points.push_back(blobs[i].pts[j]);
        }
    }
    if(points.size() > 0) {
        int step = points.size() / 5;
        if (step < 1) step = 1;
        for(int i = 0; i < points.size(); i += step) {
            float scaledX = ofMap(points[i].x, 0, kinect.getWidth(), 0, ofGetWidth(), true);
            float scaledY = ofMap(points[i].y, 0, kinect.getHeight(), 0, ofGetHeight(), true);
            ofPixels pixels = img.getPixelsRef();
            ofColor col = pixels.getColor(scaledX, scaledY);
            glow newGlow;
            newGlow.emerge(ofVec2f(scaledX, scaledY), col);
            if(glows.size() < 1)
                glows.push_back(newGlow);
            else {
                bool farEnoughAway = true;
                for(int i = 0; i < glows.size(); i++) {
                    if((newGlow.loc - glows[i].loc).length() < newGlow.rad*2) {
                        farEnoughAway = false;
                        break;
                    }
                }
                if(farEnoughAway) {
                    glows.push_back(newGlow);
                }
            }
//            else {
//                glows.push_back(newGlow);
//            }
            //if(glows.size() < 50)
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_TAB) {
        drawGui = !drawGui;
    }
    else if(key == 'c') {
        drawCam = !drawCam;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
