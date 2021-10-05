#include "ImageLoader.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"

#include "Backend/Vulkan/VKCommandEncoder.h"
#include "Backend/Vulkan/VKCommandBuffer.h"
#include "Backend/Vulkan/VKTexture.h"


#include "ktx.h"

NS_RX_BEGIN

uint32 GetMipMapLevels(uint32 n)
{
    uint32 level = 0;
    for (; n != 0; level++, n = n >> 1);
    return level;
}

std::vector<std::vector<uint8>> GenerateMipMap(uint32 width, uint32 height, const uint8* pixels, uint32 channelNum)
{
    RHI_ASSERT(width == height && width > 1);
    // Pow of two
    RHI_ASSERT((width & (width - 1)) == 0);

    std::vector<std::vector<uint8>> mipmaps;

    uint32 pixelSize = channelNum;
    std::vector<uint8> mipmap0(width * height * pixelSize);
    memcpy(mipmap0.data(), pixels, width * height * pixelSize);
    mipmaps.push_back(std::move(mipmap0));

    for (width = width >> 1, height = height >> 1; width > 0 && height > 0; width = width >> 1, height = height >> 1)
    {
        uint32 lwh = width << 1;
        uint32 lBytesPerRow = lwh * pixelSize;
        uint32 bytesPerRow = width * pixelSize;

        std::vector<uint8> mipmap(width * height * pixelSize);
        //auto mipmap = std::make_shared<uint8>(new uint8[width * height * pixelSize]);
        const uint8* pData = mipmaps.back().data();

        for (uint32 h = 0; h < height; h++)
        {
            for (uint32 w = 0; w < width; w++)
            {
                uint32 rgba[4] = { 0, 0, 0, 0 };
                const uint8* ph0 = pData + (2 * h * lBytesPerRow + 2 * w * pixelSize);
                const uint8* ph1 = pData + ((2 * h + 1) * lBytesPerRow + 2 *  w * pixelSize);
                for (uint32 c = 0; c < pixelSize; ++c)
                {
                    rgba[c] += ph0[c];
                    rgba[c] += ph0[c + pixelSize];
                    rgba[c] += ph1[c + pixelSize];
                    rgba[c] += ph1[c + pixelSize];

                    // average
                    rgba[c] /= 4;

                    uint8* buffer = mipmap.data();
                    buffer[h * bytesPerRow + w * pixelSize + c] = rgba[c];
                }
            }
        }
        mipmaps.push_back(std::move(mipmap));
    }
    return std::move(mipmaps);
}

namespace ImageLoader
{
    static const std::vector<std::string> sImageExts = { ".jpg", ".png", ".tag", ".bmp", };

    struct ImageData
    {
        std::string debugName;
        RHI::TextureFormat format;
        uint32 baseWidth;
        uint32 baseHeight;
        uint32 numLevels;
        uint32 numFaces;
        uint32 byteLength;
        const uint8* bytes;
        // layer * 1000 * 1000 + face * 1000 + mipmaps
        std::unordered_map<uint32, uint32> offsets;

        uint32 GetOffset(uint32 layer, uint32 face, uint32 level) const
        {
            uint32 key = layer * 1000 * 1000 + face * 1000 + level;
            return offsets.find(key)->second;
        }

        void SetOffset(uint32 layer, uint32 face, uint32 level, uint32 offset)
        {
            uint32 key = layer * 1000 * 1000 + face * 1000 + level;
            offsets[key] = offset;
        }
    };

    RHI::Texture* CreateCubeTexture(const ImageData& imageData)
    {
        RHI_ASSERT(imageData.format == RHI::TextureFormat::RGBA8UNORM);

        RHI::TextureDescriptor descriptor;
        {
            descriptor.sampleCount = 1;
            descriptor.format = RHI::TextureFormat::RGBA8UNORM;
            descriptor.usage = RHI::TextureUsage::SAMPLED | RHI::TextureUsage::COPY_DST;
            descriptor.size = { imageData.baseWidth, imageData.baseHeight, imageData.numFaces };
            descriptor.arrayLayerCount = 1;
            descriptor.mipLevelCount = imageData.numLevels;
            descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
            descriptor.debugName = imageData.debugName;
        };
        auto texture = Engine::GetGPUDevice()->CreateTexture(descriptor);

        RHI::BufferDescriptor bufferDescriptor;
        {
            bufferDescriptor.size = imageData.byteLength;
            bufferDescriptor.usage = RHI::BufferUsage::COPY_DST | RHI::BufferUsage::COPY_SRC;
        }

        auto buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);
        buffer->SetSubData(0, imageData.byteLength, imageData.bytes);

