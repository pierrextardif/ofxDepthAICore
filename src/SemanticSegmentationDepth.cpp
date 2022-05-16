#include "SemanticSegmentationDepth.h"


namespace ofxDepthAICore{
	
	SemanticSegmentationDepth::SemanticSegmentationDepth()
	{

	}

	SemanticSegmentationDepth::~SemanticSegmentationDepth()
	{

	}


	void SemanticSegmentationDepth::setup()
	{
		arbTex = ofGetUsingArbTex();
		pipeline.setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);
		
		auto rgbOut = pipeline.create<dai::node::XLinkOut>();
    	auto nnOut = pipeline.create<dai::node::XLinkOut>();
    	auto depthOut = pipeline.create<dai::node::XLinkOut>();

		rgbOut->setStreamName("rgb");
    	queueNames.push_back("rgb");
		nnOut->setStreamName("segmentation");
		depthOut->setStreamName("depth");
    	queueNames.push_back("depth");

		nnSize = 256;
		previewSize.x = 1920;
		previewSize.y = 1080;


		camRgb = pipeline.create<dai::node::ColorCamera>();
		camRgb->setPreviewSize(previewSize.x, previewSize.y);
		camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
		camRgb->setInterleaved(false);
		camRgb->setFps(30);
		camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
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

		left = pipeline.create<dai::node::MonoCamera>();
		right = pipeline.create<dai::node::MonoCamera>();
		stereo = pipeline.create<dai::node::StereoDepth>();


		left->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
		left->setBoardSocket(dai::CameraBoardSocket::LEFT);
		left->setFps(30);
		right->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
		right->setBoardSocket(dai::CameraBoardSocket::RIGHT);
		right->setFps(30);


		stereo->setDefaultProfilePreset(dai::node::StereoDepth::PresetMode::HIGH_DENSITY);
		// LR-check is required for depth alignment
		stereo->setLeftRightCheck(true);
		stereo->setDepthAlign(dai::CameraBoardSocket::RGB);
		stereo->setExtendedDisparity(false);

		auto config = stereo->initialConfig.get();
		config.postProcessing.speckleFilter.enable = false;
		config.postProcessing.speckleFilter.speckleRange = 200;
		config.postProcessing.temporalFilter.enable = false;
		config.postProcessing.spatialFilter.enable = false;
		config.postProcessing.spatialFilter.holeFillingRadius = 10;
		config.postProcessing.spatialFilter.numIterations = 4;
		config.postProcessing.thresholdFilter.minRange = 100;
		config.postProcessing.thresholdFilter.maxRange = 65000;
		config.postProcessing.decimationFilter.decimationFactor = 1;

		
		stereo->initialConfig.set(config);


		// Linking
		camRgb->preview.link(croppingManip->inputImage);
        croppingManip->out.link(resizingManip->inputImage);
        resizingManip->out.link(detectionNN->input);

		camRgb->isp.link(rgbOut->input);
		left->out.link(stereo->left);
		right->out.link(stereo->right);
		stereo->disparity.link(depthOut->input);



		camRgb->preview.link(rgbOut->input);
		detectionNN->out.link(nnOut->input);
		
		device = new dai::Device(pipeline);

		for(const auto& name : queueNames) {
			device->getOutputQueue(name, 4, false);
		}

		
		qNN = device->getOutputQueue("segmentation", 4, false);
		qNN->setBlocking(false);
		

		NNTexture = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
		NNTexture->channels = 1;


		loadShader();
		glm::vec2 res = {float(camRgb->getPreviewWidth()), float(camRgb->getPreviewHeight())};
		resDepth = res;
		//{float(left->getResolutionWidth()), float(left->getResolutionHeight())};

       	plane.set(res.x * .1, res.y * .1, resDepth.x, resDepth.y);
       	plane.setPosition({0,0,0});
    	plane.mapTexCoords(0, 0, 1., 1.);

		resDepthReMapped = {resDepth.x / res.x, resDepth.y / res.y};
		maxDisparity = stereo->initialConfig.getMaxDisparity();

	}


	void SemanticSegmentationDepth::update()
	{
		std::shared_ptr<dai::NNData> inNN = qNN->tryGet<dai::NNData>();
		if(inNN)
		{
			NNTexture->updateNNBuffer(inNN, nnSize);
			NNTexture->updateTexture();
		}

		std::unordered_map<std::string, std::shared_ptr<dai::ImgFrame>> latestPacket;
		auto queueEvents = device->getQueueEvents(queueNames);
		for(const auto& name : queueEvents) {
			auto packets = device->getOutputQueue(name)->tryGetAll<dai::ImgFrame>();
			auto count = packets.size();
			if(count > 0) {
				latestPacket[name] = packets[count - 1];
			}
		}

		for(const auto& name : queueNames) {
			if(latestPacket.find(name) != latestPacket.end()) {
				if(name == "depth") {
					frame[name] = latestPacket[name]->getFrame();
					frame[name].convertTo(frame[name], CV_8UC1, 255. / maxDisparity);
					if(textures[name]==nullptr){
						textures[name] = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
					}

					// keep this 3 channels for Raspberry pi use.
					if(frame[name].channels() < 3) {
						cv::cvtColor(frame[name], frame[name], cv::COLOR_GRAY2RGB);
					}
					textures[name]->updateBuffer(&frame[name], latestPacket[name]->getWidth(), latestPacket[name]->getHeight());
				} else {
					frame[name] = latestPacket[name]->getCvFrame();
					if(textures[name]==nullptr){
						textures[name] = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
					}
					textures[name]->updateBuffer(&frame[name], latestPacket[name]->getWidth(), latestPacket[name]->getHeight());
				}
			}
		}
		for(const auto& name : queueNames) {
			if(textures[name] != nullptr)textures[name]->updateTexture();
		}
	}

	void SemanticSegmentationDepth::draw()
	{

		semanticDepthShader.begin();
		
		if(arbTex)
		{
			semanticDepthShader.setUniform2f("u_nnSize", glm::vec2(nnSize));
			semanticDepthShader.setUniform2f("u_previewSize", previewSize);	
			semanticDepthShader.setUniform2f("u_resDepth", resDepth);			
		}else{
			semanticDepthShader.setUniform2f("u_nnSize", glm::vec2(1.0));
			semanticDepthShader.setUniform2f("u_previewSize", glm::vec2(1.0));
			semanticDepthShader.setUniform2f("u_resDepth", glm::vec2(1.0));			

		}

        
		if(NNTexture != nullptr)semanticDepthShader.setUniformTexture("mask", NNTexture->tex, 0);
		semanticDepthShader.setUniform2f("u_cropResize", cropResize);

		int indexTex = 1;
		semanticDepthShader.setUniform1f("u_disparity", maxDisparity );
		for(const auto& name : queueNames) {
			if(textures[name] != nullptr){
				semanticDepthShader.setUniformTexture(name, textures[name]->tex, indexTex++);
			}
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