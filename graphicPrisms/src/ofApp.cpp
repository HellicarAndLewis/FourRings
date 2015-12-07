#include "ofApp.h"

bool shouldRemoveRigidBody( const shared_ptr<ofxBulletRigidBody>& ab ) {
    return ab->getPosition().x > 50;
}

bool shouldRemoveBunny( const shared_ptr<ofxBulletSoftTriMesh>& ab ) {
    return ab->getPosition().y > 15;
}

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate( 60 );
    
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
    //camera.disableMouseInput();
    
    world.setup();
    world.setCamera(&camera);
    
    ground = new ofxBulletBox();
	ground->create( world.world, ofVec3f(0., 5.5, 0.), 0., 50., 1.f, 50.f );
    ground->setProperties(.25, .95);

    ground->add();
    
    //setup the kinect
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    grayImage.allocate(kinect.getWidth(), kinect.getHeight());
    grayThreshFar.allocate(kinect.getWidth(), kinect.getHeight());
    grayThreshNear.allocate(kinect.getWidth(), kinect.getHeight());
    
    vignette.loadImage("Images/Vignette_white_001.png");
    
    // used from the OF examples/3d/pointPickerExample
    //mesh.load("lofi-bunny.ply");
    
//    mesh.setMode( OF_PRIMITIVE_TRIANGLES );
//    mesh.addVertex(ofVec3f(0, 0, 0));
//    mesh.addVertex(ofVec3f(0, 0, bodyDepth));
//    mesh.addVertex(ofVec3f(bodyHeight, 0, bodyDepth + offSet));
//    
//    mesh.addVertex(ofVec3f(0, 0, 0));
//    mesh.addVertex(ofVec3f(0, 0, offSet));
//    mesh.addVertex(ofVec3f(bodyHeight, 0, bodyDepth + offSet));
//    
//    mesh.addVertex(ofVec3f(0, bodyHeight, 0));
//    mesh.addVertex(ofVec3f(0, bodyHeight, bodyDepth));    
//    mesh.addVertex(ofVec3f(bodyHeight, bodyHeight, bodyDepth + offSet));
//    
//    mesh.addVertex(ofVec3f(0, bodyHeight, 0));
//    mesh.addVertex(ofVec3f(0, bodyHeight, offSet));
//    mesh.addVertex(ofVec3f(bodyHeight, bodyHeight, bodyDepth + offSet));

    camera.enableMouseInput();
    
    drawGui = false;
    
    string xmlSettingsPath = "Settings/Main.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(camPos.set("Camera Position", ofVec3f(0, 0, 0), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
    gui.add( bodyHeight.set("Body Height", 1.0, 0.0, 10.0));
    gui.add( bodyWidth.set("Body Width", 1.0, 0.0, 10.0));
    gui.add( bodyDepth.set("Body Depth", 1.0, 0.0, 10.0));
    gui.add( spawnRate.set("Spawn Rate", 0.1, 0.0, 1.0));
    gui.add( offSet.set("Offset", 0.0, 0.0, 10.0));
    gui.add( gravity.set("Gravity", ofVec3f(1.0, 1.0, 0.0), ofVec3f(0.0, 0.0, 0.0), ofVec3f(2.0, 2.0, 2.0)));
    gui.add( nearClip.set("Near Clip", 230, 0, 255) );
    gui.add( farClip.set("Far Clip", 200, 0, 255) );
    
    world.setGravity( gravity );

    gui.loadFromFile(xmlSettingsPath);
    
    camera.setDistance( 1 );
    
    camera.setPosition(camPos);
    camera.lookAt(ofVec3f(-5, 0, 0), ofVec3f(1., -1., 0.));
    
    light.setPosition( 0, -5, 0 );
    
    lastSpawn = ofGetElapsedTimeMillis();
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {
	world.update();
    
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
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        vector<ofxCvBlob> blobs = contourFinder.blobs;
    }
    
    world.setGravity(gravity);
    
    ofRemove( rigidBodies, shouldRemoveRigidBody );
    ofRemove( bunnies, shouldRemoveBunny );
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    int timeSinceLastSpawn = ofGetElapsedTimeMillis() - lastSpawn;
    float inverseSpawnRate = 1.0 / spawnRate;
    if((ofGetElapsedTimeMillis() - lastSpawn) > (1000 / spawnRate)) {
        shared_ptr< ofxBulletBox > ss( new ofxBulletBox() );
        ss->create( world.world, ofVec3f(-35.0 + ofRandom(5), 0.0 + ofRandom(5), 0.0 + ofRandom(-5, 5)), 0.1, bodyWidth, bodyHeight, bodyDepth );
        ss->add();
        
        rigidBodies.push_back( ss );
        lastSpawn = ofGetElapsedTimeMillis();
    }
    
    fbo.begin();
    ofClear(0, 0, 0);
    ofEnableDepthTest();
    ofBackground( ofColor( 255 ) );
    camera.begin();
    
