#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(30);
    
    cam.setup(640,480);
    
    detector.setup();
    
    string path = "markers";
    ofDirectory dir(path);
    dir.allowExt("jpg");
    dir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir.size(); i++){
        
        ofImage img;
        img.load(dir.getPath(i));
        detector.addImageToTrack(img, dir.getFile(i).getFileName());
        images.push_back(img);
    }
    
  
    detector.start();


}

//--------------------------------------------------------------
void ofApp::update(){
    
    cam.update();
    if(cam.isFrameNew()) {
        detector.update(cam.getPixels());
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofSetColor(255, 255, 255);
    cam.draw(0.0,0.0);

    for(int i=0; i<images.size(); i++) {

        if(detector.getDetected(i)) {
            ofDrawBitmapStringHighlight(detector.labels[i] + " is detected !", 20, 480 + i * 20);
        }

    }
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
