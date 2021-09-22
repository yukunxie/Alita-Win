LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vulkan_lib_static
LOCAL_MODULE_FILENAME := vulkan_lib

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
ShaderHelper.cpp \
VKBindGroup.cpp \
VKBindGroupLayout.cpp \
VKBuffer.cpp \
VKCommandBuffer.cpp \
VKCommandEncoder.cpp \
VKDevice.cpp \
VKFramebuffer.cpp \
VKPipelineLayout.cpp \
VKQueue.cpp \
VKRenderPass.cpp \
VKRenderPassEncoder.cpp \
VKRenderPipeline.cpp \
VKRenderQueue.cpp \
VKSampler.cpp \
VKShader.cpp \
VKSwapChain.cpp \
VKTexture.cpp \
VKTextureView.cpp \
VKTypes.cpp \
vulkan_wrapper.cpp

ifeq ($(SE_JSC),1)
  LOCAL_C_INCLUDES += $(LOCAL_PATH)../../include
endif

# LOCAL_EXPORT_LDLIBS := -ljnigraphics \
#                        -llog \
#                        -landroid

# LOCAL_STATIC_LIBRARIES += cocos_png_static

LOCAL_CPPFLAGS := -Wno-deprecated-declarations
LOCAL_CFLAGS := -DUSE_FILE32API -Wno-deprecated-declarations

LOCAL_CPP_EXTENSION := .cc .cpp
LOCAL_CFLAGS += -x c++

LOCAL_EXPORT_CFLAGS   := -DUSE_FILE32API
LOCAL_EXPORT_CPPFLAGS := -Wno-deprecated-declarations

include $(BUILD_STATIC_LIBRARY)
