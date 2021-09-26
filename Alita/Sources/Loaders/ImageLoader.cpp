#include "ImageLoader.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"

#include "Backend/Vulkan/VKCommandEncoder.h"
#include "Backend/Vulkan/VKCommandBuffer.h"
#include "Backend/Vulkan/VKTexture.h"


#include "ktx.h"

NS_RX_BEGIN

namespace ImageLoader
{

    RHI::Texture* LoadTextureFromData(uint32 width, uint32 height, uint32 component, const std::uint8_t* data, std::uint32_t byteLength)
    {
        RHI::Format format = RHI::Format::UNDEFINED;
        switch (component)
        {
        case 1:
            format = RHI::Format::R8_UNORM;
            break;
        case 2:
            format = RHI::Format::R8G8_UNORM;
            break;
        case 3:
            format = RHI::Format::R8G8B8_UNORM;
            break;
        case 4:
            format = RHI::Format::R8G8B8A8_UNORM;
            break;
        }
        Assert(format != RHI::Format::UNDEFINED, "invalid component");

        //RHI::ImageCreateInfo imageCreateInfo{
        //    .imageType = RHI::ImageType::IMAGE_TYPE_2D,
        //    .format = format,
        //    .extent = {width, height, 1},
        //    .mipLevels = 1,
        //    .arrayLayers = 1,
        //    .samples = RHI::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
        //    .tiling = RHI::ImageTiling::LINEAR,
        //    .sharingMode = RHI::SharingMode::EXCLUSIVE,
        //    .imageData = nullptr,
        //};


        RHI::TextureDescriptor descriptor;
        {
            descriptor.sampleCount = 1;
            descriptor.format = RHI::TextureFormat::RGBA8UNORM;
            descriptor.usage = RHI::TextureUsage::SAMPLED | RHI::TextureUsage::COPY_DST;
            descriptor.size = { width, height, 1 };
            descriptor.arrayLayerCount = 1;
            descriptor.mipLevelCount = 1;
            descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
        };
        auto texture = Engine::GetGPUDevice()->CreateTexture(descriptor);

        RHI::BufferDescriptor bufferDescriptor;
        {
            bufferDescriptor.size = (std::uint32_t)width * height * component;
            bufferDescriptor.usage = RHI::BufferUsage::COPY_DST | RHI::BufferUsage::COPY_SRC;
        }

        auto buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);
        buffer->SetSubData(0, width * height * component, data);

        RHI::BufferCopyView bufferCopyView;
        {
            bufferCopyView.buffer = buffer;
            bufferCopyView.offset = 0;
            bufferCopyView.rowsPerImage = height;
            bufferCopyView.bytesPerRow = width * component;
        }

        RHI::TextureCopyView textureCopyView;
        {
            textureCopyView.texture = texture;
            textureCopyView.origin = { 0, 0, 0 };
            textureCopyView.mipLevel = 0;
        }

        auto commandEncoder = Engine::GetGPUDevice()->CreateCommandEncoder();
        auto size = RHI::Extent3D{ width, height, 1 };
        commandEncoder->CopyBufferToTexture(bufferCopyView, textureCopyView, size);

        auto cmdBuffer = commandEncoder->Finish();
        RHI_SAFE_RETAIN(cmdBuffer);
        Engine::GetGPUDevice()->GetQueue()->Submit(1, &cmdBuffer);
        RHI_SAFE_RELEASE(cmdBuffer);

