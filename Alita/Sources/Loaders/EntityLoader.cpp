#include "EntityLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

NS_RX_BEGIN

namespace EntityLoader
{

	Model* LoadModelFromGLTF(const std::string& gltfFilename)
	{
		//using namespace tinygltf;
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		std::string modelFile = FileSystem::GetInstance()->GetAbsFilePath("Models/cube.gltf");

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, modelFile);
		if (!ret) {
			return nullptr;
		}
	}


}

NS_RX_END