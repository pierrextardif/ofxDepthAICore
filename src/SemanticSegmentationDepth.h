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
			std::shared_ptr<dai::node::NeuralNetwork> detectionNN;
			std::shared_ptr<dai::node::ImageManip> croppingManip;
			std::shared_ptr<dai::node::ImageManip> resizingManip;

			std::shared_ptr<dai::DataOutputQueue> qRgb, qNN;

			std::unique_ptr<ofxDepthAICore::DepthAITexConverter> colorTexture, NNTexture;

			int nnSize;
			bool arbTex;
			glm::vec2 previewSize;
			glm::vec2 cropResize;
			
			void loadShader();
			ofShader semanticDepthShader;
    		ofPlanePrimitive plane;
		
		// protected:
    	// 	virtual void threadedFunction();
		// 	std::condition_variable condition;
	};
}