        auto commandEncoder = Engine::GetGPUDevice()->CreateCommandEncoder();

        for (uint32 face = 0; face < imageData.numFaces; ++face)
        {
            for (uint32_t level = 0; level < descriptor.mipLevelCount; level++)
            {
                // Calculate offset into staging buffer for the current mip level and face
                uint32 offset = imageData.GetOffset(0, face, level);

                RHI::BufferCopyView bufferCopyView;
                {
                    bufferCopyView.buffer = buffer;
                    bufferCopyView.offset = offset;
                    bufferCopyView.rowsPerImage = imageData.baseHeight >> level;
                    bufferCopyView.bytesPerRow = (imageData.baseWidth >> level) * 4;
                }

                RHI::TextureCopyView textureCopyView;
                {
                    textureCopyView.texture = texture;
                    textureCopyView.origin = { 0, 0, (std::int32_t)face };
                    textureCopyView.mipLevel = level;
                }

                auto size = RHI::Extent3D{ imageData.baseHeight >> level, imageData.baseWidth >> level, 1 };
                commandEncoder->CopyBufferToTexture(bufferCopyView, textureCopyView, size);
            }
        }

        auto cmdBuffer = commandEncoder->Finish();
        RHI_SAFE_RETAIN(cmdBuffer);
        Engine::GetGPUDevice()->GetQueue()->Submit(1, &cmdBuffer);
        RHI_SAFE_RELEASE(cmdBuffer);

