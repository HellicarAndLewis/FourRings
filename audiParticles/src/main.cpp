#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{
	//ofSetCurrentRenderer( ofGLProgrammableRenderer::TYPE );
	//ofSetOpenGLVersion( 3, 2 );
	ofSetupOpenGL(1080,1920, OF_FULLSCREEN);
	ofRunApp( new ofApp());

}
