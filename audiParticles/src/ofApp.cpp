#include "ofApp.h"

#define WIDTH ofGetWidth()
#define HEIGHT ofGetHeight()

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
    
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetDefaultWidth( 300 );

	int texSize = 128;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
	// Give us a starting point for the camera
	camera.setNearClip( 0.01f );
	camera.setPosition( 0, 0.4, 0.5 );
	camera.setMovementMaxSpeed( 0.01f );

	time = 0.0f;
//	timeStep = 1.0f / 60.0f / 16;
	
	drawGui = false;
    
    //Setup the videoGrabber
    grabber = new ofVideoGrabber();
    
    grabber->initGrabber( 160, 120 );
    
    //Setup the kinect
    kinect = new ofxKinect();

    kinect->init();
    
    kinect->open();
    
    // print the intrinsic IR sensor values
    if(kinect->isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect->getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect->getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect->getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect->getZeroPlaneDistance() << "mm";
    }
    
    //Setup the gui for controlling input and output
    string xmlSettingsPath = "Settings/InputOutput.xml";
    gui.setup("input/output", xmlSettingsPath);
    gui.setPosition(ofGetWidth()/2, 10);
    gui.add(input.set("Input", KINECT, 0, 1));
    gui.add(output.set("Element", FIRE, 0, 3));
    gui.loadFromFile(xmlSettingsPath);
    
    xWind.set(0.017);
    
    elements.resize(4);
    elements[0].loadFromFile("Settings/Fire.xml");
    elements[1].loadFromFile("Settings/Water.xml");
    elements[2].loadFromFile("Settings/Earth.xml");
    elements[3].loadFromFile("Settings/Air.xml");
    
    lastOutput = NUM_OUTPUTS;
    
    ofSetWindowShape(1080, 1920);
    ofSetWindowPosition(ofGetScreenWidth(), 0);
    
    foreground.loadImage("Textures/Lavacracks.png");
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
    if(output != lastOutput) {
        elements[output].setToParticleSystem(&particles, &camera);
        lastOutput = output;
    }
	// Update time, this let's us hit space and slow down time, even reverse it.
//	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );}
//	else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += particles.timeStep;
    
    if(input == KINECT) {
        kinect->update();
    }
    if(input == CAMERA) {
        grabber->update();
        if(grabber->isFrameNew()) {
            img.setFromPixels(grabber->getPixels(), grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
            img.setImageType(OF_IMAGE_GRAYSCALE);
            flowFinder.calcOpticalFlow(img);
            ofVec2f flow = flowFinder.getAverageFlow();
            float xFlow = ofMap(flow.x, -10, 10, 0.5, -0.5, true);
            xWind.target(xFlow);
            xWind.update();
//            particles.baseSpeed.set(ofVec3f(xWind.val, particles.baseSpeed.get().y,  particles.baseSpeed.get().z));
        }
    }
}




//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
    if(elements[output].backgroundLoaded) {
        ofPushStyle();
        ofSetColor(elements[output].backgroundColor);
        elements[output].background.draw(0, 0, WIDTH, HEIGHT);
        ofPopStyle();
    } else {
        ofBackground(elements[output].backgroundColor);
    }
    particles.update( time  );

	
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(60) );
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			//ofDrawGridPlane( 0.5, 12, false );
		ofPopMatrix();
		 
		ofSetColor( ofColor::white );
	
		particles.draw( &camera );
    
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
    
//    drawPointCloud();
    
   // kinect->draw(420, 10, 400, 300);
	
	int size = 196;
//	particles.particleDataFbo.source()->getTextureReference(0).draw( 0,	 0, size, size );
    ofPushStyle();
    ofSetColor(255, 255, 255);
    elements[output].foreground.draw(0, 0, WIDTH, HEIGHT);
    ofPopStyle();

	if( drawGui )
	{
		particles.gui.draw();
        gui.draw();
        //switch for which input we're using
        switch (input) {
            case KINECT:
                ofScale(-1, 1);
                kinect->draw(WIDTH/2, HEIGHT - kinect->getHeight() - 10, kinect->getHeight(), kinect->getWidth());
                break;
            case CAMERA:
                ofScale(-1, 1);
                grabber->draw(-WIDTH/2, HEIGHT - grabber->getHeight() - 10, grabber->getWidth(), grabber->getHeight());
            default:
                break;
        }
	}
	
	ofDisableDepthTest();
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
    
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == OF_KEY_TAB )
	{
		drawGui = !drawGui;
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_LEFT  )
	{
	}
	else if( key == OF_KEY_RIGHT  )
	{
	}
    else if( key == ' ' )
    {
        elements[output].setFromCurrentSystem(&particles, &camera);
        switch (output) {
            case 0:
                elements[output].saveToFile("Settings/Fire.xml");
                break;
            case 1:
                elements[output].saveToFile("Settings/Water.xml");
                break;
            case 2:
                elements[output].saveToFile("Settings/Earth.xml");
                break;
            case 3:
                elements[output].saveToFile("Settings/Air.xml");
                break;
            default:
                break;
        }
    }
    
    switch (key) {
        case '1':
            output = 0;
            break;
        case '2':
            output = 1;
            break;
        case '3':
            output = 2;
            break;
        case '4':
            output = 3;
            break;
        default:
            break;
    }
}
//-----------------------------------------------------------------------------------------
//
void ofApp::drawPointCloud() {
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    int step = 2;
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            if(kinect->getDistanceAt(x, y) > 0) {
                mesh.addColor(kinect->getColorAt(x,y));
                mesh.addVertex(kinect->getWorldCoordinateAt(x, y));
            }
        }
    }
    glPointSize(3);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards'
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();
    mesh.drawVertices();
    ofDisableDepthTest();
    ofPopMatrix();
}
