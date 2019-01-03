#pragma once

#include "ofMain.h"
#include "ofxGui.h"
class ofApp : public ofBaseApp{
    
public:
    void setup();
    void draw();
    void exit();
    void dragEvent(ofDragInfo dragInfo);
    
    
    vector<string> report;
    ofxPanel config;
    ofParameter<int> maxFileSize;
    ofParameter<int> maxWidth;
};
