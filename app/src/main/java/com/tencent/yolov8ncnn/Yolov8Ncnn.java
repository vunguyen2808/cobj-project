package com.tencent.yolov8ncnn;

import android.content.res.AssetManager;
import android.view.Surface;

public class Yolov8Ncnn
{
    public native boolean loadModel(AssetManager mgr, int modelid);
    public native boolean openCamera(int facing);
    public native boolean closeCamera();
    public native boolean setOutputWindow(Surface surface);

    public native boolean takeScreenshot(String filepath);

    static {
        System.loadLibrary("yolov8ncnn");
    }
}
