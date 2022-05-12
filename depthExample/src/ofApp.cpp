#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    depthAligned = new DepthAligned(); 
    depthAligned->setup();


    cam.setDistance(200);
}

//--------------------------------------------------------------
void ofApp::update(){
    depthAligned->update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0, 0, 10);
    ofEnableDepthTest();
    cam.begin();
    ofScale(.5);
    depthAligned->draw();
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