#include "Terrain.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"
#include "World/MeshComponent.h"
#include "Graphics/RenderScene.h"

#include <math.h>

NS_RX_BEGIN

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

Terrain* Terrain::CreateFromHeightMap(const std::string& imgFilename, float minHeight, float maxHeight, float unit, const TVector2& texCoordScale)
{
    int texWidth, texHeight, texChannels;
    const TData& imageData = FileSystem::GetInstance()->GetBinaryData(imgFilename.c_str());
    stbi_uc* pixels = stbi_load_from_memory(imageData.data(), imageData.size(), &texWidth,
        &texHeight, &texChannels, STBI_grey);

    std::vector<TVector3> vertices(texWidth * texHeight);
    std::vector<TVector3> diffuse(texWidth * texHeight);
    std::vector<TVector2> texCoords(texWidth * texHeight);
    std::vector<std::uint32_t> indices((texWidth - 1) * (texHeight - 1) * 6);
    
    for (int idx = 0; idx < vertices.size(); ++idx)
    {
        int ix = idx % texHeight;
        int iz = idx / texHeight;
        
        vertices[idx].x = (ix - texWidth / 2) * unit;
        vertices[idx].z = (iz - texHeight / 2) * unit;
        vertices[idx].y = pixels[idx] / 255.0f * (maxHeight - minHeight) + minHeight;
        
        //diffuse[idx] = { 1.0f, 0.0f, 1.0f };

        texCoords[idx] = { fmod(ix * texCoordScale.x / texWidth, 1.0f), fmod(iz * texCoordScale.y / texHeight, 1.0f) };
    }

    for (int z = 0; z < texHeight - 1; ++z)
    {
        for (int x = 0; x < texWidth - 1; ++x)
        {
            int start = (z * (texWidth - 1) + x) * 6;

            int a = z * texWidth + x;
            int b = a + 1;
            int c = (z + 1) * texWidth + x;
            int d = c + 1;

            indices[start + 0] = a;
            indices[start + 1] = c;
            indices[start + 2] = b;
            indices[start + 3] = c;
            indices[start + 4] = d;
            indices[start + 5] = b;

            {
                auto d1 = vertices[c] - vertices[a];
                auto d2 = vertices[b] - vertices[a];
                auto n = glm::normalize(glm::cross(d1, d2));
                diffuse[a] = diffuse[b] = diffuse[c] = n;
            }

            {
                auto d1 = vertices[d] - vertices[c];
                auto d2 = vertices[b] - vertices[c];
                auto n = glm::normalize(glm::cross(d1, d2));
                diffuse[d] = n;
            }
        }
    }

    MeshComponent* meshComp = new MeshComponent();
    meshComp->Geometry_ = new Geometry;
    meshComp->Material_ = new Material("Materials/PBR.json");

    {
        auto vbBuffer = new VertexBuffer();
        vbBuffer->kind = VertexBufferAttriKind::POSITION;
        vbBuffer->format = InputAttributeFormat::FLOAT3;
        vbBuffer->InitData(vertices.data(), vertices.size() * sizeof(vertices[0]));
        meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
    }

    {
        auto vbBuffer = new VertexBuffer();
        vbBuffer->kind = VertexBufferAttriKind::NORMAL;
        vbBuffer->format = InputAttributeFormat::FLOAT3;
        vbBuffer->InitData(diffuse.data(), diffuse.size() * sizeof(diffuse[0]));
        meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
    }

    {
        auto vbBuffer = new VertexBuffer();
        vbBuffer->kind = VertexBufferAttriKind::TEXCOORD;
        vbBuffer->format = InputAttributeFormat::FLOAT2;
        vbBuffer->InitData(texCoords.data(), texCoords.size() * sizeof(texCoords[0]));
        meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
    }

    meshComp->Geometry_->indexBuffer_.indexType = IndexType::UINT32;
    meshComp->Geometry_->indexBuffer_.InitData(indices.data(), indices.size() * sizeof(indices[0]));

    auto terrain = new Terrain();
    terrain->AddComponment(meshComp);
    return terrain;
}


NS_RX_END