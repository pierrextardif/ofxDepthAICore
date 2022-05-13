#include "SemanticSegmentationDepth.h"


namespace ofxDepthAICore{
	
	SemanticSegmentationDepth::SemanticSegmentationDepth()
	{

	}

	SemanticSegmentationDepth::~SemanticSegmentationDepth()
	{

        //waitForThread(false);
	}


	void SemanticSegmentationDepth::setup()
	{
		arbTex = ofGetUsingArbTex();
		pipeline.setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);
		
		auto rgbOut = pipeline.create<dai::node::XLinkOut>();
    	auto nnOut = pipeline.create<dai::node::XLinkOut>();

		rgbOut->setStreamName("rgb");
		nnOut->setStreamName("segmentation");

		nnSize = 256;
		previewSize.x = 1920;
		previewSize.y = 1080;


		camRgb = pipeline.create<dai::node::ColorCamera>();
		camRgb->setPreviewSize(previewSize.x, previewSize.y);
		camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
		camRgb->setInterleaved(false);
		camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);


        float ratio = (previewSize.x- previewSize.y) / previewSize.x;
        float ratioXMin = ratio / 2.;
        float ratioXMax = 1.0 - ratio / 2.;
        cropResize = { ratioXMin, ratioXMax };

        croppingManip = pipeline.create<dai::node::ImageManip>();
        croppingManip->initialConfig.setCropRect(ratioXMin, 0, ratioXMax, 1);
        croppingManip->setMaxOutputFrameSize(camRgb->getPreviewHeight() * camRgb->getPreviewWidth() * 3);

        resizingManip = pipeline.create<dai::node::ImageManip>();
        resizingManip->initialConfig.setResize(nnSize, nnSize);
        resizingManip->initialConfig.setFrameType(dai::ImgFrame::Type::BGR888p);


		


    	std::string nnPath("/home/pierre/Downloads/Nap-depthai-core-main/mod_depthaicore/data/nn/deeplabv3p_person_6_shaves.blob");
		detectionNN = pipeline.create<dai::node::NeuralNetwork>();
    	detectionNN->setBlobPath(nnPath);
    	detectionNN->setNumInferenceThreads(2);
    	detectionNN->input.setBlocking(false);

		// Linking
		camRgb->preview.link(croppingManip->inputImage);
        croppingManip->out.link(resizingManip->inputImage);
        resizingManip->out.link(detectionNN->input);



		camRgb->preview.link(rgbOut->input);
		detectionNN->out.link(nnOut->input);
		
		device = new dai::Device(pipeline);
		
		qRgb = device->getOutputQueue("rgb", 4, false);
		qNN = device->getOutputQueue("segmentation", 4, false);
		qNN->setBlocking(false);
		

		colorTexture = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
		NNTexture = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
		NNTexture->channels = 1;


		loadShader();
		glm::vec2 res = {float(camRgb->getPreviewWidth()), float(camRgb->getPreviewHeight())};
       	plane.set(res.x * .1, res.y * .1, res.x, res.y);
       	plane.setPosition({0,0,0});
    	plane.mapTexCoords(0, 0, 1., 1.);


	}


	void SemanticSegmentationDepth::update()
	{
		// std::unique_lock<std::mutex> lock(mutex);
		if(qRgb){
				std::shared_ptr<dai::ImgFrame> inRgb = qRgb->tryGet<dai::ImgFrame>();

				if(inRgb)
				{
					colorTexture->updateBuffer(inRgb);
					colorTexture->updateTexture();

				}

				std::shared_ptr<dai::NNData> inNN = qNN->tryGet<dai::NNData>();
				if(inNN)
				{
					NNTexture->updateNNBuffer(inNN, nnSize);
					NNTexture->updateTexture();
				}
		}

	}

	void SemanticSegmentationDepth::draw()
	{

		semanticDepthShader.begin();
        
		if(colorTexture != nullptr)semanticDepthShader.setUniformTexture("rgb", colorTexture->tex, 0);
		if(NNTexture != nullptr)semanticDepthShader.setUniformTexture("mask", NNTexture->tex, 1);
		semanticDepthShader.setUniform2f("u_cropResize", cropResize);
		
		if(arbTex)
		{
			semanticDepthShader.setUniform2f("u_nnSize", glm::vec2(nnSize));
			semanticDepthShader.setUniform2f("u_previewSize", previewSize);	
		}else{
			semanticDepthShader.setUniform2f("u_nnSize", glm::vec2(1.0));
			semanticDepthShader.setUniform2f("u_previewSize", glm::vec2(1.0));	

		}
		
		plane.draw();
		
		semanticDepthShader.end();
	}


	void SemanticSegmentationDepth::loadShader()
	{
		if(ofIsGLProgrammableRenderer()){
			if(semanticDepthShader.load("shadersGL3/NNMaskShader"))std::cout << "loaded OK" << std::endl;
		}else{
			std::cout << "not working" << std::endl;
		}
	}
}