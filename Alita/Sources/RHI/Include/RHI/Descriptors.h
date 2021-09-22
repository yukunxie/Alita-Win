//
// Created by realxie on 2019-10-21.
//

#ifndef RHI_DESCRIPTORS_H
#define RHI_DESCRIPTORS_H

#include "Macros.h"
#include "Flags.h"
#include "TurboVector.h"
#include "Constants.h"

#include <cmath>
#include <vector>
#include <optional>
#include <limits>

NS_RHI_BEGIN

class Device;

class TextureView;

class Texture;

class PipelineLayout;

class Buffer;

class BindingResource;

class BindGroupLayout;

class Shader;

class QuerySet;

typedef std::uint32_t Bool32;

enum class PowerPreference
{
    LOW_POWER,
    HIGH_PERFORMANCE
};

enum class TextureFormat
{
    INVALID,
    
    // 8-bit formats
        R8UNORM,
    R8SNORM,
    R8UINT,
    R8SINT,
    
    // 16-bit formats
        R16UINT,
    R16SINT,
    R16FLOAT,
    RG8UNORM,
    RG8SNORM,
    RG8UINT,
    RG8SINT,
    
    // 32-bit formats
        R32UINT,
    R32SINT,
    R32FLOAT,
    RG16UINT,
    RG16SINT,
    RG16FLOAT,
    RGBA8UNORM,
    RGBA8UNORM_SRGB,
    RGBA8SNORM,
    RGBA8UINT,
    RGBA8SINT,
    BGRA8UNORM,
    BGRA8UNORM_SRGB,
    
    // Packed 32-bit formats
        RGB10A2UNORM,
    RG11B10FLOAT,
    
    // 64-bit formats
        RG32UINT,
    RG32SINT,
    RG32FLOAT,
    RGBA16UINT,
    RGBA16SINT,
    RGBA16FLOAT,
    
    // 128-bit formats
        RGBA32UINT,
    RGBA32SINT,
    RGBA32FLOAT,
    
    // Depth and stencil formats
        DEPTH32FLOAT,
    DEPTH24PLUS,
    DEPTH24PLUS_STENCIL8
};

struct ColorWrite
{
    static const std::uint32_t RED = 0x1;
    static const std::uint32_t GREEN = 0x2;
    static const std::uint32_t BLUE = 0x4;
    static const std::uint32_t ALPHA = 0x8;
    static const std::uint32_t ALL = 0xF;
};

enum class VertexFormat
{
    UCHAR2,
    UCHAR4,
    CHAR2,
    CHAR4,
    UCHAR2NORM,
    UCHAR4NORM,
    CHAR2NORM,
    CHAR4NORM,
    USHORT2,
    USHORT4,
    SHORT2,
    SHORT4,
    USHORT2NORM,
    USHORT4NORM,
    SHORT2NORM,
    SHORT4NORM,
    HALF2,
    HALF4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    UINT,
    UINT2,
    UINT3,
    UINT4,
    INT,
    INT2,
    INT3,
    INT4
};

enum class IndexFormat
{
    UINT16,
    UINT32,
};

enum class LoadOp
{
    LOAD,
    CLEAR,
    UNDEFINED,
};

enum class StoreOp
{
    STORE,
    CLEAR,
};

union Color
{
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };
    
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
};

struct RenderPassColorAttachmentDescriptor
{
    TextureView* attachment = nullptr;
    TextureView* resolveTarget = nullptr;
    Color loadValue;
    LoadOp loadOp = LoadOp::LOAD;
    StoreOp storeOp = StoreOp::STORE;
    
    RenderPassColorAttachmentDescriptor& operator = (const RenderPassColorAttachmentDescriptor& other)
    {
        this->attachment = other.attachment;
        this->resolveTarget = other.resolveTarget;
        this->loadValue = other.loadValue;
        this->loadOp = other.loadOp;
        this->storeOp = other.storeOp;
        return *this;
    }
};

struct RenderPassDepthStencilAttachmentDescriptor
{
    TextureView* attachment = nullptr;
    
    LoadOp depthLoadOp = LoadOp::CLEAR;
    StoreOp depthStoreOp = StoreOp::STORE;
    float depthLoadValue = 1.0f;
    
    LoadOp stencilLoadOp = LoadOp::CLEAR;
    StoreOp stencilStoreOp = StoreOp::STORE;
    std::uint32_t stencilLoadValue = 0;
};

struct ObjectDescriptorBase
{
    // std::string label;
};

