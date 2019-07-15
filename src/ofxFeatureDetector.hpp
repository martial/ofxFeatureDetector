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

        //stop();
    }
    
    void setup();
    void update(ofPixels & input);
    void threadedFunction();
    void draw();
    
    void setExtractorSettings(int thresold, int octaves);

    bool getDetected(int index);
    int getLowestScoreIndex();
    
    void addImageToTrack(ofImage  & image, string label);
    
    /// Start the thread.
    void start(){
        startThread();
    }
    void stop(){
        
        waitForThread(true);

    }
    
    vector<string> labels;
    
    float distanceRatio;
    int nTries, nMinMatches;
    
    bool bIsRunning;
    
    ofxCvColorImage            camImg;
    ofxCvGrayscaleImage        camGrayImg;
    ofImage resultImg;
    vector<float>                 detectedsDistanceResult;

    vector<int>     detectedsScore;

private:
    
    Ptr<FeatureDetector>            detector;
    Ptr<DescriptorExtractor> extractor;
    Ptr<cv::DescriptorMatcher> matcher;

    vector<cv::Mat> images;
    cv::Mat sceneImg;
    
    ofThreadChannel<cv::Mat> camChannel;
    int nChannels;
    bool bHasProcessed;

    vector<bool>    detecteds;

    float currentTime;
    
    bool bVerbose;
    
    std::condition_variable condition;

};

#endif /* FeatureDetector_hpp */
