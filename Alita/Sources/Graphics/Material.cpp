//
// Created by realxie on 2019-10-29.
//

#include "Material.h"

#include "rapidjson/document.h"

NS_RX_BEGIN

Material::Material(const std::string& configFilename)
{
	rapidjson::Document doc;
	{
		std::string text = FileSystem::GetInstance()->GetStringData("Shaders/CommonMaterial.json");
		doc.Parse(text.c_str()); 
	}

	std::string vsFilename = doc["code"]["vs"].GetString();
	std::string fsFilename = doc["code"]["fs"].GetString();
}

NS_RX_END