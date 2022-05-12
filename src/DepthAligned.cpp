#include "DepthAligned.h"


#define STRINGIFY(A) #A

namespace ofxDepthAICore{
	
	DepthAligned::DepthAligned()
	{

	}

	DepthAligned::~DepthAligned()
	{

        waitForThread(false);
	}


	void DepthAligned::setup()
	{
		pipeline.setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);
		
		auto rgbOut = pipeline.create<dai::node::XLinkOut>();
    	auto depthOut = pipeline.create<dai::node::XLinkOut>();

		camRgb = pipeline.create<dai::node::ColorCamera>();
		left = pipeline.create<dai::node::MonoCamera>();
		right = pipeline.create<dai::node::MonoCamera>();
		stereo = pipeline.create<dai::node::StereoDepth>();


		rgbOut->setStreamName("rgb");
    	queueNames.push_back("rgb");
		depthOut->setStreamName("depth");
    	queueNames.push_back("depth");

		camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
		camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
		camRgb->setFps(30);


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

		auto config = stereo->initialConfig.get();
		config.postProcessing.speckleFilter.enable = false;
		config.postProcessing.speckleFilter.speckleRange = 50;
		config.postProcessing.temporalFilter.enable = true;
		config.postProcessing.spatialFilter.enable = true;
		config.postProcessing.spatialFilter.holeFillingRadius = 2;
		config.postProcessing.spatialFilter.numIterations = 4;
		config.postProcessing.thresholdFilter.minRange = 100;
		config.postProcessing.thresholdFilter.maxRange = 3000;
		config.postProcessing.decimationFilter.decimationFactor = 1;
		stereo->initialConfig.set(config);

		// Linking
		camRgb->isp.link(rgbOut->input);
		left->out.link(stereo->left);
		right->out.link(stereo->right);
		stereo->disparity.link(depthOut->input);
		
		device = new dai::Device(pipeline);

		// Sets queues size and behavior
		for(const auto& name : queueNames) {
			device->getOutputQueue(name, 4, false);
		}


		loadShader();
		resDepth = {float(left->getResolutionWidth()), float(left->getResolutionHeight())};

		glm::vec2 res = {float(camRgb->getVideoWidth()), float(camRgb->getVideoHeight())};
       	plane.set(res.x * .1, res.y * .1, resDepth.x, resDepth.y);
       	plane.setPosition({0,0,0});
    	plane.mapTexCoords(0, 0, res.x, res.y);


		resDepthReMapped = {resDepth.x / res.x, resDepth.y / res.y};
		
		maxDisparity = stereo->initialConfig.getMaxDisparity();

		startThread();

	}


	void DepthAligned::update()
	{
		std::unique_lock<std::mutex> lock(mutex);
		for(const auto& name : queueNames) {
			if(textures[name] != nullptr)textures[name]->updateTexture();
		}


		// if(textures["depth"] != nullptr){
		// 	mesh.clear();
		// 	mesh.setMode(OF_PRIMITIVE_POINTS);
			
		// 	for( int x = 0; x < resDepth.x; x++){
		// 		for( int y = 0; y < resDepth.y; y++){
					


		// 			glm::vec3 p = 
		// 		}
		// 	}
		// }


		// if(n!=0){
		// 	const rs2::vertex * vs = points.get_vertices();
		// 	for(int i=0; i<n; i++){
		// 		if(vs[i].z){
		// 			const rs2::vertex v = vs[i];
		// 			glm::vec3 v3(v.x,v.y,v.z);
		// 			mesh.addVertex(v3);
					
		// 			float red   = (float)(textureImg.getPixels()[i * 3]);
		// 			float green = (float)(textureImg.getPixels()[(i * 3) + 1]);
		// 			float blue  = (float)(textureImg.getPixels()[(i * 3) + 2]);
		// 			ofColor c = ofColor({red,green,blue});
					
		// 			mesh.addColor(c);
		// 		}
		// 	}
		// }

	    condition.notify_all();
	}


	void DepthAligned::draw()
	{   
		depthShader.begin();
        
		int indexTex = 0;
		depthShader.setUniform1f("u_disparity", maxDisparity );
		for(const auto& name : queueNames) {
			if(textures[name] != nullptr)depthShader.setUniformTexture(name, textures[name]->tex, indexTex++);
		}

		plane.draw();
		
		depthShader.end();
	}

	void DepthAligned::threadedFunction(){
        while(isThreadRunning()){
			std::unique_lock<std::mutex> lock(mutex);

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
						// auto maxDisparity = stereo->initialConfig.getMaxDisparity();
						// // Optional, extend range 0..95 -> 0..255, for a better visualisation
						if(1) frame[name].convertTo(frame[name], CV_8UC1, 255. / maxDisparity);
						// // Optional, apply false colorization
						//if(1) cv::applyColorMap(frame[name], frame[name], cv::COLORMAP_HOT);
						if(textures[name]==nullptr){
							textures[name] = std::make_unique<ofxDepthAICore::DepthAITexConverter>();
							// textures[name]->channels =1; 
						}

            			if(frame[name].channels() < 3) {
                			cv::cvtColor(frame[name], frame[name], cv::COLOR_GRAY2BGR);
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
		

            condition.wait(lock);
		}
	}

	void DepthAligned::loadShader()
	{
		if(ofIsGLProgrammableRenderer()){
			if(depthShader.load("shadersGL3/depthShader"))std::cout << "loaded OK" << std::endl;
		}else{
			std::cout << "not working" << std::endl;
		}
	}
    

	std::string DepthAligned::loadVertShader()
	{
		return STRINGIFY(

							uniform mat4 modelViewMatrix;
							uniform mat4 modelViewProjectionMatrix;


							uniform sampler2DRect depth;


							in vec4  position;
							in vec2 texcoord;
							
							varying vec2 texCoordVarying;

							void main() {
								

    							texCoordVarying = vec2(texcoord.x, texcoord.y);
								float z = texture2DRect(depth, texcoord).z;
								gl_Position    = modelViewProjectionMatrix * vec4(position.xy, z, position.w);	
								
								
							}
							);


	}

	std::string DepthAligned::loadFragShader()
	{
		return STRINGIFY(

							uniform sampler2DRect rgb;

							varying vec2 texCoordVarying;
							

							void main (void){
								
								gl_FragColor = texture2DRect(rgb, texCoordVarying);
							}
							);
	}
}