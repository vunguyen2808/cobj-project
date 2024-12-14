#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <android/log.h>

#include <jni.h>

#include <string>
#include <vector>

#include <platform.h>
#include <benchmark.h>

#include "yolo.h"

#include "ndkcamera.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>

#if __ARM_NEON
#endif

static int draw_unsupported(cv::Mat &rgb) {
    const char text[] = "unsupported";

    int baseLine = 0;
    cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 1.0, 1, &baseLine);

    int y = (rgb.rows - label_size.height) / 2;
    int x = (rgb.cols - label_size.width) / 2;

    cv::rectangle(rgb, cv::Rect(cv::Point(x, y),
                                cv::Size(label_size.width, label_size.height + baseLine)),
                  cv::Scalar(255, 255, 255), -1);

    cv::putText(rgb, text, cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0));

    return 0;
}

static Yolo *g_yolo = 0;
static ncnn::Mutex lock;

class MyNdkCamera : public NdkCameraWindow {
public:
    virtual void on_image_render(cv::Mat &rgb) const;

    cv::Mat getLastRenderedImage() const;

private:
    mutable cv::Mat lastRenderedImage;
};

cv::Mat MyNdkCamera::getLastRenderedImage() const {
    return lastRenderedImage.clone();
}

void MyNdkCamera::on_image_render(cv::Mat &rgb) const {
    {
        ncnn::MutexLockGuard g(lock);

        if (g_yolo) {
            std::vector <Object> objects;
            g_yolo->detect(rgb, objects);
            g_yolo->draw(rgb, objects);

            char count_text[32];
            sprintf(count_text, "COUNT %zu", objects.size());

            int baseLine = 0;
            cv::Size label_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1,
                                                  &baseLine);

            int y = 20;
            int x = 10;

            cv::rectangle(rgb, cv::Rect(cv::Point(x, y - label_size.height),
                                        cv::Size(label_size.width, label_size.height + baseLine)),
                          cv::Scalar(255, 255, 255), -1);

            cv::putText(rgb, count_text, cv::Point(x, y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        } else {
            draw_unsupported(rgb);
        }
    }

    rgb.copyTo(lastRenderedImage);
}

static MyNdkCamera *g_camera = 0;

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnLoad");

    g_camera = new MyNdkCamera;

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnUnload");

    {
        ncnn::MutexLockGuard g(lock);

        delete g_yolo;
        g_yolo = 0;
    }

    delete g_camera;
    g_camera = 0;
}

JNIEXPORT jboolean JNICALL
Java_com_tencent_yolov8ncnn_Yolov8Ncnn_loadModel(JNIEnv *env, jobject thiz, jobject assetManager,
                                                 jint modelid) {
    if (modelid < 0 || modelid > 6) {
        return JNI_FALSE;
    }

    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "loadModel %p", mgr);

    const char *modeltypes[] =
            {
                    "n",
                    "s",
            };

    const int target_sizes[] =
            {
                    320,
                    320,
            };

    const float mean_vals[][3] =
            {
                    {103.53f, 116.28f, 123.675f},
                    {103.53f, 116.28f, 123.675f},
            };

    const float norm_vals[][3] =
            {
                    {1 / 255.f, 1 / 255.f, 1 / 255.f},
                    {1 / 255.f, 1 / 255.f, 1 / 255.f},
            };

    const char *modeltype = modeltypes[(int) modelid];
    int target_size = target_sizes[(int) modelid];

    {
        ncnn::MutexLockGuard g(lock);

        if (!g_yolo)
            g_yolo = new Yolo;
        g_yolo->load(mgr, modeltype, target_size, mean_vals[(int) modelid],
                     norm_vals[(int) modelid]);
    }

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_tencent_yolov8ncnn_Yolov8Ncnn_openCamera(JNIEnv *env, jobject thiz, jint facing) {
    if (facing < 0 || facing > 1)
        return JNI_FALSE;

    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "openCamera %d", facing);

    g_camera->open((int) facing);

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_tencent_yolov8ncnn_Yolov8Ncnn_closeCamera(JNIEnv *env, jobject thiz) {
    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "closeCamera");

    g_camera->close();

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_tencent_yolov8ncnn_Yolov8Ncnn_setOutputWindow(JNIEnv *env, jobject thiz, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);

    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "setOutputWindow %p", win);

    g_camera->set_window(win);

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_tencent_yolov8ncnn_Yolov8Ncnn_takeScreenshot(JNIEnv *env, jobject thiz, jstring filepath) {
    const char *path = env->GetStringUTFChars(filepath, 0);
    cv::Mat img = g_camera->getLastRenderedImage();

    if (img.empty()) {
        env->ReleaseStringUTFChars(filepath, path);
        return JNI_FALSE;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "takeScreenshot %s", path);
    bool result = cv::imwrite(path, img);
    env->ReleaseStringUTFChars(filepath, path);
    return result ? JNI_TRUE : JNI_FALSE;
}

}
