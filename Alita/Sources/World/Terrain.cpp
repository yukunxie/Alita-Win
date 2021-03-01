#include "Terrain.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"

NS_RX_BEGIN

Terrain::Terrain()
{
    //AddComponment(MeshComponentBuilder::CreateBox());
}

Terrain::~Terrain()
{
}

Terrain* Terrain::CreateFromHeightMap(const std::string& imgFilename, float minHeight, float maxHeight, float unit)
{
    int texWidth, texHeight, texChannels;
    const TData& imageData = FileSystem::GetInstance()->GetBinaryData(imgFilename.c_str());
    stbi_uc* pixels = stbi_load_from_memory(imageData.data(), imageData.size(), &texWidth,
        &texHeight, &texChannels, STBI_grey);

    std::vector<TVector3> vertices(texWidth * texHeight);
    
    for (int idx = 0; idx < vertices.size(); ++idx)
    {
        int ix = idx % texHeight;
        int iz = idx / texHeight;
        
        vertices[idx].x = (ix - texWidth / 2) * unit;
        vertices[idx].z = (iz - texHeight / 2) * unit;
        vertices[idx].y = pixels[idx] / 255.0f * (maxHeight - minHeight) - minHeight;
    }

    /*for (int i = 0; i < texWidth; ++i)
    {
        for (int j = 0; j < texHeight; ++j)
        {

        }
    }*/

    return nullptr;
}

void Terrain::Tick(float dt)
{
    /*auto mesh = GetComponent<MeshComponent>();
    Engine::GetEngine()->GetRenderScene()->AddPrimitive(mesh);*/
}

NS_RX_END