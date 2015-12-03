#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
	agua.setup(1080, 1920);
    
    img.loadImage("images/fondo.jpg");
    img.resize(ofGetWidth(), ofGetHeight());
    
    //Setup the kinect
    kinect.setRegistration(true);
    
    kinect.init();
    
    kinect.open();
    
    kinect.setDepthClipping();
    
    img.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayImage.mirror(false, true);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(230, true);
        grayThreshFar.threshold(70);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        vector<ofxCvBlob> blobs = contourFinder.blobs;
        for(int i = 0; i < blobs.size(); i++) {
            float scaledX = ofMap(blobs[i].centroid.x, 0, kinect.getWidth(), 0, ofGetWidth());
            float scaledY = ofMap(blobs[i].centroid.y, 0, kinect.getHeight(), 0, ofGetHeight());
            float scaledArea = ofMap(blobs[i].area, 0, (kinect.getWidth() * kinect.getHeight())/2, 0, 100);
            agua.disturb(scaledX, scaledY, scaledArea, 500.0f);
        }
//        vector<ofVec2f> points;
//        for(int i = 0; i < contourFinder.nBlobs; i++) {
//            for(int j = 0; j < blobs[i].nPts; j++) {
//                points.push_back(blobs[i].pts[j]);
//            }
//        }
//        if(points.size() > 0) {
//            int step = points.size() / 20;
//            if (step < 1) step = 1;
//            for(int i = 0; i < points.size(); i += step) {
//                float scaledX = ofMap(points[i].x, 0, kinect.getWidth(), 0, ofGetWidth(), true);
//                float scaledY = ofMap(points[i].y, 0, kinect.getHeight(), 0, ofGetHeight(), true);
//                agua.disturb(scaledX, scaledY, 12, 128);
//            }
//        }

    }

	agua.update(&img.getTextureReference());
	ofSetWindowTitle( ofToString(ofGetFrameRate()) + " FPS" );
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	agua.draw(true);
    
//    ofPushMatrix();
//    ofScale(-1, 1);
//    kinect.drawDepth(-kinect.getWidth()/2, 0, kinect.getWidth()/2, kinect.getHeight()/2);
//    ofPopMatrix();
//    contourFinder.draw(0, 0, kinect.getWidth()/2, kinect.getHeight()/2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