//    world.drawDebug();
    
    ofEnableLighting();
    light.enable();
    ofSetColor( 255 );
    //ground->draw();
    
    ofSetColor( 0 );
    for( int i = 0; i < rigidBodies.size(); i++ ) {
        rigidBodies[i]->draw();
    }
    
    ofSetLineWidth( 1 );
    
    light.disable();
    ofDisableLighting();
    
//    light.draw();
    
    camera.end();
    
    ofDisableDepthTest();
    
    ofSetColor( 255, 255, 255 );
//    ofDrawBitmapString("Shoot ball(spacebar): "+ofToString( rigidBodies.size(), 0)+"\nAdd Bunny(b): "+ofToString(bunnies.size(), 0), 20, 20 );
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 0, ofGetHeight() - 10);
    
    camPos.set(camera.getPosition());
    
    if( drawGui ) {
        gui.draw();
        
        grayImage.draw(0, ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
        contourFinder.draw(0, ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
    }
    fbo.end();
    
    fbo.draw(0, 0);
    ofSetColor(255);
    //vignette.draw(0, 0, ofGetWidth(), ofGetHeight());
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if( key == ' ' ) {
        shared_ptr< ofxBulletBox > ss( new ofxBulletBox() );
        ss->create( world.world, ofVec3f(-25.0, -10.0, 5.0), 1.0, 1.0, 1.0, 1.0 );
        ss->add();
        
        ofVec3f frc = -camera.getPosition();
        frc.normalize();
        //ss->applyCentralForce( frc * 2000 );
        
        rigidBodies.push_back( ss );
    }
    
    if( key == 'b' ) {
        ofQuaternion tquat;
        tquat.makeRotate( 180, 1, 0, 0 );
        
        float tscale = ofRandom(0.3, 1);
        btTransform tt = ofGetBtTransform( ofVec3f( ofRandom(-5,5)*tscale*30, -15 * tscale * 30, 0), tquat );
        
        shared_ptr<ofxBulletSoftTriMesh> bunny(new ofxBulletSoftTriMesh());
        bunny->create( &world, mesh, tt, 2 * tscale );
        
        bunny->getSoftBody()->generateBendingConstraints( 3, bunny->getSoftBody()->m_materials[0] );
        
        bunny->getSoftBody()->randomizeConstraints();
        bunny->getSoftBody()->scale( btVector3( 0.025*tscale, 0.025*tscale, 0.025*tscale) );
        
        bunny->getSoftBody()->m_cfg.collisions =	btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;
        bunny->getSoftBody()->generateClusters(6);
        
        bunny->add();
        
        bunny->getSoftBody()->m_cfg.piterations =2;
        bunny->getSoftBody()->m_cfg.kDF			=1;
        bunny->getSoftBody()->m_cfg.kSSHR_CL	=1;
        bunny->getSoftBody()->m_cfg.kSS_SPLT_CL	=0;
        bunny->getSoftBody()->m_cfg.kSKHR_CL	=0.1f;
        bunny->getSoftBody()->m_cfg.kSK_SPLT_CL	= 1;
        
//        bunny->setStiffness(0.5, 0.5, 0.5);
        
        bunnies.push_back( bunny );
    }
    
    if( key == 127 || key == OF_KEY_DEL ) {
        if( bunnies.size() ) {
            bunnies.erase( bunnies.begin() );
        }
    }
    
    if( key == OF_KEY_TAB ) {
        drawGui = !drawGui;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    float mouseX = ofMap(x, 0, ofGetWidth(), 10, -10);
    float mouseY = ofMap(y, 0, ofGetHeight(), 10, -10);
    
    light.setPosition(mouseY, 0, mouseX);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
	
}