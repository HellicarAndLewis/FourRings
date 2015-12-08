#include "ofApp.h"

bool shouldRemoveRigidBody( const shared_ptr<ofxBulletRigidBody>& ab ) {
    return ab->getPosition().x > 50;
}

bool shouldRemoveBunny( const shared_ptr<ofxBulletCustomShape>& ab ) {
    return ab->getPosition().x > 200;
}

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate( 60 );
    
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
    //camera.disableMouseInput();
    
    ofDirectory imagesDir;
    imagesDir.listDir("Images/Audi Crops");
    for(int i = 0; i < imagesDir.numFiles(); i++) {
        backgroundImageNames.push_back(imagesDir.getPath(i));
    }
    
    backgroundImage.loadImage(backgroundImageNames[0]);
    
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
    
    shader.load("Shaders/NoGrayscaleVert.glsl", "Shaders/NoGrayscaleFrag.glsl");

    camera.enableMouseInput();
    
    drawGui = false;
    
    string xmlSettingsPath = "Settings/Main.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(camPos.set("Camera Position", ofVec3f(0, 0, 0), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
    gui.add( bodyHeight.set("Body Height", 1.0, 0.0, 10.0));
    gui.add( bodyWidth.set("Body Width", 1.0, 0.0, 10.0));
    gui.add( bodyDepth.set("Body Depth", 1.0, 0.0, 10.0));
    gui.add( spawnRate.set("Spawn Rate", 0.1, 0.0, 4.0));
    gui.add( offSet.set("Offset", 0.0, 0.0, 10.0));
    gui.add( gravity.set("Gravity", ofVec3f(1.0, 1.0, 0.0), ofVec3f(0.0, 0.0, 0.0), ofVec3f(2.0, 2.0, 2.0)));
    gui.add( nearClip.set("Near Clip", 230, 0, 255) );
    gui.add( farClip.set("Far Clip", 200, 0, 255) );
    gui.add( foregroundCol.set("ForegroundColor", ofColor(255, 255, 255), ofColor(0, 0, 0), ofColor(255, 255, 255)));
    gui.add( lightXRange.set("Light X Range", 1.0, 0.0, 100.0));
    gui.add( lightYRange.set("Light Y Range", 1.0, 0.0, 100.0));
    gui.add( spawnSpread.set("X Spawn Spread", 30, 0.0, 50.0));

    gui.loadFromFile(xmlSettingsPath);
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    //Each Vertex
    mesh.addVertex(ofVec3f(0, 0, 0)); // 0
    mesh.addVertex(ofVec3f(bodyWidth, 0, offSet)); // 1
    mesh.addVertex(ofVec3f(bodyWidth, 0, bodyDepth + offSet)); // 2
    mesh.addVertex(ofVec3f(0, 0, bodyDepth)); // 3
    mesh.addVertex(ofVec3f(0, bodyHeight, 0)); // 4
    mesh.addVertex(ofVec3f(bodyWidth, bodyHeight, offSet)); // 5
    mesh.addVertex(ofVec3f(bodyWidth, bodyHeight, bodyDepth + offSet)); // 6
    mesh.addVertex(ofVec3f(0, bodyHeight, bodyDepth)); // 7
    
    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);
    
    mesh.addIndex(0);
    mesh.addIndex(3);
    mesh.addIndex(2);
    
    mesh.addIndex(4);
    mesh.addIndex(5);
    mesh.addIndex(6);
    
    mesh.addIndex(4);
    mesh.addIndex(7);
    mesh.addIndex(6);
    
    mesh.addIndex(0);
    mesh.addIndex(4);
    mesh.addIndex(3);
    
    mesh.addIndex(4);
    mesh.addIndex(3);
    mesh.addIndex(7);
    
    mesh.addIndex(1);
    mesh.addIndex(5);
    mesh.addIndex(2);
    
    mesh.addIndex(5);
    mesh.addIndex(2);
    mesh.addIndex(6);
    
    mesh.addIndex(0);
    mesh.addIndex(4);
    mesh.addIndex(1);
    
    mesh.addIndex(5);
    mesh.addIndex(4);
    mesh.addIndex(1);
    
    mesh.addIndex(3);
    mesh.addIndex(7);
    mesh.addIndex(2);
    
    mesh.addIndex(6);
    mesh.addIndex(7);
    mesh.addIndex(2);
    
    world.setGravity( gravity );
    
    camera.setDistance( 1 );
    
    camera.setPosition(camPos);
    camera.lookAt(ofVec3f(0 , 0, 0), ofVec3f(0., 1., 0.));
    
    int numLights = 1;
    lights.resize(numLights);
    lightLocs.resize(numLights);
    for(int i = 0; i < lights.size(); i++) {
        lights[i] = new ofLight();
//        lights[i]->setup();
        // lights[i]->setSpotlight();
        lightLocs[i].set(ofVec2f(0, 0));
        lightLocs[i].attraction = 0.007;
        lights[i]->setPosition( 0, 0, 0 );
    }
    
    lastSpawn = ofGetElapsedTimeMillis();
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    
    xParalax.set(0);
    xParalax.attraction = 0.1;
}