        return texture;
    }

    RHI::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength)
    {
        int texWidth, texHeight, texChannels;

        stbi_uc* pixels = stbi_load_from_memory(data, byteLength, &texWidth,
            &texHeight, &texChannels, STBI_rgb_alpha);

        return LoadTextureFromData(texWidth, texHeight, 4, pixels, texWidth * texHeight * 4);
    }

    RHI::Texture* CreateCubeTexture(ktxTexture* ktxTexture)
    {
        RHI_ASSERT(ktxTexture->glFormat == 6408);

        //RHI::ImageCreateInfo imageCreateInfo{
        //    .imageType = RHI::ImageType::IMAGE_TYPE_2D,
        //    .format = RHI::Format::R8G8B8A8_UNORM,
        //    .extent = {ktxTexture->baseWidth, ktxTexture->baseHeight, 1},
        //    .mipLevels = 1,
        //    .arrayLayers = 1,
        //    .samples = RHI::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
        //    .tiling = RHI::ImageTiling::LINEAR,
        //    .sharingMode = RHI::SharingMode::EXCLUSIVE,
        //    .imageData = nullptr,
        //};

        RHI::TextureDescriptor descriptor;
        {
            descriptor.sampleCount = 1;
            descriptor.format = RHI::TextureFormat::RGBA8UNORM;
            descriptor.usage = RHI::TextureUsage::SAMPLED | RHI::TextureUsage::COPY_DST;
            descriptor.size = { ktxTexture->baseWidth, ktxTexture->baseHeight, ktxTexture->numFaces };
            descriptor.arrayLayerCount = 1;
            descriptor.mipLevelCount = ktxTexture->numLevels;
            descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
        };
        auto texture = Engine::GetGPUDevice()->CreateTexture(descriptor);

        ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);
        ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

        RHI::BufferDescriptor bufferDescriptor;
        {
            bufferDescriptor.size = ktxTextureSize;
            bufferDescriptor.usage = RHI::BufferUsage::COPY_DST | RHI::BufferUsage::COPY_SRC;
        }

        auto buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);
        buffer->SetSubData(0, ktxTextureSize, ktxTextureData);

        auto commandEncoder = Engine::GetGPUDevice()->CreateCommandEncoder();

        for (uint32 face = 0; face < 6; ++face)
        {
            for (uint32_t level = 0; level < descriptor.mipLevelCount; level++)
            {
                // Calculate offset into staging buffer for the current mip level and face
                ktx_size_t offset;
                KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
                //assert(ret == KTX_SUCCESS);
                //VkBufferImageCopy bufferCopyRegion = {};
                //bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                //bufferCopyRegion.imageSubresource.mipLevel = level;
                //bufferCopyRegion.imageSubresource.baseArrayLayer = face;
                //bufferCopyRegion.imageSubresource.layerCount = 1;
                //bufferCopyRegion.imageExtent.width = ktxTexture->baseWidth >> level;
                //bufferCopyRegion.imageExtent.height = ktxTexture->baseHeight >> level;
                //bufferCopyRegion.imageExtent.depth = 1;
                //bufferCopyRegion.bufferOffset = offset;
                ////bufferCopyRegions.push_back(bufferCopyRegion);

                RHI::BufferCopyView bufferCopyView;
                {
                    bufferCopyView.buffer = buffer;
                    bufferCopyView.offset = offset;
                    bufferCopyView.rowsPerImage = ktxTexture->baseHeight >> level;
                    bufferCopyView.bytesPerRow = (ktxTexture->baseWidth >> level) * 4;
                }

                RHI::TextureCopyView textureCopyView;
                {
                    textureCopyView.texture = texture;
                    textureCopyView.origin = { 0, 0, (std::int32_t)face };
                    textureCopyView.mipLevel = level;
                }

                auto size = RHI::Extent3D{ ktxTexture->baseHeight >> level, ktxTexture->baseWidth >> level, 1 };
                commandEncoder->CopyBufferToTexture(bufferCopyView, textureCopyView, size);
            }
        }

        auto cmdBuffer = commandEncoder->Finish();
        RHI_SAFE_RETAIN(cmdBuffer);
        Engine::GetGPUDevice()->GetQueue()->Submit(1, &cmdBuffer);
        RHI_SAFE_RELEASE(cmdBuffer);

        return texture;
    }

    RHI::Texture* LoadTextureFromKtxFormat(const void* bytes, uint32 size)
    {
        ktxTexture* ktxTexture;
        KTX_error_code result = ktxTexture_CreateFromMemory((ktx_uint8_t*)bytes, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
        RHI_ASSERT(result == KTX_SUCCESS);

        if (ktxTexture->numFaces == 6)
        {
            return CreateCubeTexture(ktxTexture);
        }
        else
        {
            return nullptr;
        }

        return nullptr;
    }

    RHI::Texture* LoadTextureFromUri(const std::string& filename)
    {
        const TData& imageData = FileSystem::GetInstance()->GetBinaryData(filename.c_str());
        
        if (filename.substr(filename.find_last_of(".") + 1) == "ktx") {
            return LoadTextureFromKtxFormat(imageData.data(), imageData.size());
        }
        else {
            return LoadTextureFromData(imageData.data(), imageData.size());
        }
    }
}

NS_RX_END