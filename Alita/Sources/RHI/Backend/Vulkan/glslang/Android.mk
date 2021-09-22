LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := glslang_static
LOCAL_MODULE_FILENAME := glslang_lib

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
    MachineIndependent/glslang_tab.cpp \
    MachineIndependent/attribute.cpp \
    MachineIndependent/Constant.cpp \
    MachineIndependent/iomapper.cpp \
    MachineIndependent/InfoSink.cpp \
    MachineIndependent/Initialize.cpp \
    MachineIndependent/IntermTraverse.cpp \
    MachineIndependent/Intermediate.cpp \
    MachineIndependent/ParseContextBase.cpp \
    MachineIndependent/ParseHelper.cpp \
    MachineIndependent/PoolAlloc.cpp \
    MachineIndependent/RemoveTree.cpp \
    MachineIndependent/Scan.cpp \
    MachineIndependent/ShaderLang.cpp \
    MachineIndependent/SymbolTable.cpp \
    MachineIndependent/Versions.cpp \
    MachineIndependent/intermOut.cpp \
    MachineIndependent/limits.cpp \
    MachineIndependent/linkValidate.cpp \
    MachineIndependent/parseConst.cpp \
    MachineIndependent/reflection.cpp \
    MachineIndependent/preprocessor/Pp.cpp \
    MachineIndependent/preprocessor/PpAtom.cpp \
    MachineIndependent/preprocessor/PpContext.cpp \
    MachineIndependent/preprocessor/PpScanner.cpp \
    MachineIndependent/preprocessor/PpTokens.cpp \
    MachineIndependent/propagateNoContraction.cpp \
    GenericCodeGen/CodeGen.cpp \
    GenericCodeGen/Link.cpp \
    OSDependent/Unix/ossource.cpp


LOCAL_CPPFLAGS := -Wno-deprecated-declarations
LOCAL_CFLAGS := -DUSE_FILE32API -Wno-deprecated-declarations

LOCAL_CPP_EXTENSION := .cc .cpp
LOCAL_CFLAGS += -x c++

LOCAL_EXPORT_CFLAGS   := -DUSE_FILE32API
LOCAL_EXPORT_CPPFLAGS := -Wno-deprecated-declarations

include $(BUILD_STATIC_LIBRARY)