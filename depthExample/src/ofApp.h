#pragma once

#include "ofMain.h"


#include "ofxDepthAICore.h"

using namespace ofxDepthAICore;

/*
class DepthTexture: public ofThread{

public:
	DepthTexture(){
	}
	~DepthTexture(){

        waitForThread(false);
	}

private:

    dai::Pipeline pipeline;

    // Connect to device and start pipeline
    dai::Device* device;

    // Define sources and outputs
    std::shared_ptr<dai::node::MonoCamera> monoLeft;
    std::shared_ptr<dai::node::MonoCamera> monoRight;
    std::shared_ptr<dai::node::StereoDepth> stereo;

    // XLinkOut
    std::shared_ptr<dai::node::XLinkOut> xoutLeft;
    std::shared_ptr<dai::node::XLinkOut> xoutRight;
    std::shared_ptr<dai::node::XLinkOut> xoutDisp;
    std::shared_ptr<dai::node::XLinkOut> xoutDepth;
    std::shared_ptr<dai::node::XLinkOut> xoutRectifL;
    std::shared_ptr<dai::node::XLinkOut> xoutRectifR;

   

    std::shared_ptr<dai::DataOutputQueue> leftQueue;
    std::shared_ptr<dai::DataOutputQueue> rightQueue;
    std::shared_ptr<dai::DataOutputQueue> dispQueue;
    std::shared_ptr<dai::DataOutputQueue> depthQueue;
    std::shared_ptr<dai::DataOutputQueue> rectifLeftQueue;
    std::shared_ptr<dai::DataOutputQueue> rectifRightQueue;


    // Disparity range is used for normalization
    float disparityMultiplier;

public :
	void setup(){
		monoLeft = pipeline.create<dai::node::MonoCamera>();
		monoRight = pipeline.create<dai::node::MonoCamera>();

		stereo = pipeline.create<dai::node::StereoDepth>();
		xoutLeft = pipeline.create<dai::node::XLinkOut>();
		xoutRight = pipeline.create<dai::node::XLinkOut>();
		xoutDisp = pipeline.create<dai::node::XLinkOut>();
		xoutDepth = pipeline.create<dai::node::XLinkOut>();
		xoutRectifL = pipeline.create<dai::node::XLinkOut>();
		xoutRectifR = pipeline.create<dai::node::XLinkOut>();

		xoutLeft->setStreamName("left");
		xoutRight->setStreamName("right");
		xoutDisp->setStreamName("disparity");
		xoutDepth->setStreamName("depth");
		xoutRectifL->setStreamName("rectified_left");
		xoutRectifR->setStreamName("rectified_right");


		// Properties
		monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_480_P);
		monoLeft->setBoardSocket(dai::CameraBoardSocket::LEFT);
		monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_480_P);
		monoRight->setBoardSocket(dai::CameraBoardSocket::RIGHT);

		// StereoDepth
		stereo->setDefaultProfilePreset(dai::node::StereoDepth::PresetMode::HIGH_DENSITY);
		stereo->setRectifyEdgeFillColor(0);  // black, to better see the cutout
		// stereo->setInputResolution(1280, 720);
		stereo->initialConfig.setMedianFilter(dai::MedianFilter::KERNEL_5x5);
		stereo->setLeftRightCheck(true);
		stereo->setExtendedDisparity(false);
		stereo->setSubpixel(false);

		// Linking
		monoLeft->out.link(stereo->left);
		monoRight->out.link(stereo->right);

		stereo->syncedLeft.link(xoutLeft->input);
		stereo->syncedRight.link(xoutRight->input);
		stereo->disparity.link(xoutDisp->input);

		stereo->rectifiedLeft.link(xoutRectifL->input);
		stereo->rectifiedRight.link(xoutRectifR->input);

		stereo->depth.link(xoutDepth->input);

		device = new dai::Device(pipeline);


    	leftQueue = device->getOutputQueue("left", 8, false);
    	rightQueue = device->getOutputQueue("right", 8, false);
    	dispQueue = device->getOutputQueue("disparity", 8, false);
    	depthQueue = device->getOutputQueue("depth", 8, false);
    	rectifLeftQueue = device->getOutputQueue("rectified_left", 8, false);
    	rectifRightQueue = device->getOutputQueue("rectified_right", 8, false);

		disparityMultiplier = 255 / stereo->initialConfig.getMaxDisparity();

		depthTex = std::make_unique < DepthAITexConverter >();
		leftColorTex = std::make_unique < DepthAITexConverter >();
		rightColorTex = std::make_unique < DepthAITexConverter >();

		startThread();
	}
	void threadedFunction(){
        while(isThreadRunning()){

            std::unique_lock<std::mutex> lock(mutex);
			auto left = leftQueue->get<dai::ImgFrame>();
			auto right = rightQueue->get<dai::ImgFrame>();

			auto disparity = dispQueue->get<dai::ImgFrame>();
			cv::Mat disp(disparity->getCvFrame());
			disp.convertTo(disp, CV_8UC1, disparityMultiplier);  // Extend disparity range

			cv::Mat disp_color;
			cv::applyColorMap(disp, disp_color, cv::COLORMAP_JET);
			
			if(!depthTex->whSet()){
				int w = disparity->getWidth();
				int h = disparity->getHeight();
				depthTex->setWH(w, h);
			}
			depthTex->updateBuffer(disp_color.data);

			auto rectifL = rectifLeftQueue->get<dai::ImgFrame>();
			if(!leftColorTex->whSet()){
				int w = rectifL->getWidth();
				int h = rectifL->getHeight();
				leftColorTex->setWH(w, h);
			}
			cv::Mat leftRectMat = rectifL->getCvFrame();
			leftColorTex->updateBuffer(leftRectMat.data);
			
			auto rectifR = rectifRightQueue->get<dai::ImgFrame>();
			if(!rightColorTex->whSet()){
				int w = rectifR->getWidth();
				int h = rectifR->getHeight();
				rightColorTex->setWH(w, h);
			}
			cv::Mat rightRectMat = rectifR->getCvFrame();
			rightColorTex->updateBuffer(rightRectMat.data);
			
            condition.wait(lock);
		}
	}

    


	void update()
	{
		std::unique_lock<std::mutex> lock(mutex);
		depthTex->updateTexture();
		leftColorTex->updateTexture();
		//rightColorTex->updateTexture();
        condition.notify_all();
	}
	void draw()
	{
		depthTex->draw();
		ofPushMatrix();
		ofTranslate(0, depthTex->getHeight());
		leftColorTex->draw();
		//ofTranslate(0, leftColorTex->getHeight());
		//rightColorTex->draw();
		ofPopMatrix();

	}


	private:
		std::unique_ptr< DepthAITexConverter > depthTex, leftColorTex, rightColorTex;

	protected:
		std::condition_variable condition;

};
*/

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		ofEasyCam cam;
		DepthAligned* depthAligned;
};
