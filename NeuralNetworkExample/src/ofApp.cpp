#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    depthAICoreTest = new SemanticSegmentationDepth(); 
    depthAICoreTest->setup();


    cam.setDistance(200);
}

//--------------------------------------------------------------
void ofApp::update(){
    depthAICoreTest->update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0, 0, 4);
    ofEnableDepthTest();
    cam.begin();
    ofRotateYRad(ofGetElapsedTimef());
    ofScale(1.0);
    depthAICoreTest->draw();
    cam.end();
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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