//
// Created by realxie on 2019-10-29.
//

#include "Material.h"
#include "Engine/Engine.h"

#include "RHI.h"
#include "Backend/Vulkan/ShaderHelper.h"

NS_RX_BEGIN

RHI::ShaderModule* CreateShader(const std::string& filename, RHI::ShaderType shaderType)
{
		std::string shaderText = FileSystem::GetInstance()->GetStringData(filename.c_str());

		auto spirV = RHI::CompileGLSLToSPIRV(shaderText, shaderType);
		RHI::ShaderModuleDescriptor descriptor;
		descriptor.binaryCode = std::move(spirV);
		descriptor.codeType = RHI::ShaderCodeType::BINARY;
		return Engine::GetGPUDevice()->CreateShaderModule(descriptor);
}

Material::Material(const std::string& configFilename)
{
	rapidjson::Document doc;
	{
		std::string text = FileSystem::GetInstance()->GetStringData("Shaders/CommonMaterial.json");
		doc.Parse(text.c_str()); 
	}

	CreateBindGroupLayout(doc);

	std::string vsFilename = doc["code"]["vs"].GetString();
	std::string fsFilename = doc["code"]["fs"].GetString();

	RHI::ShaderModule* vertShader = CreateShader(vsFilename, RHI::ShaderType::VERTEX);
	RHI::ShaderModule* fragShader = CreateShader(fsFilename, RHI::ShaderType::FRAGMENT);
}

RHI::BindGroupLayout* Material::CreateBindGroupLayout(const rapidjson::Document& doc)
{
	if (!doc.HasMember("bindings"))
	{
		return nullptr;
	}
	Assert(doc["bindings"].IsArray());

	for (auto& cfg : doc["bindings"].GetArray())
	{
		std::string name = cfg["name"].GetString();
		std::uint32_t binding = cfg["binding"].GetUint();
		std::string format = cfg["format"].GetString();

		std::vector<MaterialBufferParameter> fields;

		if (cfg.HasMember("fields") && cfg["fields"].IsArray())
		{
			for (const auto& f : cfg["fields"].GetArray())
			{
				MaterialBufferParameter param;
				param.name = f["name"].GetString();
				param.offset = f["offset"].GetUint();
				std::string format = f["format"].GetString();
				if (format == "mat4")
					param.format = MaterialParameterType::MAT4;
				else
					Assert(false);

				fields.push_back(param);
			}
		}

		if (fields.size() > 0)
		{

		}
	}
	
	return nullptr;
}

NS_RX_END