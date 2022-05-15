#pragma once

#include "ofxDepthAICore.h"


namespace ofxDepthAICore{
	class SemanticSegmentationDepth	
	{
		public :
			SemanticSegmentationDepth();
			~SemanticSegmentationDepth();

			void setup();
			void update();
			void draw();


		private :

			dai::Pipeline pipeline;
			dai::Device* device;

			std::shared_ptr<dai::node::ColorCamera> camRgb;
			std::shared_ptr<dai::node::MonoCamera> left;
			std::shared_ptr<dai::node::MonoCamera> right;
			std::shared_ptr<dai::node::StereoDepth> stereo;
			std::shared_ptr<dai::node::NeuralNetwork> detectionNN;
			std::shared_ptr<dai::node::ImageManip> croppingManip;
			std::shared_ptr<dai::node::ImageManip> resizingManip;

			std::shared_ptr<dai::DataOutputQueue> qRgb, qNN;

    		std::unordered_map<std::string, cv::Mat> frame;			
			std::unordered_map<std::string, std::unique_ptr<ofxDepthAICore::DepthAITexConverter > > textures;

			std::unique_ptr<ofxDepthAICore::DepthAITexConverter> colorTexture, NNTexture;


			float maxDisparity;
			glm::vec2 resDepth, resDepthReMapped;
			std::vector<std::string> queueNames;
			
			int nnSize;
			bool arbTex;
			glm::vec2 previewSize;
			glm::vec2 cropResize;
			
			void loadShader();
			ofShader semanticDepthShader;
    		ofPlanePrimitive plane;
	};
}