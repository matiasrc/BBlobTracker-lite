#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    //ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    //----------------- XML -------------------
    loadSettings();
    
    //----------------- CAM -------------------
    camWidth            = 640;
    camHeight           = 480;
    imagePixelNumber    = camWidth * camHeight;
    deviceName          = "NO DEVICE AVAILABLE";

    isOneDeviceAvailable =  false;
    
    setupCam(deviceID);
    
    mirroredImg.allocate(camWidth,camHeight);
    
    //-----------------SYPHON ------------------
 
    
    //-------BG SUBTRACTION ------
    grayImg.allocate(camWidth,camHeight);
    grayBg.allocate(camWidth,camHeight);
    bitonal.allocate(camWidth,camHeight);
    adaptiveBg.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE );
    
    bLearnBackground = false;
    clearBg = false;
    
    loadBackgroundImage();
    
    //-----------COUNTOUR FINDER ---------
    contourFinder.setThreshold(threshold);
    contourFinder.setMinArea(minArea / 100 * imagePixelNumber);
    contourFinder.setMaxArea(maxArea / 100 * imagePixelNumber);
    contourFinder.setInvert(invert); // find black instead of white
    contourFinder.setFindHoles(findHoles);
    // wait for half a second before forgetting something
    contourFinder.getTracker().setPersistence(persistence);
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(distance);
    
    activeBlobs = 0;
    //----------------- GUI -------------------
    //required call
    gui.setup();
    
    ImGui::GetIO().MouseDrawCursor = false;
            
    //----------------- OSC -------------------    
    sender.setup(host, puerto);
}

void ofApp::update() {
	cam.update();
    
    w = ofGetWidth();
    h = ofGetHeight();
    
	if(cam.isFrameNew()) {
        
        mirroredImg.setFromPixels(cam.getPixels());
        mirroredImg.mirror(vMirror, hMirror);
                
        //------BG SUBTRACTION-------------
        grayImg = mirroredImg;
        grayImg.updateTexture();
        bitonal.absDiff(grayBg, grayImg);

        bitonal.threshold(threshold);
        bitonal.updateTexture();
        
        //////////////////////////////////////////////
        // background learning

        // static
        if(bLearnBackground == true){
            bLearnBackground = false;
            grayBg = grayImg;
            grayBg.updateTexture();
            ofLogNotice()<< "captura fondo una vez";
        }
        if(clearBg){
            clearBg = false;
            grayBg.set(0);
        }
        // adaptive
        if(adaptive){
            
            lerp(grayImg, adaptiveBg, adaptiveBg, adaptSpeed);
            
            //Mat temp = toCv(grayBg);
            //temp = toCv(grayBg)*(1.0f-adaptSpeed) + toCv(grayImg)*adaptSpeed;
            //toOf(temp, grayBg.getPixels());

            grayBg = adaptiveBg;
            grayBg.updateTexture();
            
            //finalBg = grayBg;
            //finalBg.updateTexture();
        }
        //------- CONTOUR FINDER ---------
        contourFinder.findContours(bitonal);
    
        //------- OSC ---------
        updateOsc();
    }
}