struct RenderPassDescriptor : ObjectDescriptorBase
{
    TurboVector<RenderPassColorAttachmentDescriptor, kMaxColorAttachments + 1> colorAttachments;
    RenderPassDepthStencilAttachmentDescriptor depthStencilAttachment;
    QuerySet* occlusionQuerySet = nullptr;
};

struct RenderBundleEncoderDescriptor : ObjectDescriptorBase
{
    TurboVector<TextureFormat, kMaxColorAttachments> colorFormats;
    TextureFormat depthStencilFormat = TextureFormat::INVALID;
    std::uint32_t sampleCount = 1;
};

struct RenderBundleDescriptor : ObjectDescriptorBase
{
};

struct ComputePassDescriptor : ObjectDescriptorBase
{
};

struct FenceDescriptor : ObjectDescriptorBase
{
    std::uint64_t initialValue = 0;
};

struct ProgrammableStageDescriptor
{
    Shader* shader;
    std::string entryPoint;
    // TODO: other stuff like specialization constants?
    
    bool operator==(const ProgrammableStageDescriptor &other) const
    {
        return this->shader == other.shader && this->entryPoint == other.entryPoint;
    }
};

struct RasterizationStateDescriptor
{
    FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
    CullMode cullMode = CullMode::NONE;
    std::uint32_t depthBias = 0;
    float depthBiasSlopeScale = 0;
    float depthBiasClamp = 0;
    
    bool operator==(const RasterizationStateDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->frontFace == other.frontFace;
        ret = ret && this->cullMode == other.cullMode;
        ret = ret && this->depthBias == other.depthBias;
        ret = ret && std::fabs(this->depthBiasSlopeScale - other.depthBiasSlopeScale) < 1e-5;
        ret = ret && std::fabs(this->depthBiasClamp - other.depthBiasClamp) < 1e-5;
        return ret;
    }
};

enum class CompareFunction
{
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

struct BlendDescriptor
{
    BlendFactor srcFactor = BlendFactor::ONE;
    BlendFactor dstFactor = BlendFactor::ZERO;
    BlendOp operation = BlendOp::ADD;
    
    bool operator==(const BlendDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->srcFactor == other.srcFactor;
        ret = ret && this->dstFactor == other.dstFactor;
        ret = ret && this->operation == other.operation;
        return ret;
    }
};

struct ColorStateDescriptor
{
    TextureFormat format;
    BlendDescriptor alphaBlend;
    BlendDescriptor colorBlend;
    std::uint32_t writeMask = ColorWrite::ALL;  // GPUColorWrite.ALL
    
    bool operator==(const ColorStateDescriptor &other) const
    {
        bool ret = this->format == other.format;
        ret = ret && this->alphaBlend == other.alphaBlend;
        ret = ret && this->colorBlend == other.colorBlend;
        ret = ret && this->writeMask == other.writeMask;
        return ret;
    }
};

enum class StencilOperation
{
    KEEP,
    ZERO,
    REPLACE,
    INVERT,
    INCREMENT_CLAMP,
    DECREMENT_CLAMP,
    INCREMENT_WRAP,
    DECREMENT_WRAP
};

struct StencilStateFaceDescriptor
{
    CompareFunction compare = CompareFunction::ALWAYS;
    StencilOperation failOp = StencilOperation::KEEP;
    StencilOperation depthFailOp = StencilOperation::KEEP;
    StencilOperation passOp = StencilOperation::KEEP;
    
    bool operator==(const StencilStateFaceDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->compare == other.compare;
        ret = ret && this->failOp == other.failOp;
        ret = ret && this->depthFailOp == other.depthFailOp;
        ret = ret && this->passOp == other.passOp;
        return ret;
    }
};

struct DepthStencilStateDescriptor
{
    TextureFormat format = TextureFormat::DEPTH24PLUS_STENCIL8;
    
    Bool32 depthWriteEnabled = false;
    CompareFunction depthCompare = CompareFunction::ALWAYS;
    
    StencilStateFaceDescriptor stencilFront;
    StencilStateFaceDescriptor stencilBack;
    
    std::uint32_t stencilReadMask = 0xFFFFFFFF;
    std::uint32_t stencilWriteMask = 0xFFFFFFFF;
    