//--------------------------------------------------------------
void ofApp::update() {
	world.update();
    
    for(int i = 0; i < lights.size(); i++) {
        lightLocs[i].update();
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
//        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
//        vector<ofxCvBlob> blobs = contourFinder.blobs;
//        if(blobs.size() > 0) {
//            ofVec2f overallCentroid;
//            for(int i = 0; i < blobs.size(); i++) {
//                ofVec2f centroid = blobs[i].centroid;
//                overallCentroid.x += centroid.x;
//                overallCentroid.y += centroid.y;
//    //            centroid.x = ofMap(centroid.x, 0, kinect.getWidth(), lightXRange, -lightXRange);
//    //            centroid.y = ofMap(centroid.y, 0, kinect.getHeight(), lightYRange, -lightYRange);
////                lightLocs[i].target(ofVec2f(centroid.x, centroid.y));
////                lights[i]->setPosition(0, 0, lightLocs[i].val.x);
//            }
//            overallCentroid.x /= blobs.size();
//            overallCentroid.y /= blobs.size();
//            overallCentroid.x = ofMap(overallCentroid.x,0, kinect.getWidth(), lightXRange, -lightXRange);
//            overallCentroid.y = ofMap(overallCentroid.y, 0, kinect.getHeight(), lightYRange, -lightYRange);
//            lightLocs[0].target(ofVec2f(overallCentroid.x, overallCentroid.y));
//            lights[0]->setPosition(0, 0, lightLocs[0].val.x);
//        } else {
//            for(int i = blobs.size(); i < lights.size(); i++) {
//                lightLocs[i].target(ofVec2f(0, 0));
//                lights[i]->setPosition(0, 0, lightLocs[0].val.x);
//            }
//        }
        flow.calcOpticalFlow(grayImage);
        ofVec2f averageFlow = flow.getAverageFlow();
        float x = averageFlow.x;
        xParalax.target(ofMap(x, -2, 2, 30, -30));
        xParalax.update();
        camera.setPosition(camera.getPosition().x, camera.getPosition().y, xParalax.val);
    }
    
    world.setGravity(gravity);
    
    ofRemove( rigidBodies, shouldRemoveRigidBody );
    ofRemove( parallelograms, shouldRemoveBunny );
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    int timeSinceLastSpawn = ofGetElapsedTimeMillis() - lastSpawn;
    float inverseSpawnRate = 1.0 / spawnRate;
    if((ofGetElapsedTimeMillis() - lastSpawn) > (1000 / spawnRate)) {
        
        shared_ptr< ofxBulletCustomShape > ss1( new ofxBulletCustomShape() );
//        shared_ptr< ofxBulletCustomShape > ss2( new ofxBulletCustomShape() );

        //        shared_ptr<ofxBulletTriMeshShape> ss( new ofxBulletTriMeshShape() );
//        ss->create(world.world, mesh, ofVec3f(-35.0 + ofRandom(5), 0.0 + ofRandom(5)), 1.0);
        //shared_ptr< ofxBulletBox > ss( new ofxBulletBox() );
        ss1->addMesh(mesh, ofVec3f(1, 1, 1), true);
        ss1->create( world.world, ofVec3f(-50.0 + ofRandom(-5, 5), 0.0 + ofRandom(-30, 30), 0.0 + ofRandom(-spawnSpread, spawnSpread)) );
        ss1->add();
        
        parallelograms.push_back( ss1 );
//        parallelograms.push_back( ss2 );

        lastSpawn = ofGetElapsedTimeMillis();
    }
    
    fbo.begin();
    ofClear(0, 0, 0);
    ofEnableDepthTest();
    ofBackground( 255, 255, 255 );
    camera.begin();
    
//    world.drawDebug();
    
    ofEnableLighting();
    for(int i = 0; i < lights.size(); i++) {
        lights[i]->enable();
    }
    ofSetColor( 255 );
    //ground->draw();
    
    ofSetColor( 255 );
    for( int i = 0; i < rigidBodies.size(); i++ ) {
        rigidBodies[i]->draw();
    }
    
    for(int i = 0; i < parallelograms.size(); i++) {
        parallelograms[i]->transformGL();
        mesh.drawFaces();
        parallelograms[i]->restoreTransformGL();
    }
    
    ofSetLineWidth( 1 );
    
    for(int i = 0; i < lights.size(); i++) {
        lights[i]->disable();
    }
    ofDisableLighting();
    
//    for(int i = 0; i < lights.size(); i++) {
//        lights[i]->draw();
//    }
    
    camera.end();
    
    ofDisableDepthTest();
    
    ofSetColor( 255, 255, 255 );
//    ofDrawBitmapString("Shoot ball(spacebar): "+ofToString( rigidBodies.size(), 0)+"\nAdd Bunny(b): "+ofToString(bunnies.size(), 0), 20, 20 );
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 0, ofGetHeight() - 10);
    
    camPos.set(camera.getPosition());
    
    fbo.end();
    
    shader.begin();
        shader.setUniform1f("thresh", thresh);
        shader.setUniformTexture("texture0", fbo, 0);
        shader.setUniformTexture("background", backgroundImage, 1);
        shader.setUniform3f("foregroundColor", foregroundCol.get().r, foregroundCol.get().g, foregroundCol.get().b);
        fbo.draw(0, 0);
    shader.end();
    ofSetColor(255);
    
    if( drawGui ) {
        gui.draw();
        
        grayImage.draw(0, ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
        contourFinder.draw(0, ofGetHeight() - kinect.getHeight(), kinect.getWidth(), kinect.getHeight());
    }
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
    if( key == OF_KEY_LEFT) {
        backgroundIndex--;
        if(backgroundIndex < 0) backgroundIndex = 0;
        backgroundImage.loadImage(backgroundImageNames[backgroundIndex]);
    }
    if( key == OF_KEY_RIGHT) {
        backgroundIndex++;
        if(backgroundIndex >= backgroundImageNames.size()) backgroundIndex = backgroundImageNames.size() - 1;
        backgroundImage.loadImage(backgroundImageNames[backgroundIndex]);
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
    
//    lights[0]->setPosition(mouseY, 0, mouseX);
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