        return texture;
    }

    RHI::Texture* LoadTextureFromData(uint32 texWidth, uint32 texHeight, uint32 texChannels, const std::uint8_t* pixels, std::uint32_t byteLength, const std::string& debugName)
    {
        ImageData imageData;
        {
            imageData.debugName = debugName;
            imageData.baseWidth = texWidth;
            imageData.baseHeight = texHeight;
            imageData.byteLength = texWidth * texHeight * 4;
            imageData.format = RHI::TextureFormat::RGBA8UNORM;
            imageData.numFaces = 1;
            imageData.numLevels = GetMipMapLevels(texWidth);
        }
        //imageData.SetOffset(0, 0, 0, 0);
        std::vector<uint8> bytes;

        std::vector<uint8> tmpData;
        const uint8* pData = nullptr;

        if (texChannels != 4)
        {
            tmpData.resize(texWidth * texHeight * 4, 255);
            uint8* pCurrent = tmpData.data();
            for (int i = 0; i < texWidth * texHeight * texChannels; i += texChannels)
            {
                for (int j = 0; j < texChannels; ++j)
                {
                    pCurrent[j] = pixels[i + j];
                }
                pCurrent += 4;
            }
            pData = tmpData.data();
        }
        else
        {
            pData = pixels;
        }

        std::vector<uint8> buffer;
        buffer.reserve(texWidth * texHeight * texChannels * 2);
        texChannels = 4;
        auto mipmaps = GenerateMipMap(texWidth, texHeight, pData, texChannels);
        for (uint32 level = 0; level < mipmaps.size(); ++level)
        {
            auto w = texWidth >> level;
            auto h = texHeight >> level;
            auto size = w * h * texChannels;
            auto offset = buffer.size();
            buffer.resize(buffer.size() + size);
            memcpy(buffer.data() + offset, mipmaps[level].data(), size);
            imageData.SetOffset(0, 0, level, offset);
        }

        imageData.bytes = buffer.data();
        imageData.byteLength = buffer.size();

        auto texture = CreateCubeTexture(imageData);

        return texture;
    }

    RHI::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& fileName)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load_from_memory(data, byteLength, &texWidth, &texHeight, &texChannels, STBI_default);
        RHI_ASSERT(pixels != nullptr);

        auto texture = LoadTextureFromData(texWidth, texHeight, texChannels, pixels, texWidth * texHeight * texChannels, fileName);
        STBI_FREE(pixels);

        return texture;
    }

    RHI::Texture* LoadTextureFromKtxFormat(const void* bytes, uint32 size, const std::string& fileName = "")
    {
        ktxTexture* ktxTexture;
        KTX_error_code result = ktxTexture_CreateFromMemory((ktx_uint8_t*)bytes, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
        RHI_ASSERT(result == KTX_SUCCESS);

        RHI::Texture* texture;

        RHI_ASSERT(ktxTexture->glFormat == 6408);
        ImageData imageData;
        {
            imageData.debugName = fileName;
            imageData.format = RHI::TextureFormat::RGBA8UNORM;
            imageData.baseWidth = ktxTexture->baseWidth;
            imageData.baseHeight = ktxTexture->baseHeight;
            imageData.numLevels = ktxTexture->numLevels;
            imageData.numFaces = ktxTexture->numFaces;
            imageData.bytes = ktxTexture_GetData(ktxTexture);
            imageData.byteLength = ktxTexture_GetSize(ktxTexture);
        }

        for (uint32 face = 0; face < imageData.numFaces; ++face)
        {
            for (uint32_t level = 0; level < ktxTexture->numLevels; level++)
            {
                // Calculate offset into staging buffer for the current mip level and face
                ktx_size_t offset;
                KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
                imageData.SetOffset(0, face, level, offset);
            }
        }
        texture = CreateCubeTexture(imageData);

        if (ktxTexture)
        {
            ktxTexture_Destroy(ktxTexture);
        }

        return texture;
    }

    RHI::Texture* LoadTextureFromUri(const std::string& filename)
    {
        const TData& imageData = FileSystem::GetInstance()->GetBinaryData(filename.c_str());
        
        if (filename.substr(filename.find_last_of(".") + 1) == "ktx") {
            return LoadTextureFromKtxFormat(imageData.data(), imageData.size(), filename);
        }
        else {
            return LoadTextureFromData(imageData.data(), imageData.size(), filename);
        }
    }

    RHI::Texture* LoadCubeTexture(const std::string& cubeTextureName)
    {
        constexpr uint32 kFaceNum = 6;
        const char* kFaceNames[kFaceNum] = {"px", "nx", "py", "ny", "pz", "nz"};
        std::string fileExt = "";
        for (const auto& ext : sImageExts)
        {
            std::string filename = "Textures\\CubeTextures\\" + cubeTextureName + "\\" + "nx" + ext;
            if (!FileSystem::GetInstance()->GetAbsFilePath(filename.c_str()).empty())
            {
                fileExt = ext;
                break;
            }
        }

        RHI_ASSERT(!fileExt.empty());

        ImageData imageData;
        {
            imageData.debugName = cubeTextureName;
            imageData.format = RHI::TextureFormat::RGBA8UNORM;
            imageData.numFaces = kFaceNum;
            imageData.numLevels = 1;
        }
        std::vector<uint8> bytes;

        for (int face = 0; face < kFaceNum; ++face)
        {
            std::string filename = "Textures\\CubeTextures\\" + cubeTextureName + "\\" + kFaceNames[face] + fileExt;
            const TData& data = FileSystem::GetInstance()->GetBinaryData(filename.c_str());

            int texWidth, texHeight, texChannels;
            std::vector<uint8> tmpData;
            const uint8* pData = nullptr;
            stbi_uc* pixels = stbi_load_from_memory(data.data(), data.size(), &texWidth, &texHeight, &texChannels, STBI_default);
            pData = pixels;
            if (texChannels != 4)
            {
                tmpData.resize(texWidth * texHeight * 4, 255);
                pData = tmpData.data();
                uint8* pCurrent = tmpData.data();
                for (int i = 0; i < texWidth * texHeight * texChannels; i += texChannels)
                {
                    for (int j = 0; j < texChannels; ++j)
                    {
                        pCurrent[j] = pixels[i + j];
                    }
                    pCurrent += 4;
                }
            }
            /*uint32 offset = bytes.size();
            imageData.SetOffset(0, face, 0, offset);*/
            uint32 length = texWidth * texHeight * 4;
            if (face == 0)
            {
                imageData.baseWidth = texWidth;
                imageData.baseHeight = texHeight;
                //imageData.byteLength = texWidth * texHeight * 4 * kFaceNum;
                bytes.reserve(texWidth * texHeight * 4 * kFaceNum * 2);
            }
            RHI_ASSERT(imageData.baseWidth == texWidth);
            RHI_ASSERT(imageData.baseHeight == texHeight);

            auto mipmaps = GenerateMipMap(texWidth, texHeight, pData, texChannels);
            for (uint32 level = 0; level < mipmaps.size(); ++level)
            {
                auto w = texWidth >> level;
                auto h = texHeight >> level;
                auto size = w * h * texChannels;
                auto offset = bytes.size();
                bytes.resize(bytes.size() + size);
                memcpy(bytes.data() + offset, mipmaps[level].data(), size);
                imageData.SetOffset(0, face, level, offset);
            }


            //bytes.resize(offset + length);
            //memcpy(bytes.data() + offset, pData, length);

            STBI_FREE(pixels);
        }
        imageData.numLevels = GetMipMapLevels(imageData.baseWidth);
        imageData.bytes = bytes.data();
        imageData.byteLength = bytes.size();

        auto texture = CreateCubeTexture(imageData);
        return texture;
    }
}

NS_RX_END