//
//  FeatureDetector.cpp
//  HomographyTest
//
//  Created by Martial Geoffre-Rouland on 26/04/2019.
//

#include "ofxFeatureDetector.hpp"

void ofxFeatureDetector::setup() {

    detector = BRISK::create(25, 3 );    
    bHasProcessed = true;


}
void ofxFeatureDetector::update(ofPixels & input) {
    
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
            detector->compute(cam,  keypoints_scene, descriptors_scene);
                
            // detect and compute all images

            FlannBasedMatcher matcher(new flann::LshIndexParams(20,10,2));

            for(int i=0; i<images.size(); i++) {

                if (images[i].empty()) {
                     std::cerr << "Couldn't read image in";
                     continue;
                 }

                // match !
                vector<vector<cv::DMatch>> matches;
                matcher.knnMatch( images[i], descriptors_scene, matches, 2 );

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
                
               // ofLogNotice("search image at ") << i << " with " <<  good_matches.size();

                if( good_matches.size() > 0 ) {
                    ofLogNotice("detected image at ") << i << " with " <<  good_matches.size();
                    detecteds[i] = true;
                } else {
                     detecteds[i] = false;
                }

             }

            bHasProcessed = true;

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
    detector->compute(matImg,  keypoints_object, descriptors_object);

    images.push_back(descriptors_object);
    detecteds.push_back(0);
    
    labels.push_back(label);

}
