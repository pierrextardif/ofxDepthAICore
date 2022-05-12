#pragma once

#include "ofMain.h"


namespace ofxDepthAICore{
	/*
	stores the content of cv::Mat 
	into a unsigned char * 
	then uploads it to a ofTexture
	*/
	struct DepthAITexConverter{

		ofTexture tex;
		unsigned char* buffer = nullptr;
		int w = -1,h = -1;
		int channels = -1;

		DepthAITexConverter()
		{

		}

		void setWH(int _w, int _h)
		{
			w = _w;
			h = _h;

		}
		bool whSet()
		{
			return (w != -1 && h != -1 ) ? true : false;
		}

		DepthAITexConverter(int _w, int _h, int channels)
		{
			w = _w;
			h = _h;

		}

		void updateBuffer(cv::Mat* cvFrame, int w, int h)
		{
			if(!whSet()){
				setWH(w, h);
			}
			updateBuffer(cvFrame->data);
		}

		void updateBuffer(std::shared_ptr<dai::ImgFrame> imgFrame)
		{
			if(!whSet()){
				int w = imgFrame->getWidth();
				int h = imgFrame->getHeight();
				setWH(w, h);
			}
			cv::Mat cvFrame = imgFrame->getCvFrame();
			updateBuffer(cvFrame.data);
		}

		void updateNNBuffer(std::shared_ptr<dai::NNData> inNN, int nnSize)
		{

			if(!whSet()){
				setWH(nnSize, nnSize);
			}

			std::vector<dai::TensorInfo> vecAllLayers = inNN->getAllLayers();
			if(vecAllLayers.size() > 0){
				std::vector<std::int32_t> layer1 = inNN->getFirstLayerInt32();
            	if(layer1.size() > 0) {
					//std::cout << "layer1.size() = " << ofToString(layer1.size()) << std::endl;
					updateBuffer((uint8_t*)layer1.data());
				}

			}

		}

		void updateBuffer(uint8_t* frameData)
		{
			buffer = frameData;
		}

		void updateTexture()
		{
			if(w != -1 && h != -1 && channels == -1)
			{
				tex.loadData(buffer, w, h, GL_RGB);
			}else if(w != -1 && h != -1 && channels == 1){
				tex.loadData(buffer, w, h, GL_LUMINANCE);

			}

		}
		void updateNNTexture()
		{
			if(w != -1 && h != -1 && buffer != nullptr)tex.loadData(buffer, w, h, GL_RED);

		}

		void draw()
		{
			if(tex.isAllocated())tex.draw(0,0,0);
		}

		int getWidth()
		{
			return (tex.isAllocated()) ? tex.getWidth() : 1;
		}

		int getHeight()
		{
			return (tex.isAllocated()) ? tex.getHeight() : 1;
		}

	};
}