void ofApp::draw() {
    
    ofPushStyle();
    ofSetColor(255);
    
    w = ofGetWidth();
    h = ofGetHeight();
    
    offset_x = 0;
    offset_y = 0;
    
    if (imageView == 0){ // TODAS
        w = ofGetWidth() / 2;
        h = ofGetHeight() / 2;
        offset_x = w;
        offset_y = h;
        mirroredImg.draw(0, 0, w, h);
        grayImg.draw(w, 0, w, h);
        grayBg.draw(0, h, w, h);
        bitonal.draw(w, h, w, h);
    }
    else if(imageView == 1){ //
        mirroredImg.draw(0, 0, w, h); // imagen original de la cámara
    }
    else if(imageView == 2){ //
        grayImg.draw(0, 0, w, h); // imagen en escala de grises
    }
    else if(imageView == 3){
        grayBg.draw(0, 0, w, h); // Background
    }
    else{
        bitonal.draw(0, 0, w, h); // Bitonal (blano y negro)
    }
    ofPopStyle();
    

    ofPushStyle();
    ofSetColor(0, 255, 0);
    
    ofNoFill();

    for(int i = 0; i < activeBlobs; i++) {
     
        // some different styles of contour centers
        ofVec2f centroid = toOf(contourFinder.getCentroid(i));
        ofVec2f average = toOf(contourFinder.getAverage(i));
        ofVec2f center = toOf(contourFinder.getCenter(i));
        ofSetColor(cyanPrint);
        ofDrawCircle(centroid.x / camWidth * w + offset_x, centroid.y / camHeight * h + offset_y, 4);
        ofSetColor(magentaPrint);
        ofDrawCircle(average.x / camWidth * w + offset_x, average.y / camHeight * h + offset_y, 4);
        ofSetColor(yellowPrint);
        ofDrawCircle(center.x / camWidth * w + offset_x, center.y / camHeight * h + offset_y, 4);

        // you can also get the area and perimeter using ofPolyline:
        // ofPolyline::getArea() and ofPolyline::getPerimeter()
        double area = contourFinder.getContourArea(i);
        double perimeter = contourFinder.getArcLength(i);
        
        
        ofSetColor(0, 0, 255);
        cv::Rect r = contourFinder.getBoundingRect(i);
        ofRectangle br = toOf(r);
        ofDrawRectangle(br.x / camWidth * w + offset_x, br.y / camHeight * h + offset_y, br.width / camWidth * w, br.height / camHeight * h);
        
        ofSetColor(0, 255, 0);
        for(std::size_t j = 0; j < contourFinder.getContour(i).size()-1; j++) {
            cv::Point p1 = contourFinder.getContour(i).at(j);
            cv::Point p2 = contourFinder.getContour(i).at(j+1);
            ofDrawLine(p1.x / camWidth * w + offset_x, p1.y / camHeight * h + offset_y,
                       p2.x / camWidth * w + offset_x, p2.y / camHeight * h + offset_y);
        }
        
        ofSetColor(yellowPrint);
        //ofPoint center = toOf(contourFinder.getCenter(i));
        ofPushMatrix();
        ofTranslate(center.x / camWidth * w + offset_x, center.y / camHeight * h + offset_y);
        int label = contourFinder.getLabel(i);
        string msg1 = "ID: " + ofToString(label);
        string msg2 = "AGE: " + ofToString(contourFinder.getTracker().getAge(label));
        ofDrawBitmapString(msg1, 8, 0);
        ofDrawBitmapString(msg2, 8, 15);
        
        
        ofSetColor(255, 0, 0);
        ofVec2f velocity = toOf(contourFinder.getVelocity(i));
        ofScale(5, 5);
        ofSetLineWidth(1.5);
        ofDrawLine(offset_x, offset_y, velocity.x / camWidth * w + offset_x, velocity.y / camHeight * h + offset_y);
        ofPopMatrix();

        
        ofSetColor(255);
        if(contourFinder.getHole(i)) {
            //ofDrawBitmapStringHighlight("hole", center.x, center.y);
        }
        ofSetColor(255);
        ofDrawBitmapStringHighlight("Objetos (Blobs) encontrados: " + ofToString(activeBlobs), 5, ofGetHeight() - 5);
    }
    ofPopStyle();
    
    //-----------------SYPHON & SPOUT ------------------

    //----------------- GUI -------------------
    drawGui();
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs& e){
    
    #if __APPLE__
    if(e.key == 's' && e.hasModifier(OF_KEY_COMMAND)){
        saveSettings();
        saveBackgroundImage();
    }
    #else
    if(e.key == 19 ){
        saveSettings();
        saveBackgroundImage();
    }
    
    #endif
    else if(e.key == ' '){
        bLearnBackground = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs& e){
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------

void ofApp::saveBackgroundImage() {
  ofImage tempImage;
  tempImage.setFromPixels(grayBg.getPixels());
  tempImage.save("background.png"); // Puedes cambiar el nombre del archivo aquí
  ofLogNotice() << "Imagen de fondo guardada como background.png";
}
//--------------------------------------------------------------
void ofApp::loadBackgroundImage() {
    // Ruta al archivo de imagen (puedes modificarla)
    string imagePath = "background.png";

    // Crear una instancia temporal de ofImage
    ofImage tempImage;

    // Intentar cargar la imagen
    if (tempImage.load(imagePath)) {
        // Convertir a ofxCvGrayscaleImage
        grayBg.setFromPixels(tempImage.getPixels());
        ofLogNotice() << "Imagen de fondo cargada correctamente: " << imagePath;
    } else {
        ofLogError() << "Error al cargar la imagen de fondo: " << imagePath;
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    saveSettings();
    saveBackgroundImage();
}

