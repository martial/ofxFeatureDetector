//
//  FeatureDetector.cpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#include "ofxFeatureDetector.hpp"

void ofxFeatureDetector::setup() {

    detector        = ORB::create(5000);
    extractor       = BRISK::create(60, 4 );
    matcher         = new cv::BFMatcher(cv::NORM_HAMMING, false);
 
    bHasProcessed   = true;
    
    currentTime = ofGetElapsedTimeMillis();
    
    distanceRatio   = 0.5;
    nTries          = 3;
    nMinMatches     = 1;

    bVerbose        = false;
    
    bIsRunning = true;

}
void ofxFeatureDetector::update(ofPixels & input) {
    
    camImg.clear();
    camGrayImg.clear();
        
    camImg.allocate( input.getWidth(),  input.getHeight());
    camGrayImg.allocate( input.getWidth(), input.getHeight());
    
    camImg.setFromPixels(input);
    camGrayImg = camImg;
    
    cv::Mat sceneImg = cv::cvarrToMat(camGrayImg.getCvImage());
    //sceneImg.convertTo(sceneImg, -1, 2, 0);
    
    
    
    if (sceneImg.empty()) {
           std::cerr << "Couldn't read cam in";
           return;
      }

    if(bHasProcessed && bIsRunning) {
        camChannel.send(sceneImg);
        bHasProcessed = false;
    }


}

void ofxFeatureDetector::threadedFunction() {

    while(isThreadRunning()){


           // std::unique_lock<std::mutex> lock(mutex);

            cv::Mat cam;
            while(camChannel.receive(cam)){

                std::vector<cv::KeyPoint> keypoints_scene;
                cv::Mat descriptors_scene;
                
                detector->detect(cam, keypoints_scene);
                extractor->compute(cam,  keypoints_scene, descriptors_scene);
                
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

                      //  ofLogNotice("ofxFeatureDetector ")  << "matches" << matches.size();


                        float totalDistance = 0.0;
                        int nMatches = 0;
                        for(size_t i = 0; i < matches.size(); ++i)
                        {
                            if(matches[i].size() < 2)
                                continue;

                            const cv::DMatch &m1 = matches[i][0];
                            const cv::DMatch &m2 = matches[i][1];
                            
                            float targetDistance = distanceRatio * m2.distance;
                            
                            if(m1.distance <= targetDistance) {
                                good_matches.push_back(m1);
                                totalDistance += targetDistance;


                            }

                            nMatches++;
                            
                        }

                        float medDistance = totalDistance / (float)good_matches.size();
                        if(good_matches.size() == 0)
                        medDistance = 999;

                        if( good_matches.size() >= nMinMatches ) {
                            
                            detectedsScore[i]++;
                        } else {
                            
                            detectedsScore[i]--;
                        }
                        
                        detectedsScore[i] = ofClamp(detectedsScore[i], 0, nTries);
                        
                        if( detectedsScore[i] == 0 ||  detectedsScore[i] >= nTries) {
                            
                            bool bIsDetected = detectedsScore[i] == nTries;
                            
                            if(bIsDetected && bIsDetected != detecteds[i] )
                                ofLogNotice("status image at ") << i << " with and dist " << medDistance;
                            
                            detecteds[i] = bIsDetected;

                        }

                        float blurRate      = 0.9;
                        detectedsDistanceResult[i]      = blurRate *  detectedsDistanceResult[i]   +  (1.0f - blurRate) * medDistance;
                        
                        /*

                        if(detecteds[i]== 1) {
                         
                            float blurRate      = 0.9;
                             detectedsDistanceResult[i]      = blurRate *  detectedsDistanceResult[i]   +  (1.0f - blurRate) * medDistance;
                        } else {
                            detectedsDistanceResult[i] = 999;

                        }
                         
                         */
                        
                    } catch (...) {
                        
                    }

             }

            bHasProcessed = true;
                
            if(bVerbose) {
                
                float timeDiff = ofGetElapsedTimeMillis() - currentTime;
                ofLogNotice("ofxFeatureDetector ")  << " processed in " <<  timeDiff << " millis";
                currentTime = ofGetElapsedTimeMillis();
                    
            }

        }

     //unlock();

      }
    waitForThread();


}

void ofxFeatureDetector::setExtractorSettings(int thresold, int octaves) {
    
    lock();
    extractor       = BRISK::create(thresold, octaves );
    unlock();

    
}



void ofxFeatureDetector::draw() {
    camGrayImg.draw(0,0);

}

bool ofxFeatureDetector::getDetected(int index) {

    return detecteds[index];

}

int ofxFeatureDetector::getLowestScoreIndex(){

    int min_pos = distance(detectedsDistanceResult.begin(),min_element(detectedsDistanceResult.begin(),detectedsDistanceResult.end()));
    return min_pos;
}




void ofxFeatureDetector::addImageToTrack(ofImage & image, string label) {
    
    if(image.isAllocated()) {
        
                
        ofxCvColorImage			img;
        ofxCvGrayscaleImage 	grayImg;

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
        detectedsDistanceResult.push_back(999);

        
    } else {
        ofLogNotice("error while loading image ") << label;
    }

}
