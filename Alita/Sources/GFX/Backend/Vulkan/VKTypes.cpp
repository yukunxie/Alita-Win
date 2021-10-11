//
// Created by realxie on 2019-10-04.
//

#include "VKTypes.h"

NS_GFX_BEGIN

const char* GetVkResultString(VkResult code)
{
    switch (code)
    {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
        default: return "Error VkResult";
    }
}

VkFrontFace ToVulkanType(const FrontFace &value)
{
    switch (value)
    {
        case FrontFace::CLOCKWISE:
            return VK_FRONT_FACE_CLOCKWISE;

        case FrontFace::COUNTER_CLOCKWISE:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

VkCullModeFlagBits ToVulkanType(const CullMode &value)
{
    switch (value)
    {
        case CullMode::FRONT_BIT:
            return VK_CULL_MODE_FRONT_BIT;
        
        case CullMode::BACK_BIT:
            return VK_CULL_MODE_BACK_BIT;
        
        case CullMode::FRONT_AND_BACK:
            return VK_CULL_MODE_FRONT_AND_BACK;
        
        default:
            return VK_CULL_MODE_NONE;
    }
    return VK_CULL_MODE_NONE;
}

VkFormat ToVulkanType(Format format)
{
    return (VkFormat) format;
}

VkSampleCountFlagBits ToVulkanType(SampleCountFlagBits bit)
{
    return (VkSampleCountFlagBits) bit;
}

VkAttachmentLoadOp ToVulkanType(AttachmentLoadOp op)
{
    return (VkAttachmentLoadOp) op;
}

VkAttachmentStoreOp ToVulkanType(AttachmentStoreOp op)
{
    return (VkAttachmentStoreOp) op;
}

VkImageLayout ToVulkanType(ImageLayout layout)
{
    return (VkImageLayout) layout;
}

VkShaderStageFlagBits ToVulkanType(ShaderStageFlagBits bits)
{
    return (VkShaderStageFlagBits) bits;
}

VkVertexInputRate ToVulkanType(VertexInputRate inputRate)
{
    return (VkVertexInputRate) inputRate;
}

VkSharingMode ToVulkanType(SharingMode sharingMode)
{
    return (VkSharingMode) sharingMode;
}

VkImageTiling ToVulkanType(ImageTiling imageTiling)
{
    return (VkImageTiling) imageTiling;
}

VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags imageUsageFlags)
{
    return (VkImageUsageFlags) imageUsageFlags;
}

VkImageType ToVulkanType(ImageType imageType)
{
    return (VkImageType) imageType;
}

VkDescriptorType ToVulkanType(BindingType type)
{
    switch (type)
    {
        case BindingType::UNIFORM_BUFFER:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case BindingType::STORAGE_BUFFER:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case BindingType::READONLY_STORAGE_BUFFER:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case BindingType::SAMPLER:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
        case BindingType::SAMPLED_TEXTURE:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case BindingType::STORAGE_TEXTURE:
            return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    }
}

VkShaderStageFlags GetVkShaderStageFlags(ShaderStageFlags flag)
{
    VkShaderStageFlags ret = 0;
    if (flag & ShaderStage::VERTEX)
    {
        ret |= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    }
    
    if (flag & ShaderStage::FRAGMENT)
    {
        ret |= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    
    if (flag & ShaderStage::COMPUTE)
    {
        ret |= VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    }
    
    if (!ret)
    {
        // TODO realxie error handle
        LOGE("invlid ShaderStageFlags");
    }
    
    return ret;
}

VkPrimitiveTopology ToVulkanType(PrimitiveTopology topology)
{
    return (VkPrimitiveTopology) topology;
}

VkVertexInputRate ToVulkanType(InputStepMode mode)
{
    switch (mode)
    {
        case InputStepMode::VERTEX :
            return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        case InputStepMode::INSTANCE:
            return VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
    }
}

std::uint32_t GetVertexFormatSize(VertexFormat format)
{
    switch (format)
    {
        case VertexFormat::UCHAR2 :
            return 2;
        case VertexFormat::UCHAR4 :
            return 4;
        case VertexFormat::CHAR2 :
            return 2;
        case VertexFormat::CHAR4 :
            return 4;
        case VertexFormat::UCHAR2NORM :
            return 2;
        case VertexFormat::UCHAR4NORM :
            return 4;
        case VertexFormat::CHAR2NORM :
            return 2;
        case VertexFormat::CHAR4NORM :
            return 4;
        case VertexFormat::USHORT2 :
            return 4;
        case VertexFormat::USHORT4 :
            return 8;
        case VertexFormat::SHORT2 :
            return 4;
        case VertexFormat::SHORT4 :
            return 8;
        case VertexFormat::USHORT2NORM :
            return 4;
        case VertexFormat::USHORT4NORM :
            return 8;
        case VertexFormat::SHORT2NORM :
            return 4;
        case VertexFormat::SHORT4NORM :
            return 8;
        case VertexFormat::HALF2 :
            return 4;
        case VertexFormat::HALF4 :
            return 8;
        case VertexFormat::FLOAT :
            return 4;
        case VertexFormat::FLOAT2 :
            return 8;
        case VertexFormat::FLOAT3 :
            return 12;
        case VertexFormat::FLOAT4 :
            return 16;
        case VertexFormat::UINT :
            return 4;
        case VertexFormat::UINT2 :
            return 8;
        case VertexFormat::UINT3 :
            return 12;
        case VertexFormat::UINT4 :
            return 16;
        case VertexFormat::INT :
            return 4;
        case VertexFormat::INT2 :
            return 8;
        case VertexFormat::INT3 :
            return 12;
        case VertexFormat::INT4 :
            return 16;
    }
}

VkFormat ToVulkanType(VertexFormat format)
{
    switch (format)
    {
        case VertexFormat::UCHAR2 :
            return VkFormat::VK_FORMAT_R8G8_UINT;
        case VertexFormat::UCHAR4 :
            return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
        case VertexFormat::CHAR2 :
            return VkFormat::VK_FORMAT_R8G8_SINT;
        case VertexFormat::CHAR4 :
            return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
        case VertexFormat::UCHAR2NORM :
            return VkFormat::VK_FORMAT_R8G8_UNORM;
        case VertexFormat::UCHAR4NORM :
            return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
        case VertexFormat::CHAR2NORM :
            return VkFormat::VK_FORMAT_R8G8_SNORM;
        case VertexFormat::CHAR4NORM :
            return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
        case VertexFormat::USHORT2 :
            return VkFormat::VK_FORMAT_R16G16_UINT;
        case VertexFormat::USHORT4 :
            return VkFormat::VK_FORMAT_R16G16B16A16_UINT;
        case VertexFormat::SHORT2 :
            return VkFormat::VK_FORMAT_R16G16_SINT;
        case VertexFormat::SHORT4 :
            return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
        case VertexFormat::USHORT2NORM :
            return VkFormat::VK_FORMAT_R16G16_UNORM;
        case VertexFormat::USHORT4NORM :
            return VkFormat::VK_FORMAT_R16G16B16A16_UNORM;
        case VertexFormat::SHORT2NORM :
            return VkFormat::VK_FORMAT_R16G16_SNORM;
        case VertexFormat::SHORT4NORM :
            return VkFormat::VK_FORMAT_R16G16B16A16_SNORM;
        case VertexFormat::HALF2 :
            return VkFormat::VK_FORMAT_R16G16_SFLOAT;
        case VertexFormat::HALF4 :
            return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
        case VertexFormat::FLOAT :
            return VkFormat::VK_FORMAT_R32_SFLOAT;
        case VertexFormat::FLOAT2 :
            return VkFormat::VK_FORMAT_R32G32_SFLOAT;
        case VertexFormat::FLOAT3 :
            return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
        case VertexFormat::FLOAT4 :
            return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
        case VertexFormat::UINT :
            return VkFormat::VK_FORMAT_R32_UINT;
        case VertexFormat::UINT2 :
            return VkFormat::VK_FORMAT_R32G32_UINT;
        case VertexFormat::UINT3 :
            return VkFormat::VK_FORMAT_R32G32B32_UINT;
        case VertexFormat::UINT4 :
            return VkFormat::VK_FORMAT_R32G32B32A32_UINT;
        case VertexFormat::INT :
            return VkFormat::VK_FORMAT_R32_SINT;
        case VertexFormat::INT2 :
            return VkFormat::VK_FORMAT_R32G32_SINT;
        case VertexFormat::INT3 :
            return VkFormat::VK_FORMAT_R32G32B32_SINT;
        case VertexFormat::INT4 :
            return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
    }
}

VkFormat ToVulkanType(TextureFormat format)
{
    switch (format)
    {
        
        case TextureFormat::R8UNORM :
            return VkFormat::VK_FORMAT_R8_UNORM;
        case TextureFormat::R8SNORM :
            return VkFormat::VK_FORMAT_R8_SNORM;
        case TextureFormat::R8UINT :
            return VkFormat::VK_FORMAT_R8_UINT;
        case TextureFormat::R8SINT :
            return VkFormat::VK_FORMAT_R8_SINT;
        
        case TextureFormat::R16UINT :
            return VkFormat::VK_FORMAT_R16_UINT;
        case TextureFormat::R16SINT :
            return VkFormat::VK_FORMAT_R16_SINT;
        case TextureFormat::R16FLOAT :
            return VkFormat::VK_FORMAT_R16_SFLOAT;
        case TextureFormat::RG8UNORM :
            return VkFormat::VK_FORMAT_R8G8_UNORM;
        case TextureFormat::RG8SNORM :
            return VkFormat::VK_FORMAT_R8G8_SNORM;
        case TextureFormat::RG8UINT :
            return VkFormat::VK_FORMAT_R8G8_UINT;
        case TextureFormat::RG8SINT :
            return VkFormat::VK_FORMAT_R8G8_SINT;
        
        case TextureFormat::R32UINT :
            return VkFormat::VK_FORMAT_R32_UINT;
        case TextureFormat::R32SINT :
            return VkFormat::VK_FORMAT_R32_SINT;
        case TextureFormat::R32FLOAT :
            return VkFormat::VK_FORMAT_R32_SFLOAT;
        case TextureFormat::RG16UINT :
            return VkFormat::VK_FORMAT_R16G16_UINT;
        case TextureFormat::RG16SINT :
            return VkFormat::VK_FORMAT_R16G16_SINT;
        case TextureFormat::RG16FLOAT :
            return VkFormat::VK_FORMAT_R16G16_SFLOAT;
        case TextureFormat::RGBA8UNORM :
            return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::RGBA8UNORM_SRGB :
            return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
        case TextureFormat::RGBA8SNORM :
            return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
        case TextureFormat::RGBA8UINT :
            return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
        case TextureFormat::RGBA8SINT :
            return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
        case TextureFormat::BGRA8UNORM :
            return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        case TextureFormat::BGRA8UNORM_SRGB :
            return VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
        
        case TextureFormat::RGB10A2UNORM :
            return VkFormat::VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case TextureFormat::RG11B10FLOAT :
            return VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        
        case TextureFormat::RG32UINT :
            return VkFormat::VK_FORMAT_R32G32_UINT;
        case TextureFormat::RG32SINT :
            return VkFormat::VK_FORMAT_R32G32_SINT;
        case TextureFormat::RG32FLOAT :
            return VkFormat::VK_FORMAT_R32G32_SFLOAT;
        case TextureFormat::RGBA16UINT :
            return VkFormat::VK_FORMAT_R16G16B16A16_UINT;
        case TextureFormat::RGBA16SINT :
            return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
        case TextureFormat::RGBA16FLOAT :
            return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
        
        case TextureFormat::RGBA32UINT :
            return VkFormat::VK_FORMAT_R32G32B32A32_UINT;
        case TextureFormat::RGBA32SINT :
            return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
        case TextureFormat::RGBA32FLOAT :
            return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
        
        case TextureFormat::DEPTH32FLOAT :
            return VkFormat::VK_FORMAT_D32_SFLOAT;
        case TextureFormat::DEPTH24PLUS :
            GFX_ASSERT(false);
        case TextureFormat::DEPTH24PLUS_STENCIL8 :
            return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
        
        default:
            GFX_ASSERT(false);
            return VkFormat::VK_FORMAT_UNDEFINED;
    }
}

std::uint32_t GetTextureFormatPixelSize(TextureFormat format)
{
    switch (format)
    {
        
        case TextureFormat::R8UNORM :
        case TextureFormat::R8SNORM :
        case TextureFormat::R8UINT :
        case TextureFormat::R8SINT :
            return 1;
        
        case TextureFormat::R16UINT :
        case TextureFormat::R16SINT :
        case TextureFormat::R16FLOAT :
        case TextureFormat::RG8UNORM :
        case TextureFormat::RG8SNORM :
        case TextureFormat::RG8UINT :
        case TextureFormat::RG8SINT :
            return 2;
        
        case TextureFormat::R32UINT :
        case TextureFormat::R32SINT :
        case TextureFormat::R32FLOAT :
        case TextureFormat::RG16UINT :
        case TextureFormat::RG16SINT :
        case TextureFormat::RG16FLOAT :
        case TextureFormat::RGBA8UNORM :
        case TextureFormat::RGBA8UNORM_SRGB :
        case TextureFormat::RGBA8SNORM :
        case TextureFormat::RGBA8UINT :
        case TextureFormat::RGBA8SINT :
        case TextureFormat::BGRA8UNORM :
        case TextureFormat::BGRA8UNORM_SRGB :
        case TextureFormat::RGB10A2UNORM :
        case TextureFormat::RG11B10FLOAT :
            return 4;
        
        case TextureFormat::RG32UINT :
        case TextureFormat::RG32SINT :
        case TextureFormat::RG32FLOAT :
        case TextureFormat::RGBA16UINT :
        case TextureFormat::RGBA16SINT :
        case TextureFormat::RGBA16FLOAT :
            return 8;
        
        case TextureFormat::RGBA32UINT :
        case TextureFormat::RGBA32SINT :
        case TextureFormat::RGBA32FLOAT :
            return 16;
        
        case TextureFormat::DEPTH32FLOAT :
        case TextureFormat::DEPTH24PLUS_STENCIL8 :
            return 4;
        
        case TextureFormat::DEPTH24PLUS :
            GFX_ASSERT(false);
            return 3;
        
        default:
            GFX_ASSERT(false);
            return 0;
    }
}

TextureFormat ToVulkanType(VkFormat format)
{
    switch (format)
    {
        case VkFormat::VK_FORMAT_R8_UNORM:
            return TextureFormat::R8UNORM;
        case VkFormat::VK_FORMAT_R8_SNORM:
            return TextureFormat::R8SNORM;
        case VkFormat::VK_FORMAT_R8_UINT:
            return TextureFormat::R8UINT;
        case VkFormat::VK_FORMAT_R8_SINT:
            return TextureFormat::R8SINT;
        
        case VkFormat::VK_FORMAT_R16_UINT:
            return TextureFormat::R16UINT;
        case VkFormat::VK_FORMAT_R16_SINT:
            return TextureFormat::R16SINT;
        case VkFormat::VK_FORMAT_R16_SFLOAT:
            return TextureFormat::R16FLOAT;
        case VkFormat::VK_FORMAT_R8G8_UNORM:
            return TextureFormat::RG8UNORM;
        case VkFormat::VK_FORMAT_R8G8_SNORM:
            return TextureFormat::RG8SNORM;
        case VkFormat::VK_FORMAT_R8G8_UINT:
            return TextureFormat::RG8UINT;
        case VkFormat::VK_FORMAT_R8G8_SINT:
            return TextureFormat::RG8SINT;
        
        case VkFormat::VK_FORMAT_R32_UINT:
            return TextureFormat::R32UINT;
        case VkFormat::VK_FORMAT_R32_SINT:
            return TextureFormat::R32SINT;
        case VkFormat::VK_FORMAT_R32_SFLOAT:
            return TextureFormat::R32FLOAT;
        case VkFormat::VK_FORMAT_R16G16_UINT:
            return TextureFormat::RG16UINT;
        case VkFormat::VK_FORMAT_R16G16_SINT:
            return TextureFormat::RG16SINT;
        case VkFormat::VK_FORMAT_R16G16_SFLOAT:
            return TextureFormat::RG16FLOAT;
        case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:
            return TextureFormat::RGBA8UNORM;
        case VkFormat::VK_FORMAT_R8G8B8A8_SRGB:
            return TextureFormat::RGBA8UNORM_SRGB;
        case VkFormat::VK_FORMAT_R8G8B8A8_SNORM:
            return TextureFormat::RGBA8SNORM;
        case VkFormat::VK_FORMAT_R8G8B8A8_UINT:
            return TextureFormat::RGBA8UINT;
        case VkFormat::VK_FORMAT_R8G8B8A8_SINT:
            return TextureFormat::RGBA8SINT;
        case VkFormat::VK_FORMAT_B8G8R8A8_UNORM:
            return TextureFormat::BGRA8UNORM;
        case VkFormat::VK_FORMAT_B8G8R8A8_SRGB:
            return TextureFormat::BGRA8UNORM_SRGB;
        
        case VkFormat::VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return TextureFormat::RGB10A2UNORM;
        case VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return TextureFormat::RG11B10FLOAT;
        
        case VkFormat::VK_FORMAT_R32G32_UINT:
            return TextureFormat::RG32UINT;
        case VkFormat::VK_FORMAT_R32G32_SINT:
            return TextureFormat::RG32SINT;
        case VkFormat::VK_FORMAT_R32G32_SFLOAT:
            return TextureFormat::RG32FLOAT;
        case VkFormat::VK_FORMAT_R16G16B16A16_UINT:
            return TextureFormat::RGBA16UINT;
        case VkFormat::VK_FORMAT_R16G16B16A16_SINT:
            return TextureFormat::RGBA16SINT;
        case VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT:
            return TextureFormat::RGBA16FLOAT;
        
        case VkFormat::VK_FORMAT_R32G32B32A32_UINT:
            return TextureFormat::RGBA32UINT;
        case VkFormat::VK_FORMAT_R32G32B32A32_SINT:
            return TextureFormat::RGBA32SINT;
        case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
            return TextureFormat::RGBA32FLOAT;
        
        case VkFormat::VK_FORMAT_D32_SFLOAT:
            return TextureFormat::DEPTH32FLOAT;
        case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
            return TextureFormat::DEPTH24PLUS_STENCIL8;
        
        default:
            GFX_ASSERT(false);
            return TextureFormat::INVALID;
    }
}

VkBool32 ToVulkanType(bool b)
{
    return b ? VK_TRUE : VK_FALSE;
}

VkCompareOp ToVulkanType(CompareFunction compareFunction)
{
    switch (compareFunction)
    {
        case CompareFunction::NEVER :
            return VkCompareOp::VK_COMPARE_OP_NEVER;
        case CompareFunction::LESS :
            return VkCompareOp::VK_COMPARE_OP_LESS;
        case CompareFunction::EQUAL :
            return VkCompareOp::VK_COMPARE_OP_EQUAL;
        case CompareFunction::LESS_EQUAL :
            return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareFunction::GREATER :
            return VkCompareOp::VK_COMPARE_OP_GREATER;
        case CompareFunction::NOT_EQUAL :
            return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
        case CompareFunction::GREATER_EQUAL :
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareFunction::ALWAYS :
            return VK_COMPARE_OP_ALWAYS;
    }
}

VkAttachmentLoadOp ToVulkanType(LoadOp op)
{
    switch (op)
    {
        case LoadOp::LOAD:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOp::CLEAR:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOp::UNDEFINED:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp ToVulkanType(StoreOp op)
{
    switch (op)
    {
        case StoreOp::STORE:
            return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        
        case StoreOp::CLEAR:
            return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

VkBufferUsageFlags GetVkBufferUsageFlags(BufferUsageFlags flags)
{
    VkBufferUsageFlags vkFlags = 0x0;
    
    if (BufferUsage::COPY_SRC & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    
    if (BufferUsage::COPY_DST & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    
    if (BufferUsage::INDEX & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    
    if (BufferUsage::VERTEX & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    
    if (BufferUsage::UNIFORM & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    
    if (BufferUsage::STORAGE & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    
    if (BufferUsage::INDIRECT & flags)
        vkFlags |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    
    // TODO realxie conditional rendering
    
    return vkFlags;
    
}

VkImageType ToVulkanType(TextureDimension dim)
{
    switch (dim)
    {
        case TextureDimension::TEXTURE_1D:
            return VkImageType::VK_IMAGE_TYPE_1D;
        
        case TextureDimension::TEXTURE_2D:
            return VkImageType::VK_IMAGE_TYPE_2D;
        
        case TextureDimension::TEXTURE_3D:
            return VkImageType::VK_IMAGE_TYPE_3D;
    }
}

VkImageUsageFlags GetVkImageUsageFlags(TextureUsageFlags flags, TextureFormat format)
{
    VkImageUsageFlags vkImageUsageFlags = 0x0;
    
    if (flags & (std::uint32_t) TextureUsage::COPY_SRC)
        vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    
    if (flags & (std::uint32_t) TextureUsage::COPY_DST)
        vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
    if (flags & (std::uint32_t) TextureUsage::SAMPLED)
        vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
    
    if (flags & (std::uint32_t) TextureUsage::STORAGE)
        vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
    
    if (flags & (std::uint32_t) TextureUsage::OUTPUT_ATTACHMENT)
    {
        if (TextureFormat::DEPTH24PLUS_STENCIL8 == format || TextureFormat::DEPTH32FLOAT == format)
        {
            vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        else
        {
            vkImageUsageFlags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
    }
    
    return vkImageUsageFlags;
}

VkSampleCountFlagBits GetVkSampleCountFlagBits(std::uint32_t sampleCount)
{
    switch (sampleCount)
    {
        case 1:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        
        case 2:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
        
        case 4:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
        
        case 8:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
        
        case 16:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
        
        case 64:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
        
        default:
            LOGE("Invalid sample count: %u, must be power of two.", sampleCount);
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    }
}

VkFilter ToVulkanType(FilterMode mode)
{
    switch (mode)
    {
        case FilterMode::NEAREST:
            return VkFilter::VK_FILTER_NEAREST;
        case FilterMode::LINEAR:
            return VkFilter::VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode GetVkSamplerMipmapMode(FilterMode mode)
{
    switch (mode)
    {
        case FilterMode::NEAREST:
            return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case FilterMode::LINEAR:
            return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkSamplerAddressMode ToVulkanType(AddressMode mode)
{
    switch (mode)
    {
        case AddressMode::CLAMP_TO_EDGE:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case AddressMode::REPEAT:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case AddressMode::MIRROR_REPEAT:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }
}

VkBlendFactor ToVulkanType(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::ZERO:
            return VK_BLEND_FACTOR_ZERO;
        
        case BlendFactor::ONE:
            return VK_BLEND_FACTOR_ONE;
        
        case BlendFactor::SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        
        case BlendFactor::DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        
        case BlendFactor::SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        
        case BlendFactor::DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        
        case BlendFactor::CONSTANT_COLOR:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        
        case BlendFactor::CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        
        case BlendFactor::SRC_ALPHA_SATURATE:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        
        case BlendFactor::SRC1_COLOR:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        
        case BlendFactor::ONE_MINUS_SRC1_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        
        case BlendFactor::SRC1_ALPHA:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    }
    GFX_ASSERT(false);
}

VkBlendOp ToVulkanType(BlendOp op)
{
    switch (op)
    {
        case BlendOp::ADD:
            return VK_BLEND_OP_ADD;
        
        case BlendOp::SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        
        case BlendOp::REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        
        case BlendOp::MIN:
            return VK_BLEND_OP_MIN;
        
        case BlendOp::MAX:
            return VK_BLEND_OP_MAX;
    }
    GFX_ASSERT(false);
}

VkImageAspectFlags ToVulkanType(TextureAspect aspect, VkFormat format)
{
    if (VK_FORMAT_D24_UNORM_S8_UINT != format && VK_FORMAT_D32_SFLOAT != format)
    {
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
    
    if (VK_FORMAT_D32_SFLOAT == format) {
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    
    switch (aspect)
    {
        case TextureAspect::ALL:
            return VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;
        case TextureAspect::STENCIL_ONLY:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        case TextureAspect::DEPTH_ONLY:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        
    }
}

VkImageViewType ToVulkanType(TextureViewDimension dim)
{
    switch (dim)
    {
        case TextureViewDimension::DIM_1D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
        case TextureViewDimension::DIM_2D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        case TextureViewDimension::DIM_2D_ARRAY:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case TextureViewDimension::DIM_CUBE:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureViewDimension::DIM_CUBE_ARRAY:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        case TextureViewDimension::DIM_3D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
    }
}

VkStencilOp ToVulkanType(StencilOperation op)
{
    switch (op)
    {
        case StencilOperation::KEEP:
            return VK_STENCIL_OP_KEEP;
        
        case StencilOperation::ZERO:
            return VK_STENCIL_OP_ZERO;
        
        case StencilOperation::REPLACE:
            return VK_STENCIL_OP_REPLACE;
        
        case StencilOperation::INVERT:
            return VK_STENCIL_OP_INVERT;
        
        case StencilOperation::INCREMENT_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        
        case StencilOperation::DECREMENT_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        
        case StencilOperation::INCREMENT_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        
        case StencilOperation::DECREMENT_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }
}

bool TextureFormatHasDepth(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH24PLUS_STENCIL8:
        case TextureFormat::DEPTH24PLUS:
        case TextureFormat::DEPTH32FLOAT:
            return true;
        default:
            return false;
    }
}

bool TextureFormatHasStencil(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH24PLUS_STENCIL8:
            return true;
        default:
            return false;
    }
}

bool TextureFormatHasDepthOrStencil(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH24PLUS_STENCIL8:
            return true;
        default:
            return false;
    }
}

// Computes which vulkan access type could be required for the given Dawn usage.
VkAccessFlags VulkanAccessFlags(TextureUsageFlags usage, TextureFormat format)
{
    VkAccessFlags flags = 0;
    
    if (usage & TextureUsage::COPY_SRC)
    {
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (usage & TextureUsage::COPY_DST)
    {
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (usage & TextureUsage::SAMPLED)
    {
        flags |= VK_ACCESS_SHADER_READ_BIT;
    }
    if (usage & TextureUsage::STORAGE)
    {
        flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    }
    if (usage & TextureUsage::OUTPUT_ATTACHMENT)
    {
        if (TextureFormatHasDepthOrStencil(format))
        {
            flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        else
        {
            flags |=
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
    }
    if (usage & TextureUsage::PRESENT)
    {
        // There is no access flag for present because the VK_KHR_SWAPCHAIN extension says
        // that vkQueuePresentKHR makes the memory of the image visible to the presentation
        // engine. There's also a note explicitly saying dstAccessMask should be 0. On the
        // other side srcAccessMask can also be 0 because synchronization is required to
        // happen with a semaphore instead.
        flags |= 0;
    }
    
    return flags;
}

// Chooses which Vulkan image layout should be used for the given Dawn usage
VkImageLayout GetVulkanImageLayout(TextureUsageFlags usage, TextureFormat format)
{
    if (usage == TextureUsage::UNDEFINED)
    {
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }
    
    if (usage == 0 || (usage & (usage - 1)) != 0)
    {
        return VK_IMAGE_LAYOUT_GENERAL;
    }
    
    // Usage has a single bit so we can switch on its value directly.
    switch (usage)
    {
        case TextureUsage::COPY_DST:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case TextureUsage::SAMPLED:
            if (TextureFormat::DEPTH24PLUS_STENCIL8 == format)
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            else
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // Vulkan texture copy functions require the image to be in _one_  known layout.
            // Depending on whether parts of the texture have been transitioned to only
            // TransferSrc or a combination with something else, the texture could be in a
            // combination of GENERAL and TRANSFER_SRC_OPTIMAL. This would be a problem, so we
            // make TransferSrc use GENERAL.
        case TextureUsage::COPY_SRC:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            // Writable storage textures must use general. If we could know the texture is read
            // only we could use SHADER_READ_ONLY_OPTIMAL
        case TextureUsage::STORAGE:
            return VK_IMAGE_LAYOUT_GENERAL;
        case TextureUsage::OUTPUT_ATTACHMENT:
            if (TextureFormat::DEPTH24PLUS_STENCIL8 == format)
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            else if (TextureFormat::DEPTH32FLOAT == format)
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            else
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case TextureUsage::PRESENT:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            return VK_IMAGE_LAYOUT_GENERAL;
    }
}

// Computes which Vulkan pipeline stage can access a texture in the given  usage
VkPipelineStageFlags VulkanPipelineStage(TextureUsageFlags usage, TextureFormat format)
{
    VkPipelineStageFlags flags = 0;
    
    if (usage == TextureUsage::UNDEFINED)
    {
        // This only happens when a texture is initially created (and for srcAccessMask) in
        // which case there is no need to wait on anything to stop accessing this texture.
        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    if (usage & (TextureUsage::COPY_SRC | TextureUsage::COPY_DST))
    {
        flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    if (usage & (TextureUsage::SAMPLED | TextureUsage::STORAGE))
    {
        flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if (usage & TextureUsage::OUTPUT_ATTACHMENT)
    {
        if (TextureFormatHasDepthOrStencil(format))
        {
            flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                     VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            // TODO(cwallez@chromium.org): This is missing the stage where the depth and
            // stencil values are written, but it isn't clear which one it is.
        }
        else
        {
            flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
    }
    if (usage & TextureUsage::PRESENT)
    {
        // There is no pipeline stage for present but a pipeline stage is required so we use
        // "bottom of pipe" to block as little as possible and vkQueuePresentKHR will make
        // the memory visible to the presentation engine. The spec explicitly mentions that
        // "bottom of pipe" is ok. On the other direction, synchronization happens with a
        // semaphore so bottom of pipe is ok too (but maybe it could be "top of pipe" to
        // block less?)
        flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    
    // A zero value isn't a valid pipeline stage mask
    GFX_ASSERT(flags != 0);
    return flags;
}

// Computes which Vulkan texture aspects are relevant for the given Dawn format
VkImageAspectFlags VulkanAspectMask(TextureFormat format)
{
    bool isDepth = TextureFormatHasDepth(format);
    bool isStencil = TextureFormatHasStencil(format);
    
    VkImageAspectFlags flags = 0;
    if (isDepth)
    {
        flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (isStencil)
    {
        flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    
    if (flags != 0)
    {
        return flags;
    }
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

NS_GFX_END

