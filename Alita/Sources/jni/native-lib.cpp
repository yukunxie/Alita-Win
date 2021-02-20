#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include<android/native_window_jni.h>

#include "../Render/RealRenderer.h"
#include "../aux/AFileSystem.h"

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_realxie_alita_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */)
{
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_realxie_alita_MainActivity_setAssetManagerJNI(JNIEnv *env, jobject /*thiz*/,
                                                          jobject context, jobject assetManager)
{
    AFileSystem::getInstance()->initAssetManager(AAssetManager_fromJava(env, assetManager));
}

JNIEXPORT void JNICALL
Java_com_realxie_alita_MainActivity_initVulkanContextJNI(JNIEnv *env, jobject /*thiz*/, jobject surface)
{
    if (!RealRenderer::getInstance()->isReady())
    {
        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        RealRenderer::getInstance()->initVulkanContext(window);
    }
}

JNIEXPORT void JNICALL
Java_com_realxie_alita_MainActivity_nativeRenderJNI(JNIEnv *env, jobject /*thiz*/, jfloat dt)
{
    if (RealRenderer::getInstance()->isReady())
    {
        RealRenderer::getInstance()->drawFrame();
    }
}

}
