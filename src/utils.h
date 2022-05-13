#pragma once

#include "ofMain.h"

#define SEMANTIC_CHANNELS GL_RGBA

namespace ofxDepthAICore{
	/*
	stores the content of cv::Mat 
	into a unsigned char * 
	then uploads it to a ofTexture
	*/
	struct DepthAITexConverter{

		ofTexture tex;
		unsigned char* buffer = nullptr;
		int32_t* extraBuff = nullptr;
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
			//cv::cvtColor(cvFrame, cvFrame, cv::COLOR_BGR2RGB);
			updateBuffer(cvFrame.data);
		}

		void updateNNBuffer(std::shared_ptr<dai::NNData> inNN, int nnSize)
		{

			if(!whSet()){
				setWH(nnSize, nnSize);
			}

			std::vector<dai::TensorInfo> vecAllLayers = inNN->getAllLayers();

			if (vecAllLayers.size() > 0) {

				std::vector<std::int32_t> layer1 = inNN->getLayerInt32(vecAllLayers[0].name);
				if (layer1.size() > 0) {

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
				tex.loadData(buffer, w, h, SEMANTIC_CHANNELS);

			}

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