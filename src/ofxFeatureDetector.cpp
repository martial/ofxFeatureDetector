//
//  FeatureDetector.cpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#include "ofxFeatureDetector.hpp"

void ofxFeatureDetector::setup() {

    //detector = BRISK::create(30, 4 );
    detector = ORB::create(1000);
    extractor = ORB::create(1000);
    matcher = new cv::BFMatcher(cv::NORM_HAMMING, false);
 
    bHasProcessed = true;
    
    currentTime = ofGetElapsedTimeMillis();


}
void ofxFeatureDetector::update(ofPixels & input) {
    
    camImg.clear();
    camGrayImg.clear();
    
    camImg.allocate(640, 480);
    camGrayImg.allocate(640, 480);
    
    camImg.setFromPixels(input);
    camGrayImg = camImg;

    cv::Mat sceneImg = cv::cvarrToMat(camGrayImg.getCvImage());
    if (sceneImg.empty()) {
           std::cerr << "Couldn't read cam in";
           return;
      }

    if(bHasProcessed) {
        camChannel.send(sceneImg);
        bHasProcessed = false;
    }


}

void ofxFeatureDetector::threadedFunction() {

    while(isThreadRunning()){

           lock();

            cv::Mat cam;
            while(camChannel.receive(cam)){

            std::vector<cv::KeyPoint> keypoints_scene;
            cv::Mat descriptors_scene;
                
            detector->detect(cam, keypoints_scene);
            extractor->compute(cam,  keypoints_scene, descriptors_scene);
                
            // detect and compute all images

            //FlannBasedMatcher matcher(new flann::LshIndexParams(20,10,2));
            //Ptr<cv::DescriptorMatcher> matcher(new cv::BFMatcher(cv::NORM_HAMMING, false));
                
            float distRatio = 0.5;
           // ofLogNotice("distRatioat ") << distRatio;

            for(int i=0; i<images.size(); i++) {

                if (images[i].empty()) {
                     std::cerr << "Couldn't read image in";
                     continue;
                 }
                
                try {
                    
                    // match !
                    vector<vector<cv::DMatch>> matches;
                    matcher->knnMatch( images[i], descriptors_scene, matches, 2 );

                    vector<cv::DMatch> good_matches;
                    good_matches.reserve(matches.size());
                    
  
                
                    float totalScore = 0.0;
                    for(size_t i = 0; i < matches.size(); ++i)
                    {
                        if(matches[i].size() < 2)
                            continue;

                        const cv::DMatch &m1 = matches[i][0];
                        const cv::DMatch &m2 = matches[i][1];
                        
                        float targetDistance = distRatio * m2.distance;
                        
                        if(m1.distance <= targetDistance) {
                            good_matches.push_back(m1);
                            
                            float pct = 1.0 - ofNormalize(m1.distance, 0, targetDistance);
                            totalScore += pct;
                            
                        }
                        
                        
                    }
                    
                    float scorePct = 0;
                    if(good_matches.size() > 0 ) {
                        scorePct = totalScore / (float)good_matches.size();
                    }
                    
                    
                    if( good_matches.size() > 3 ) {
                        
                        detectedsScore[i]++;
                    } else {
                        
                        detectedsScore[i]--;
                    }
                    
                    detectedsScore[i] = ofClamp(detectedsScore[i], 0, 3);
                    
                    if( detectedsScore[i] == 0 ||  detectedsScore[i] == 3) {
                        
                        bool bIsDetected = detectedsScore[i] == 3;
                        
                        if(bIsDetected != detecteds[i] )
                            ofLogNotice("status image at ") << i << " with " <<  detecteds[i];

                        detecteds[i] = bIsDetected;

                    }
                    
                } catch (...) {
                    
                }

             }

            bHasProcessed = true;
        
            float timeDiff = ofGetElapsedTimeMillis() - currentTime;
            ofLogNotice("ofxFeatureDetector ")  << " processed in " <<  timeDiff << " millis";
            currentTime = ofGetElapsedTimeMillis();

        }

     unlock();

      }


}


void ofxFeatureDetector::draw() {
    camGrayImg.draw(0,0);

}

bool ofxFeatureDetector::getDetected(int index) {

    return detecteds[index];

}


void ofxFeatureDetector::addImageToTrack(ofImage & image, string label) {
    
    if(image.isAllocated()) {
    
        ofxCvColorImage			img;
        ofxCvColorImage 	grayImg;

        img.allocate(image.getWidth(), image.getHeight());
        grayImg.allocate(image.getWidth(), image.getHeight());

        img.setFromPixels(image.getPixels());
        grayImg = img;

        cv::Mat matImg = cv::cvarrToMat(grayImg.getCvImage());

        std::vector<cv::KeyPoint> keypoints_object;
        cv::Mat descriptors_object;

        detector->detect(matImg, keypoints_object);
        extractor->compute(matImg,  keypoints_object, descriptors_object);

        images.push_back(descriptors_object);
        detecteds.push_back(0);
        detectedsScore.push_back(0);
        labels.push_back(label);
        
       // ofLogNotice("image loaded ok ") << label;

        
    } else {
        //ofLogNotice("error while loading image ") << label;
    }

}
