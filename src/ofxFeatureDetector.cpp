//
//  FeatureDetector.cpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#include "ofxFeatureDetector.hpp"

void ofxFeatureDetector::setup() {
    
}
void ofxFeatureDetector::update(ofPixels & input) {
    
    cv::Mat scene(input.getHeight(), input.getWidth(), CV_8UC3, input.getData());
    camChannel.send(scene);

}

void ofxFeatureDetector::threadedFunction() {
    
    while(isThreadRunning()){
        
       lock();
        
        cv::Mat cam;
        while(camChannel.receive(cam)){
            
            int minHessian = 800;
            
            std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;
            
            // detect and compute input
            cv::Ptr<SURF> detector = SURF::create(minHessian);
            detector->detect( cam, keypoints_scene );
            cv::Mat descriptors_scene;
            Ptr<SURF> extractor = SURF::create();

            extractor->compute( cam, keypoints_scene, descriptors_scene );
            
            cv::Ptr<cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();

            
            // detect and compute all images
            
            for(int i=0; i<images.size(); i++) {
                
                std::vector<cv::KeyPoint> keypoints_object;
                detector->detect( images[i], keypoints_object );
                cv::Mat descriptors_object;
                extractor->compute( images[i], keypoints_object, descriptors_object );
                
                
                // match !
                vector<vector<cv::DMatch>> matches;
                matcher->knnMatch( descriptors_object, descriptors_scene, matches, 2 );
                
                vector<cv::DMatch> good_matches;
                good_matches.reserve(matches.size());
                
                
                for(size_t i = 0; i < matches.size(); ++i)
                {
                    if(matches[i].size() < 2)
                        continue;
                    
                    const cv::DMatch &m1 = matches[i][0];
                    const cv::DMatch &m2 = matches[i][1];
                    
                    if(m1.distance <= 0.5 * m2.distance)
                        good_matches.push_back(m1);
                }
                
                if( good_matches.size() >=8 && good_matches.size() <= 60) {
                    ofLogNotice("detected image at ") << i << " with " <<  good_matches.size();
                }
                
            }

        }
        unlock();
    }
    
}


void ofxFeatureDetector::draw() {
    
}

void ofxFeatureDetector::addImageToTrack(ofImage & image) {
    
    image.resize(640, 480);
    
    cv::Mat img(image.getHeight(), image.getWidth(), CV_8UC3, image.getPixels().getData());
    images.push_back(img);
    channels[nChannels].send(img);
    nChannels++;
}
