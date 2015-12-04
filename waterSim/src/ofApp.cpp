#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
	agua.setup(1080, 1920);
    
    img.loadImage("images/fondo.jpg");
    img.resize(ofGetWidth(), ofGetHeight());
    
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

    gui.loadFromFile(xmlSettingsPath);
    
    //Setup the kinect
    kinect.setRegistration(true);
    
    drawGui = false;
    drawCam = false;
    
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

	agua.update(&img.getTextureReference());
	ofSetWindowTitle( ofToString(ofGetFrameRate()) + " FPS" );
}

//--------------------------------------------------------------
void ofApp::disturbOnCentroid(vector<ofxCvBlob> blobs, ofxWaterRipple* agua) {
        for(int i = 0; i < blobs.size(); i++) {
            float scaledX = ofMap(blobs[i].centroid.x, 0, kinect.getWidth(), 0, ofGetWidth());
            float scaledY = ofMap(blobs[i].centroid.y, 0, kinect.getHeight(), 0, ofGetHeight());
            float scaledArea = ofMap(blobs[i].area, 0, (kinect.getWidth() * kinect.getHeight())/2, 0, 100);
            agua->disturb(scaledX, scaledY, scaledArea, 500.0f);
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
	ofBackground(0);
	ofSetColor(255);
	agua.draw(true);
    
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