    bool operator==(const DepthStencilStateDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->format == other.format;
        ret = ret && this->depthWriteEnabled == other.depthWriteEnabled;
        ret = ret && this->depthCompare == other.depthCompare;
        ret = ret && this->stencilFront == other.stencilFront;
        ret = ret && this->stencilBack == other.stencilBack;
        ret = ret && this->stencilReadMask == other.stencilReadMask;
        ret = ret && this->stencilWriteMask == other.stencilWriteMask;
        return ret;
    }
};

struct VertexAttributeDescriptor
{
    BufferSize offset = 0;
    VertexFormat format;
    std::uint32_t shaderLocation;
    
    bool operator==(const VertexAttributeDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->offset == other.offset;
        ret = ret && this->format == other.format;
        ret = ret && this->shaderLocation == other.shaderLocation;
        return ret;
    }
};

enum class InputStepMode
{
    VERTEX,
    INSTANCE
};

struct VertexBufferDescriptor
{
    BufferSize arrayStride;
    InputStepMode stepMode = InputStepMode::VERTEX;
    TurboVector<VertexAttributeDescriptor, kMaxVertexAttributes> attributes;
    
    bool operator==(const VertexBufferDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->arrayStride == other.arrayStride;
        ret = ret && this->stepMode == other.stepMode;
        ret = ret && this->attributes.size() == other.attributes.size();
        
        for (size_t i = 0; ret && (i < attributes.size()); ++i)
        {
            ret = ret && attributes[i] == other.attributes[i];
        }
        
        return ret;
    }
};

struct VertexStateDescriptor
{
    IndexFormat indexFormat = IndexFormat::UINT32;
    TurboVector<VertexBufferDescriptor, kMaxVertexInputs> vertexBuffers;
    
    bool operator==(const VertexStateDescriptor &other) const
    {
        bool ret = this->indexFormat == other.indexFormat;
        ret = ret && vertexBuffers.size() == other.vertexBuffers.size();
        
        for (size_t i = 0; ret && (i < vertexBuffers.size()); ++i)
        {
            ret = ret && vertexBuffers[i] == other.vertexBuffers[i];
        }
        
        return ret;
    }
};

struct PipelineDescriptorBase : ObjectDescriptorBase
{
    PipelineLayout* layout = nullptr;
};

struct ComputePipelineDescriptor : public PipelineDescriptorBase
{
    ProgrammableStageDescriptor computeStage;
};

struct RenderPipelineDescriptor : public PipelineDescriptorBase
{
    ProgrammableStageDescriptor vertexStage;
    ProgrammableStageDescriptor fragmentStage;
    
    PrimitiveTopology primitiveTopology;
    RasterizationStateDescriptor rasterizationState;
    TurboVector<ColorStateDescriptor, kMaxColorAttachments + 1> colorStates;
    DepthStencilStateDescriptor depthStencilState;
    bool hasDepthStencilState = false;
    VertexStateDescriptor vertexState;
    
    std::uint32_t sampleCount = 1;
    std::uint32_t sampleMask = 0xFFFFFFFF;
    std::uint32_t alphaToCoverageEnabled = false;
    // TODO: other properties
    
    bool operator==(const RenderPipelineDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->layout == other.layout;
        ret = ret && this->vertexStage == other.vertexStage;
        ret = ret && this->fragmentStage == other.fragmentStage;
        ret = ret && this->primitiveTopology == other.primitiveTopology;
        ret = ret && this->primitiveTopology == other.primitiveTopology;
        ret = ret && this->rasterizationState == other.rasterizationState;
        ret = ret && this->depthStencilState == other.depthStencilState;
        ret = ret && this->vertexState == other.vertexState;
        ret = ret && this->sampleCount == other.sampleCount;
        ret = ret && this->sampleMask == other.sampleMask;
        ret = ret && this->alphaToCoverageEnabled == other.alphaToCoverageEnabled;
        ret = ret && colorStates.size() == other.colorStates.size();
        
        for (size_t i = 0; ret && (i < colorStates.size()); ++i)
        {
            ret = ret && colorStates[i] == other.colorStates[i];
        }
        
        return ret;
    }
};

enum class TextureDimension
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D
};

typedef std::uint32_t TextureUsageFlags;
struct TextureUsage
{
    static const TextureUsageFlags UNDEFINED = 0x0;
    static const TextureUsageFlags COPY_SRC = 0x01;
    static const TextureUsageFlags COPY_DST = 0x02;
    static const TextureUsageFlags SAMPLED = 0x04;
    static const TextureUsageFlags STORAGE = 0x08;
    static const TextureUsageFlags OUTPUT_ATTACHMENT = 0x10;
    static const TextureUsageFlags PRESENT = 0x20;
};

