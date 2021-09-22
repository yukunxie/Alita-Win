LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := SPIRV_static
LOCAL_MODULE_FILENAME := SPIRV_lib

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
    GlslangToSpv.cpp \
        InReadableOrder.cpp \
        Logger.cpp \
        SpvBuilder.cpp \
        SpvPostProcess.cpp \
        doc.cpp \
        SpvTools.cpp \
        disassemble.cpp


LOCAL_CPPFLAGS := -Wno-deprecated-declarations
LOCAL_CFLAGS := -DUSE_FILE32API -Wno-deprecated-declarations

LOCAL_CPP_EXTENSION := .cc .cpp
LOCAL_CFLAGS += -x c++

LOCAL_EXPORT_CFLAGS   := -DUSE_FILE32API
LOCAL_EXPORT_CPPFLAGS := -Wno-deprecated-declarations

include $(BUILD_STATIC_LIBRARY)