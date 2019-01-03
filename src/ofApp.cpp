#include "ofApp.h"
#define CONFIG_FILE "/.cod.xml"
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(12);
    config.setup("Settings",ofFilePath::getUserHomeDir()+CONFIG_FILE);
    maxFileSize.set("Max image file size (kb)", 512, 10, 2048);
    maxWidth.set("Max image dimension (px)", 1920,1024,1024*3);
    config.add(maxFileSize);
    config.add(maxWidth);
    config.loadFromFile(ofFilePath::getUserHomeDir()+CONFIG_FILE);
    config.setPosition(400,40);
}

//--------------------------------------------------------------
void ofApp::exit(){
    config.saveToFile(ofFilePath::getUserHomeDir()+CONFIG_FILE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofDrawBitmapString(ofToUpper("Drag and drop your keynote files here"), 50, 50);
    for(size_t i=0;i<report.size()/2;i++){
        ofDrawBitmapString(report[i*2],50,85+i*40);
        ofDrawBitmapString(report[i*2+1],50,85+i*40+16);
    }
    config.draw();
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    auto dir=ofDirectory();
    
    report.clear();
    
    for(auto inputFileName:dragInfo.files){
        ofLogNotice("file",inputFileName);
        ofFile inputFile(inputFileName);
        report.push_back(inputFile.getFileName());
        
        if(ofToLower(inputFileName).find(".key")==string::npos){
            inputFile.close();
            report.push_back("Not a keynote file. Ignored.");
            continue;
        }
        
        string tempFolder=inputFile.getEnclosingDirectory()+inputFile.getBaseName()+"-temp";
        
        if(inputFile.isDirectory()){
            inputFile.copyTo(tempFolder);
        }else{
            ofSystem("unzip -o \""+inputFileName+"\" -d \""+tempFolder+"\"");
        }
        
        size_t numFiles=dir.listDir(tempFolder+"/Data");
        
        size_t counter=0;
        
        for(size_t i=0;i<numFiles;i++){
            string dataFile=dir.getPath(i);
            
            ofFile tempfile(dataFile);
            
            if(tempfile.getSize()<maxFileSize*1024){
                continue;
            }
            
            ofImage tempImg(tempfile);
            if(tempImg.getWidth()<1000){
                continue;
            }
            counter++;
            if(tempImg.getWidth()>maxWidth){
                tempImg.resize(maxWidth, tempImg.getHeight()*maxWidth/tempImg.getWidth());
            }else if(tempImg.getHeight()>maxWidth){
                tempImg.resize(tempImg.getWidth()*maxWidth/tempImg.getHeight(), maxWidth);
            }
            tempImg.save(dataFile+".jpg",OF_IMAGE_QUALITY_HIGH);
            ofFile::removeFile(dataFile);
            ofFile::moveFromTo(dataFile+".jpg", dataFile);
        }
        report.push_back(ofToString(counter)+" images compressed");
        
        ofSystem("cd " +inputFile.getEnclosingDirectory()+ " && zip -r -1 \""+inputFile.getBaseName()+"-slim.key\" \""+inputFile.getBaseName()+"-temp\"");
        ofDirectory::removeDirectory(tempFolder,true);
    }
}