struct TextureDescriptor : public ObjectDescriptorBase
{
    Extent3D size;
    std::uint32_t arrayLayerCount = 1;
    std::uint32_t mipLevelCount = 1;
    std::uint32_t sampleCount = 1;
    TextureDimension dimension = TextureDimension::TEXTURE_2D;
    TextureFormat format;
    TextureUsageFlags usage;
};
enum TextureViewDimension
{
    DIM_1D,
    DIM_2D,
    DIM_2D_ARRAY,
    DIM_CUBE,
    DIM_CUBE_ARRAY,
    DIM_3D
};

// enum class TextureViewDimension {
//     TEXTUREVIEW_1D,
//     TEXTUREVIEW_2D,
//     TEXTUREVIEW_2D_ARRAY,
//     TEXTUREVIEW_CUBE,
//     TEXTUREVIEW_CUBE_ARRAY,
//     TEXTUREVIEW_3D,
// };

enum class TextureAspect
{
    ALL,
    STENCIL_ONLY,
    DEPTH_ONLY,
};

struct TextureViewDescriptor : ObjectDescriptorBase
{
    TextureFormat format = TextureFormat::INVALID;
    TextureViewDimension dimension = TextureViewDimension::DIM_2D;
    TextureAspect aspect = TextureAspect::ALL;
    std::uint32_t baseMipLevel = 0;
    std::uint32_t mipLevelCount = 0;
    std::uint32_t baseArrayLayer = 0;
    std::uint32_t arrayLayerCount = 0;
    
    bool operator==(const TextureViewDescriptor &other) const
    {
        bool ret = true;
        ret = ret && this->format == other.format;
        ret = ret && this->dimension == other.dimension;
        ret = ret && this->aspect == other.aspect;
        ret = ret && this->baseMipLevel == other.baseMipLevel;
        ret = ret && this->mipLevelCount == other.mipLevelCount;
        ret = ret && this->baseArrayLayer == other.baseArrayLayer;
        ret = ret && this->arrayLayerCount == other.arrayLayerCount;
        return ret;
    }
};

enum class AddressMode
{
    CLAMP_TO_EDGE,
    REPEAT,
    MIRROR_REPEAT
};

enum class FilterMode
{
    NEAREST,
    LINEAR
};

struct SamplerDescriptor : ObjectDescriptorBase
{
    AddressMode addressModeU = AddressMode::CLAMP_TO_EDGE;
    AddressMode addressModeV = AddressMode::CLAMP_TO_EDGE;
    AddressMode addressModeW = AddressMode::CLAMP_TO_EDGE;
    FilterMode magFilter = FilterMode::NEAREST;
    FilterMode minFilter = FilterMode::NEAREST;
    FilterMode mipmapFilter = FilterMode::NEAREST;
    float lodMinClamp = 0;
    float lodMaxClamp = std::numeric_limits<float>::max(); // TODO: What should this be? Was Number.MAX_VALUE.
    CompareFunction compare = CompareFunction::ALWAYS;
};

typedef std::uint32_t BufferUsageFlags;

struct BufferUsage
{
    static const BufferUsageFlags MAP_READ = 0x0001;
    static const BufferUsageFlags MAP_WRITE = 0x0002;
    static const BufferUsageFlags COPY_SRC = 0x0004;
    static const BufferUsageFlags COPY_DST = 0x0008;
    static const BufferUsageFlags INDEX = 0x0010;
    static const BufferUsageFlags VERTEX = 0x0020;
    static const BufferUsageFlags UNIFORM = 0x0040;
    static const BufferUsageFlags STORAGE = 0x0080;
    static const BufferUsageFlags INDIRECT = 0x0100;
    static const BufferUsageFlags QUERY_RESOLVE = 0x0200;
};

typedef std::uint32_t MapModeFlag;
typedef std::uint32_t MapModeFlags;

struct MapMode
{
    static const MapModeFlag MAP_READ =  0x0001;
    static const MapModeFlag MAP_WRITE = 0x0002;
};

struct BufferDescriptor : ObjectDescriptorBase
{
    BufferSize size;
    BufferUsageFlags usage;
};

struct Origin2DDict
{
    std::uint32_t x = 0;
    std::uint32_t y = 0;
};
typedef Origin2DDict Origin2D;

struct Origin3DDict
{
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t z = 0;
};
typedef Origin3DDict Origin3D;

//struct Extent3DDict
//{
//    std::uint32_t width;
//    std::uint32_t height;
//    std::uint32_t depth;
//};
//typedef Extent3DDict Extent3D;

