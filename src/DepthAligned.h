#pragma once

#include "ofxDepthAICore.h"


namespace ofxDepthAICore{
	class DepthAligned : public ofThread
	{
		public :
			DepthAligned();
			~DepthAligned();

			void setup();
			void update();
			void draw();


		private :

			dai::Pipeline pipeline;
			dai::Device* device;

			std::vector<std::string> queueNames;

			std::shared_ptr<dai::node::ColorCamera> camRgb;
			std::shared_ptr<dai::node::MonoCamera> left;
			std::shared_ptr<dai::node::MonoCamera> right;
			std::shared_ptr<dai::node::StereoDepth> stereo;


    		std::unordered_map<std::string, cv::Mat> frame;

			
			std::unordered_map<std::string, std::unique_ptr<ofxDepthAICore::DepthAITexConverter > > textures;
    		
			std::string loadVertShader();
			std::string loadFragShader();
			void loadShader();
			
			ofShader depthShader;
    		ofPlanePrimitive plane;


    		ofVboMesh mesh;

			float maxDisparity;
			glm::vec2 resDepth, resDepthReMapped;
		
		protected:
    		virtual void threadedFunction();
			std::condition_variable condition;
	};
}