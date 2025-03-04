#include <jni.h>
#include <string>
#include <ncnn/gpu.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include "YoloV5.h"
#include "YoloV4.h"

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    ncnn::create_gpu_instance();
    if(ncnn::get_gpu_count() > 0){
        YoloV5::hasGPU = true;
        YoloV4::hasGPU = true;
    }
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    ncnn::destroy_gpu_instance();
}

extern "C" JNIEXPORT void JNICALL
Java_com_wzt_yolov5_YOLOv5_init(JNIEnv* env, jclass, jobject assetManager) {
    if(YoloV5::detector == nullptr){
        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        YoloV5::detector = new YoloV5(mgr,"yolov5.param","yolov5.bin");
    }
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_wzt_yolov5_YOLOv5_detect(JNIEnv* env, jclass, jobject image, jdouble threshold, jdouble nms_threshold) {
    auto result = YoloV5::detector->detect(env,image,threshold,nms_threshold);

    auto box_cls = env->FindClass("com/wzt/yolov5/Box");
    auto cid = env->GetMethodID(box_cls, "<init>", "(FFFFIF)V");
    jobjectArray ret = env->NewObjectArray( result.size(), box_cls, nullptr);
    int i = 0;
    for(auto& box:result){
        env->PushLocalFrame(1);
        jobject obj = env->NewObject(box_cls, cid,box.x1,box.y1,box.x2,box.y2,box.label,box.score);
        obj = env->PopLocalFrame(obj);
        env->SetObjectArrayElement( ret, i++, obj);
    }
    return ret;
}

/*********************************************************************************************
                                         YOLOv4-tiny
 yolov4官方ncnn模型下载地址
 darknet2ncnn:https://drive.google.com/drive/folders/1YzILvh0SKQPS_lrb33dmGNq7aVTKPWS0
 ********************************************************************************************/

// 20200813 增加 MobileNetV2-YOLOv3-Nano-coco

extern "C" JNIEXPORT void JNICALL
Java_com_wzt_yolov5_YOLOv4_init(JNIEnv* env, jclass, jobject assetManager, jboolean v4tiny) {
    if (YoloV4::detector != nullptr) {
        delete YoloV4::detector;
        YoloV4::detector = nullptr;
    }
    if(YoloV4::detector == nullptr){
        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        if (v4tiny == 1) {
            YoloV4::detector = new YoloV4(mgr,"yolov4-tiny-opt.param","yolov4-tiny-opt.bin");
        } else if (v4tiny == 0) {
            YoloV4::detector = new YoloV4(mgr,"MobileNetV2-YOLOv3-Nano-coco.param","MobileNetV2-YOLOv3-Nano-coco.bin");
        }
    }
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_wzt_yolov5_YOLOv4_detect(JNIEnv* env, jclass, jobject image, jdouble threshold, jdouble nms_threshold) {
    auto result = YoloV4::detector->detect(env,image,threshold,nms_threshold);

    auto box_cls = env->FindClass("com/wzt/yolov5/Box");
    auto cid = env->GetMethodID(box_cls, "<init>", "(FFFFIF)V");
    jobjectArray ret = env->NewObjectArray( result.size(), box_cls, nullptr);
    int i = 0;
    for(auto& box:result){
        env->PushLocalFrame(1);
        jobject obj = env->NewObject(box_cls, cid,box.x1,box.y1,box.x2,box.y2,box.label,box.score);
        obj = env->PopLocalFrame(obj);
        env->SetObjectArrayElement( ret, i++, obj);
    }
    return ret;
}