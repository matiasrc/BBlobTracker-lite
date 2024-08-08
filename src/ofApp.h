#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#if __APPLE__
#include "ofxSyphon.h"
#else
#endif



class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(ofKeyEventArgs& e);
    void keyReleased(ofKeyEventArgs& e);
    void setupCam(int devID);
    void resetCameraSettings(int devID);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void exit();

    //----------------- VIEW -------------------
    int                     imageView;
    float                   w;
    float                   h;
    
    float                   offset_x;
    float                   offset_y;
	
    //----------------- CAM -------------------
    ofVideoGrabber          cam;
    ofxCvColorImage         mirroredImg;
    //ofxCvColorImage         warpedImg;
    vector<ofVideoDevice>   wdevices;
    vector<string>          devicesVector;
    vector<int>             devicesID;
    float                   camWidth, camHeight;
    int                     imagePixelNumber;
    string                  deviceName;
    int                     deviceID;
    bool                    needReset;
    bool                    isOneDeviceAvailable;
    bool                    hMirror, vMirror;
        
    //-----------------SYPHON ------------------
    
    
    //----------------- BG SUBTRACTION -------------------
    ofxCvGrayscaleImage     grayImg;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     bitonal;
    
    ofImage                 adaptiveBg;
        
    bool                    adaptive;
    float                   adaptSpeed;
    
    bool                    bLearnBackground;
    bool                    clearBg;
    void                    loadBackgroundImage();
    void                    saveBackgroundImage();
    
    //---------------- CONTOUR FINDER --------------------
    ofxCv::ContourFinder    contourFinder;
    
    //bool                    trackColor;
    //bool                    trackHs;
    //bool                    captureColor;
    //ofColor                 targetColor;
    
    float                   threshold;
    bool                    invert;
    bool                    findHoles;
    
    float                   minArea;
    float                   maxArea;
    int                     persistence;
    int                     distance;
    
    // some different styles of contour centers
    int                     activeBlobs;

    //----------------- GUI -------------------
    void                    drawGui();
    ofxImGui::Gui           gui;
            
    //----------------- OSC -------------------
    ofxOscSender            sender;
    int                     puerto;
    string                  host;
    string                  etiquetaMensajeBlobs;
    bool                    enviarBlobs;
    bool                    enviarContornos;
    
    void                    enviarOsc(string etiqueda, float valor);
    void                    enviarOsc(string etiqueda, vector<float> valores);
    void                    updateOsc();
    
    //----------------- XML -------------------
    ofxXmlSettings          XML;
    void                    saveSettings();
    void                    loadSettings();
    string                  xmlMessage;    
};
