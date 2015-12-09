#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
	agua.setup(1080, 1920);

    flatBackground.allocate(ofGetWidth(), ofGetHeight());
    flatBackground.begin();
    ofClear(0, 0, 0);
    ofSetColor(127, 127, 127);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    flatBackground.end();
    
    ofDirectory imagesDir;
    imagesDir.listDir("Images/Audi Crops");
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
    
    fadeAmnt = 0.0;
    
    fade.load("Shaders/DummyVert.glsl", "Shaders/FadeFrag.glsl");
    
    fadePass.allocate(ofGetWidth(), ofGetHeight());
    
    string xmlSettingsPath = "Settings/Main.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add( nearClip.set("Near Clip", 230, 0, 255) );
    gui.add( farClip.set("Far Clip", 200, 0, 255) );
    //profundidad means depth, but I believe in a thing called love so I'm gonna keep it in. Viva Espania!
    gui.add( profundidad.set("Depth", 100, 0, 500) );
    //radio means radius, but just listen to the rhythm of my heart, gotta save that one!
    gui.add( radio.set("radius", 12, 0, 100));
    gui.add( useCentroid.set("Use Centroid", true));
    gui.add( useContours.set("Use Contours", true));
    gui.add( col1.set("Foreground Color", ofColor(0), ofColor(0), ofColor(255)));
    gui.add( imageDuration.set("Image Duration", 20, 1, 60));
    
    gui.loadFromFile(xmlSettingsPath);
    
    overLay.load("Shaders/overLay");
    
    //Setup the kinect
    kinect.setRegistration(true);
    
    drawGui = false;
    drawCam = false;
    
    kinect.init();
    
    kinect.open();
    
    kinect.setDepthClipping();
    currentTime = ofGetElapsedTimef();
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
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        vector<ofxCvBlob> blobs = contourFinder.blobs;
        if( useCentroid )
            disturbOnCentroid(blobs, &agua);
        if( useContours )
            disturbOnContours(blobs, &agua);
    }
    
    if(fadeAmnt > 0.0 || ofGetElapsedTimef() - currentTime > imageDuration) {
        fadeAmnt += 0.01;
        if(fadeAmnt > 1.0) {
            fadeAmnt = 0.0;
            backgroundImgs[0] = backgroundImgs[1];
            backgroundImgs[1].loadImage(backgroundImgNames[backgroundIndex]);
            backgroundIndex++;
            backgroundIndex %= backgroundImgNames.size();
        }
        currentTime = ofGetElapsedTimef();
    }


	agua.update(&flatBackground.getTextureReference());
	ofSetWindowTitle( ofToString(ofGetFrameRate()) + " FPS" );
}

//--------------------------------------------------------------
void ofApp::disturbOnCentroid(vector<ofxCvBlob> blobs, ofxWaterRipple* agua) {
    for(int i = 0; i < blobs.size(); i++) {
        float scaledX = ofMap(blobs[i].centroid.x, 0, kinect.getWidth(), 0, ofGetWidth());
        float scaledY = ofMap(blobs[i].centroid.y, 0, kinect.getHeight(), 0, ofGetHeight());
        float scaledArea = ofMap(blobs[i].area, 0, (kinect.getWidth() * kinect.getHeight())/2, 0, 100);
        agua->disturb(scaledX, scaledY, radio, 500.0f);
    }
}

//--------------------------------------------------------------
void ofApp::disturbOnContours(vector<ofxCvBlob> blobs, ofxWaterRipple* agua) {
    vector<ofVec2f> points;
    for(int i = 0; i < contourFinder.nBlobs; i++) {
        for(int j = 0; j < blobs[i].nPts; j++) {
            points.push_back(blobs[i].pts[j]);
        }
    }
    if(points.size() > 0) {
        int step = points.size() / 20;
        if (step < 1) step = 1;
        for(int i = 0; i < points.size(); i += step) {
            float scaledX = ofMap(points[i].x, 0, kinect.getWidth(), 0, ofGetWidth(), true);
            float scaledY = ofMap(points[i].y, 0, kinect.getHeight(), 0, ofGetHeight(), true);
            agua->disturb(scaledX, scaledY, radio, profundidad);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    fadePass.begin();
        fade.begin();
        fade.setUniformTexture("texOut", backgroundImgs[0].getTextureReference(), 0);
        fade.setUniformTexture("texIn", backgroundImgs[1].getTextureReference(), 1);
        fade.setUniform1f("fadeAmnt", fadeAmnt);
        backgroundImgs[0].draw(0, 0, ofGetWidth(), ofGetHeight());
        fade.end();
    fadePass.end();
    
	ofBackground(0);
	ofSetColor(255);
	ofFbo* fbo = agua.getFbo();
    ofVec3f color1, color2;
    color1.x = ofMap(col1.get().r, 0.0, 255.0, 0.0, 1.0);
    color1.y = ofMap(col1.get().g, 0.0, 255.0, 0.0, 1.0);
    color1.z = ofMap(col1.get().b, 0.0, 255.0, 0.0, 1.0);

    overLay.begin();
        overLay.setUniform3f("color1", color1.x, color1.y, color1.z);
        overLay.setUniformTexture("texture0", *fbo, 0);
        overLay.setUniformTexture("revealImg", fadePass, 1);
        fbo->draw(0, 0);
    overLay.end();
    //fbo->draw(0, 0);
    
    if( drawGui ) {
        gui.draw();
        ofPushMatrix();
        ofScale(-1, 1);
        grayImage.draw(-kinect.getWidth()/2 - kinect.getWidth(), ofGetHeight() - kinect.getHeight() / 2, kinect.getWidth()/2, kinect.getHeight()/2);
        contourFinder.draw(-kinect.getWidth()/2 - kinect.getWidth(), ofGetHeight() - kinect.getHeight() / 2, kinect.getWidth()/2, kinect.getHeight()/2);
        ofPopMatrix();
    }
    if( drawCam ) {
        ofPushMatrix();
        ofScale(-1, 1);
        kinect.draw(-kinect.getWidth(), ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_TAB) {
        drawGui = !drawGui;
    }
    if(key == 'c') {
        drawCam = !drawCam;
    }
    if( key == OF_KEY_LEFT) {
        backgroundIndex--;
        if(backgroundIndex < 0) backgroundIndex = 0;
        backgroundImgs[1].loadImage(backgroundImgNames[backgroundIndex]);
    }
    if( key == OF_KEY_RIGHT) {
        backgroundIndex++;
        if(backgroundIndex >= backgroundImgNames.size()) backgroundIndex = backgroundImgNames.size() - 1;
        backgroundImgs[1].loadImage(backgroundImgNames[backgroundIndex]);
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
	agua.disturb(x,y,10,128);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (button == OF_MOUSE_BUTTON_1)
		agua.disturb(x,y,10,128);
	else
		agua.disturb(x,y,50,500);

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
