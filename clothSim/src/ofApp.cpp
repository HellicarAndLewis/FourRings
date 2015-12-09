#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    
    int rows = 20;
    int cols = 20;
    
    //position all particles
    for (int i=0; i<rows*cols; i++) {
        int x = int(i) % cols * 20;
        int y = int(i) / cols * 20;
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
    
    
    cam.setPosition(mesh.getCentroid().x, mesh.getCentroid().y, 100);
    cam.lookAt(mesh.getCentroid());
    
    light.setPosition(cam.getPosition());
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //apply forces
    for (int i=0; i<ps.size(); i++) {
        ps[i].addForce(ofVec3f(0,1.1,0));
        ps[i].addForce(ofVec3f(0,0,sin(ofGetElapsedTimef() * 1.6)));
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
    
    ofTranslate(ofGetWidth()/2-200, 100, -300);
    
    //springs
    ofSetColor(255, 255, 255);
    cam.begin();
    ofEnableLighting();
    light.enable();
    mesh.drawFaces();
    light.disable();
    ofDisableLighting();
    light.draw();

    cam.end();
    
    
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