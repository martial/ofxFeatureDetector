//
//  FeatureDetector.hpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#ifndef FeatureDetector_hpp
#define FeatureDetector_hpp

#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

class ofxFeatureDetector : public ofThread {
  
public:
    
    ofxFeatureDetector () {
        nChannels = 0;
    }
    ~ofxFeatureDetector(){
        stop();
        waitForThread(false);
    }
    
    void setup();
    void update(ofPixels & input);
    void threadedFunction();
    void draw();
    
    void addImageToTrack(ofImage & image);
    
    /// Start the thread.
    void start(){
        startThread();
    }
    void stop(){
        std::unique_lock<std::mutex> lck(mutex);
        stopThread();
    }
    
    
    
    
private:
    
    cv::SurfDescriptorExtractor extractor;
    cv::FlannBasedMatcher matcher;
    
    vector<cv::Mat> images;
    ofThreadChannel<cv::Mat> camChannel;
    ofThreadChannel<cv::Mat> channels[12];
    int nChannels;

};

#endif /* FeatureDetector_hpp */
