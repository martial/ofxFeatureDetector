//
//  FeatureDetector.hpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#ifndef FeatureDetector_hpp
#define FeatureDetector_hpp

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

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

    bool getDetected(int index);
    
    void addImageToTrack(ofImage  & image, string label);
    
    /// Start the thread.
    void start(){
        startThread();
    }
    void stop(){
        std::unique_lock<std::mutex> lck(mutex);
        stopThread();
    }
    
    vector<string> labels;

    
    
private:
    
    //Ptr<cv::ORB>            detector;
    //Ptr<DescriptorMatcher>  matcher;
    
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;

    //cv::Ptr<ORB> detector;
    //FlannBasedMatcher * matcher;
    ofxCvColorImage			camImg;
    ofxCvColorImage 	camGrayImg;
    Ptr<cv::DescriptorMatcher> matcher;

    vector<cv::Mat> images;

    ofThreadChannel<cv::Mat> camChannel;
    int nChannels;
    bool bHasProcessed;

    vector<bool>    detecteds;
    vector<int>     detectedsScore;
    
    float currentTime;
    
};

#endif /* FeatureDetector_hpp */
