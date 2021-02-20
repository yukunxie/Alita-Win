//
// Created by realxie on 2019-10-21.
//

#ifndef ALITA_DESCRIPTORS_H
#define ALITA_DESCRIPTORS_H

#include "Macros.h"
#include "Flags.h"

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

enum class PowerPreference
{
    LOW_POWER,
    HIGH_PERFORMANCE
};

enum class TextureFormat
{
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

enum class ColorWrite
{
    RED = 0x1,
    GREEN = 0x2,
    BLUE = 0x4,
    ALPHA = 0x8,
    ALL = 0xF,
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
    LoadOp loadOp;
    StoreOp storeOp;
};

struct RenderPassDepthStencilAttachmentDescriptor
{
    TextureView* attachment = nullptr;
    
    LoadOp depthLoadOp;
    StoreOp depthStoreOp;
    float depthLoadValue = 1.0f;
    
    LoadOp stencilLoadOp;
    StoreOp stencilStoreOp;
    std::uint32_t stencilLoadValue = 0;
};

struct ObjectDescriptorBase
{
    std::string label;
};

struct RenderPassDescriptor : ObjectDescriptorBase
{
    std::vector<RenderPassColorAttachmentDescriptor> colorAttachments;
    RenderPassDepthStencilAttachmentDescriptor depthStencilAttachment;
};

struct ProgrammableStageDescriptor
{
    ShaderModule* module;
    std::string entryPoint;
    // TODO: other stuff like specialization constants?
};

struct RasterizationStateDescriptor
{
    FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
    CullMode cullMode = CullMode::NONE;
    std::uint32_t depthBias = 0;
    float depthBiasSlopeScale = 0;
    float depthBiasClamp = 0;
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
};

struct ColorStateDescriptor
{
    TextureFormat format;
    BlendDescriptor alphaBlend;
    BlendDescriptor colorBlend;
    ColorWrite writeMask = ColorWrite::ALL;  // GPUColorWrite.ALL
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
};

struct DepthStencilStateDescriptor
{
    TextureFormat format;
    
    bool depthWriteEnabled = false;
    CompareFunction depthCompare = CompareFunction::ALWAYS;
    
    StencilStateFaceDescriptor stencilFront;
    StencilStateFaceDescriptor stencilBack;
    
    std::uint32_t stencilReadMask = 0xFFFFFFFF;
    std::uint32_t stencilWriteMask = 0xFFFFFFFF;
};

struct VertexAttributeDescriptor
{
    BufferSize offset = 0;
    VertexFormat format;
    std::uint32_t shaderLocation;
};

enum class InputStepMode
{
    VERTEX,
    INSTANCE
};

struct VertexBufferDescriptor
{
    BufferSize stride;
    InputStepMode stepMode = InputStepMode::VERTEX;
    std::vector<VertexAttributeDescriptor> attributeSet;
};

struct VertexInputDescriptor
{
    IndexFormat indexFormat = IndexFormat::UINT32;
    std::vector<VertexBufferDescriptor> vertexBuffers;
};

struct PipelineDescriptorBase : ObjectDescriptorBase
{
    PipelineLayout* layout;
};

struct RenderPipelineDescriptor : public PipelineDescriptorBase
{
    ProgrammableStageDescriptor vertexStage;
    ProgrammableStageDescriptor fragmentStage;
    
    PrimitiveTopology primitiveTopology;
    RasterizationStateDescriptor rasterizationState;
    std::vector<ColorStateDescriptor> colorStates;
    std::optional<DepthStencilStateDescriptor> depthStencilState;
    VertexInputDescriptor vertexInput;
    
    std::uint32_t sampleCount = 1;
    std::uint32_t sampleMask = 0xFFFFFFFF;
    bool alphaToCoverageEnabled = false;
    // TODO: other properties
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
    static const TextureUsageFlags COPY_SRC = 0x01;
    static const TextureUsageFlags COPY_DST = 0x02;
    static const TextureUsageFlags SAMPLED = 0x04;
    static const TextureUsageFlags STORAGE = 0x08;
    static const TextureUsageFlags OUTPUT_ATTACHMENT = 0x10;
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
    CompareFunction compare = CompareFunction::NEVER;
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
    // Notice, rowPitch must be exact divided by 256
    std::uint32_t rowPitch;
    std::uint32_t imageHeight;
};

struct TextureCopyView
{
    Texture* texture = nullptr;
    std::uint32_t mipLevel = 0;
    std::uint32_t arrayLayer = 0;
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

enum TextureViewDimension
{
    DIM_1D,
    DIM_2D,
    DIM_2D_ARRAY,
    DIM_CUBE,
    DIM_CUBE_ARRAY,
    DIM_3D
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
    TextureViewDimension textureDimension = TextureViewDimension::DIM_2D;
    TextureComponentType textureComponentType = TextureComponentType::FLOAT;
    bool multisampled = false;
    bool hasDynamicOffset = false;
};

struct BindGroupLayoutDescriptor
{
    std::vector<BindGroupLayoutBinding> bindings;
};

struct BindGroupBinding
{
    std::uint32_t binding;
    const BindingResource* resource = nullptr;
};

struct BindGroupDescriptor : ObjectDescriptorBase
{
    BindGroupLayout* layout = nullptr;
    std::vector<BindGroupBinding> bindings;
};

struct PipelineLayoutDescriptor
{
    std::vector<const BindGroupLayout*> bindGroupLayouts;
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
    ShaderCodeType codeType;
    std::vector<std::uint8_t> binaryCode;
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

NS_RHI_END

#endif //ALITA_DESCRIPTORS_H
