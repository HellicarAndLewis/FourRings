#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    ofBackground(0,0,0);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    
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
    
    fadePass.allocate(ofGetWidth(), ofGetHeight());
    
    int rows = 20;
    int cols = 20;
    
    string xmlSettingsPath = "Settings/Main.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add( nearClip.set("Near Clip", 230, 0, 255) );
    gui.add( farClip.set("Far Clip", 200, 0, 255) );
    gui.add( imageDuration.set("Image Duration", 20, 1, 60));
    
    //position all particles
    for (int i=0; i<rows*cols; i++) {
        int x = int(i) % cols * ofGetWidth()/(cols - 1);
        int y = int(i) / cols * ofGetHeight()/(rows - 1);
        Particle p(ofVec3f(x,y,0), y==0 ? 0 : 1, .96);
        ps.push_back(p);
        mesh.addVertex(ofVec3f(x,y,0));
    }
    
    //create all springs
    for (int i=0; i<rows*cols; i++) {
        int x = int(i) % cols;
        int y = int(i) / cols;
        
        //horizontal structural springs
        if (x<cols-1) {
            sp.push_back(Spring(&ps[i],&ps[i+1]));
        }
        
        //vertical structural springs
        if (y<rows-1) {
            sp.push_back(Spring(&ps[i],&ps[i+cols]));
        }
        
        //shear springs left to right
        if (x<cols-1 && y<rows-1) {
            sp.push_back(Spring(&ps[i],&ps[i+cols+1]));
        }
        
        //shear springs right to left
        if (y>0 && x<cols-1 && y<rows) {
            sp.push_back(Spring(&ps[i],&ps[i-cols+1]));
        }
        
        //bending springs horizontal
        if (x<cols-2) {
            sp.push_back(Spring(&ps[i],&ps[i+2]));
        }
        
        //bending springs vertical
        if (y<rows-2) {
            sp.push_back(Spring(&ps[i],&ps[i+2*cols]));
        }
        
        //bending springs diagonal
        if (y<rows-2 && x<cols-2) {
            sp.push_back(Spring(&ps[i],&ps[i+2+2*cols]));
        }
    }
    
    for (int i=0; i<rows*cols; i++) {
        int x = int(i) % cols;
        int y = int(i) / cols;
        if(x != cols-1 && y != rows-1) {
            mesh.addIndex(i);
            mesh.addIndex(i+1);
            mesh.addIndex(i+cols);
        }
        if(x!= 0 && y != 0) {
            mesh.addIndex(i);
            mesh.addIndex(i-1);
            mesh.addIndex(i-cols);
        }
    }
    
    cam.setPosition(10*rows, 10*cols, 100);
    cam.lookAt(ofVec3f(10*rows, 10*cols, 0));
    
    light.setPosition(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 100));
    
    noisePoint = 1000;
    noiseStep = 0.1;
    
    drawGui = true;
    
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    fade.load("Shaders/DummyVert.glsl", "Shaders/FadeFrag.glsl");
    
    xWind.set(0);
    yWind.set(1.1);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //apply forces
    for (int i=0; i<ps.size(); i++) {
        //ps[i].addForce(ofVec3f(sin(ofGetElapsedTimef() * 1.6),1.1,0));
        ps[i].addForce(ofVec3f(0,1.1,0.5 - ofNoise(noisePoint)));
    }
    noisePoint += noiseStep;
    
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
        grayImage.resize(grayImage.getWidth()/2, grayImage.getHeight()/2);
        flow.calcOpticalFlow(grayImage);
        ofVec2f averageFlow = flow.getAverageFlow();
        float x = averageFlow.x;
        if(abs(x) < 0.01) x = 0;
        xWind.target(ofMap(x, -2, 2, -30, 30));
        xWind.update();
        float y = averageFlow.y;
        if(abs(y) < 0.01) y = 0;
        yWind.target(ofMap(y, -2, 2, -30, 30));
        yWind.update();
        for (int i=0; i<ps.size(); i++) {
            //ps[i].addForce(ofVec3f(sin(ofGetElapsedTimef() * 1.6),1.1,0));
            ps[i].addForce(ofVec3f(xWind.val,yWind.val,0));
        }
        //camera.setPosition(camera.getPosition().x, camera.getPosition().y, xParalax.val);
    }
    
    //update springs
    for (int i=0; i<sp.size(); i++) {
        sp[i].update();
    }
    
    //update particles
    for (int i=0; i<ps.size(); i++) {
        ps[i].update();
        mesh.setVertex(i, ps[i].pos);
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    fadePass.begin();
    fade.begin();
    fade.setUniformTexture("texOut", backgroundImgs[0].getTextureReference(), 0);
    fade.setUniformTexture("texIn", backgroundImgs[1].getTextureReference(), 1);
    fade.setUniform1f("fadeAmnt", fadeAmnt);
    backgroundImgs[0].draw(0, 0, ofGetWidth(), ofGetHeight());
    fade.end();
    fadePass.end();
//    ofTranslate(ofGetWidth()/2-200, 100, -300);
    
    ofSetColor(255);
    fadePass.draw(0, 0, ofGetWidth(), ofGetHeight());
    //springs
    ofSetColor(204, 0, 0);
    //cam.begin();
    ofEnableLighting();
    light.enable();
        mesh.drawFaces();
    light.disable();
    ofDisableLighting();
    
    ofSetColor(255);
    
    if(drawGui) {
        gui.draw();
        
        grayImage.draw(0, ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
    }
    //light.draw();

    //cam.end();
    
    
//    glBegin(GL_LINES);
//    for (int i=0; i<sp.size(); i++) {
//        glVertex3f(sp[i].a->pos.x, sp[i].a->pos.y, sp[i].a->pos.z);
//        glVertex3f(sp[i].b->pos.x, sp[i].b->pos.y, sp[i].b->pos.z);
//    }
//    glEnd();
    
    //particles
//    ofSetColor(0, 0, 255);
//    glPointSize(5);
//    glBegin(GL_POINTS);
//    for (int i=0; i<ps.size(); i++) {
//        glVertex3f(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
//    }
//    glEnd();
}

void ofApp::keyPressed(int key) {
    if( key == OF_KEY_TAB ) {
        drawGui = !drawGui;
    }
}