struct BufferCopyView
{
    Buffer* buffer = nullptr;
    BufferSize offset = 0;
    // Notice, bytesPerRow must be exact divided by 256
    std::uint32_t bytesPerRow = 0;
    std::uint32_t rowsPerImage = 0;
};

struct TextureCopyView
{
    Texture* texture = nullptr;
    std::uint32_t mipLevel = 0;
    // origin.z 存储了baseArrayLayer信息
    Origin3D origin = {};
};

enum class BindingType
{
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    READONLY_STORAGE_BUFFER,
    SAMPLER,
    SAMPLED_TEXTURE,
    STORAGE_TEXTURE
    // TODO: other binding types
};

enum class TextureComponentType
{
    FLOAT,
    SINT,
    UINT
};


struct ShaderStage
{
    static const ShaderStageFlags VERTEX = 0x1;
    static const ShaderStageFlags FRAGMENT = 0x2;
    static const ShaderStageFlags COMPUTE = 0x4;
};

struct BindGroupLayoutBinding
{
    std::uint32_t binding;
    ShaderStageFlags visibility;
    BindingType type;
    
    bool operator==(const BindGroupLayoutBinding &other) const
    {
        bool ret = true;
        ret = ret && this->binding == other.binding;
        ret = ret && this->visibility == other.visibility;
        ret = ret && this->type == other.type;
        return ret;
    }
};

struct BindGroupLayoutDescriptor
{
    TurboVector<BindGroupLayoutBinding, kMaxBindingsPerGroup> entries;
    
    bool operator == (const BindGroupLayoutDescriptor& other) const
    {
        bool ret = entries.size() == other.entries.size();
        
        for (size_t i = 0; ret && (i < entries.size()); ++i)
        {
            ret = ret && entries[i] == other.entries[i];
        }
        
        return ret;
    }
};

struct BindGroupBinding
{
    std::uint32_t binding;
    BindingResource* resource = nullptr;
};

struct BindGroupDescriptor : ObjectDescriptorBase
{
    BindGroupLayout* layout = nullptr;
    TurboVector<BindGroupBinding, kMaxBindingsPerGroup> entries;
};

struct PipelineLayoutDescriptor
{
    TurboVector<BindGroupLayout*, kMaxBindingsPerGroup> bindGroupLayouts;
    
    bool operator == (const PipelineLayoutDescriptor& other) const
    {
        bool ret = bindGroupLayouts.size() == other.bindGroupLayouts.size();
        
        for (size_t i = 0; ret && (i < bindGroupLayouts.size()); ++i)
        {
            ret = ret && bindGroupLayouts[i] == other.bindGroupLayouts[i];
        }
        
        return ret;
    }
};

struct CommandEncoderDescriptor : ObjectDescriptorBase
{
    // TODO: reusability flag?
};

enum class ShaderCodeType
{
    BINARY,
    TEXT,
};

struct ShaderModuleDescriptor : ObjectDescriptorBase
{
    ShaderCodeType codeType = ShaderCodeType::BINARY;
    std::vector<std::uint8_t> code;
    std::string textCode;
};

struct CommandBufferDescriptor : ObjectDescriptorBase
{
};

struct SwapChainDescriptor : ObjectDescriptorBase
{
    Device* device = nullptr;
    TextureFormat format = TextureFormat::RGBA8UNORM;
    TextureUsageFlags usage = TextureUsage::OUTPUT_ATTACHMENT;
};

struct Extensions
{
    Bool32 anisotropicFiltering = false;
};

struct Limits
{
    std::uint32_t maxBindGroups = 4;
    std::uint32_t maxDynamicUniformBuffersPerPipelineLayout = 8;
    std::uint32_t maxDynamicStorageBuffersPerPipelineLayout = 4;
    std::uint32_t maxSampledTexturesPerShaderStage = 16;
    std::uint32_t maxSamplersPerShaderStage = 16;
    std::uint32_t maxStorageBuffersPerPipelineLayout = 8;
    std::uint32_t maxStorageTexturesPerShaderStage = 4;
    std::uint32_t maxUniformBuffersPerShaderStage = 12;
};

struct DeviceDescriptor
{
    Extensions extensions;
    Limits limits;
};

struct RenderTargetDescriptor
{
};

struct BindingResourceDescriptor
{
};

enum class QueryType
{
    OCCLUSION,
};

struct QuerySetDescriptor : ObjectDescriptorBase {
    QueryType type;
    std::uint32_t count;
};

NS_RHI_END

#endif //RHI_DESCRIPTORS_H
