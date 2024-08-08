

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::loadSettings(){
    //-----------
    //the string is printed at the top of the app
    //to give the user some feedback
    xmlMessage = "loading mySettings.xml";

    //we load our settings file
    //if it doesn't exist we can still make one
    //by hitting the 's' key
    if( XML.load("mySettings.xml") ){
        xmlMessage = "mySettings.xml loaded!";
    }else{
        xmlMessage = "unable to load mySettings.xml check data/ folder";
    }
        
    
    //---------------- BG SUBTRACTION - CALIBRACION --------------------
    threshold = XML.getValue("BGSUBTRACTION:THRESHOLD", 127);
    adaptive = XML.getValue("BGSUBTRACTION:ADAPTATIVE", false);
    adaptSpeed = XML.getValue("BGSUBTRACTION:ADAPTSPEED", 0.01);
    
    //---------------- BLOBS CONTOUR FINDER --------------------
    minArea = XML.getValue("CV:MINAREA", 0.0);
    maxArea = XML.getValue("CV:MAXAREA", 1.0);
    persistence = XML.getValue("CV:PERSISTENCE", 15);
    distance = XML.getValue("CV:DISTANCE", 30);
    invert = XML.getValue("CV:INVERT", false);
    findHoles = XML.getValue("CV:FINDHOLES", false);
    
    //---------------- ENTRADA --------------------
    deviceID = XML.getValue("CAM:DEVICEID", 0);
    hMirror = XML.getValue("CAM:HMIRROR", false);
    vMirror = XML.getValue("CAM:VMIRROR", false);

    
    //---------------- OSC --------------------
    puerto = XML.getValue("OSC:PUERTO", 12345);
    host = XML.getValue("OSC:HOST", "127.0.0.1");
    etiquetaMensajeBlobs = XML.getValue("OSC:ETIQUETA:BLOBS", "/bblobtracker/blobs");
    enviarBlobs = XML.getValue("OSC:ENVIARBLOBS", true);
    enviarContornos = XML.getValue("OSC:ENVIARCONTORNOS", false);
    
    //---------------- VER --------------------
    imageView = XML.getValue("VIEW:IMAGEVIEW", 0);

    ofLog(OF_LOG_NOTICE,xmlMessage);
}
//--------------------------------------------------------------
void ofApp::saveSettings(){

    XML.clear();

    //---------------- BG SUBTRACTION - CALIBRACION --------------------
    XML.setValue("BGSUBTRACTION:THRESHOLD", threshold);
    XML.setValue("BGSUBTRACTION:ADAPTATIVE", adaptive);
    XML.setValue("BGSUBTRACTION:ADAPTSPEED", adaptSpeed);
    
    //---------------- CONTOUR FINDER --------------------
    XML.setValue("CV:MINAREA", minArea);
    XML.setValue("CV:MAXAREA", maxArea);
    XML.setValue("CV:PERSISTENCE", persistence);
    XML.setValue("CV:DISTANCE", distance);
    XML.setValue("CV:INVERT", invert);
    XML.setValue("CV:FINDHOLES", findHoles);
    
    //---------------- CAM --------------------
    XML.setValue("CAM:DEVICEID", deviceID);
    XML.setValue("CAM:HMIRROR", hMirror);
    XML.setValue("CAM:VMIRROR", vMirror);
    /*
    XML.setValue("CAM:WARPING:PASO", paso);
    XML.setValue("CAM:WARPING:AX", warp[0].x);
    XML.setValue("CAM:WARPING:Ay", warp[0].y);
    XML.setValue("CAM:WARPING:BX", warp[1].x);
    XML.setValue("CAM:WARPING:BY", warp[1].y);
    XML.setValue("CAM:WARPING:CX", warp[2].x);
    XML.setValue("CAM:WARPING:CY", warp[2].y);
    XML.setValue("CAM:WARPING:DX", warp[3].x);
    XML.setValue("CAM:WARPING:DY", warp[3].y);
     */
    
    //---------------- OSC --------------------
    XML.setValue("OSC:PUERTO", puerto);
    XML.setValue("OSC:HOST", host);
    XML.setValue("OSC:ETIQUETA:BLOBS", etiquetaMensajeBlobs);
    XML.setValue("OSC:ENVIARBLOBS", enviarBlobs);
    XML.setValue("OSC:ENVIARCONTORNOS", enviarContornos);
    
    //---------------- VER --------------------
    XML.setValue("VIEW:IMAGEVIEW", imageView);
        
    XML.saveFile("mySettings.xml");
    xmlMessage ="settings saved to xml!";
    ofLog(OF_LOG_NOTICE,xmlMessage);
}
