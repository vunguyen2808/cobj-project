package com.tencent.yolov8ncnn;

import android.Manifest;
import android.app.Activity;
import android.content.ContentValues;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.icu.text.SimpleDateFormat;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.util.Date;
import java.util.Locale;

public class MainActivity extends Activity implements SurfaceHolder.Callback {
    public static final int REQUEST_CAMERA = 100;
    private final Yolov8Ncnn yolov8ncnn = new Yolov8Ncnn();
    private int current_model = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        SurfaceView cameraView = findViewById(R.id.cameraview);

        cameraView.getHolder().setFormat(PixelFormat.RGBA_8888);
        cameraView.getHolder().addCallback(this);

        Button buttonSwitchCamera = findViewById(R.id.buttonSwitchCamera);
        buttonSwitchCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                saveImageToMediaStore();
            }

            public void saveImageToMediaStore() {
                SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault());
                String currentTime = sdf.format(new Date());

                File tempFile = new File(getCacheDir(), "temp_screenshot_" + currentTime + ".jpg");
                boolean success = yolov8ncnn.takeScreenshot(tempFile.getAbsolutePath());

                if (success) {
                    ContentValues values = new ContentValues();
                    values.put(MediaStore.Images.Media.TITLE, "screenshot_" + currentTime);
                    values.put(MediaStore.Images.Media.DISPLAY_NAME, "screenshot_" + currentTime + ".jpg");
                    values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
                    values.put(MediaStore.Images.Media.RELATIVE_PATH, Environment.DIRECTORY_PICTURES + "/Pipe");

                    Uri uri = getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);

                    try (OutputStream os = getContentResolver().openOutputStream(uri);
                         InputStream is = Files.newInputStream(tempFile.toPath())) {
                        byte[] buffer = new byte[4 * 1024]; // 4KB
                        int read;
                        while ((read = is.read(buffer)) != -1) {
                            os.write(buffer, 0, read);
                        }
                        os.flush();
                        Toast.makeText(MainActivity.this, "Screenshot saved!", Toast.LENGTH_SHORT).show();
                    } catch (IOException e) {
                        e.printStackTrace();
                        Toast.makeText(MainActivity.this, "Failed to save screenshot!", Toast.LENGTH_SHORT).show();
                    }

                    tempFile.delete();
                } else {
                    Toast.makeText(MainActivity.this, "Failed to take screenshot!", Toast.LENGTH_SHORT).show();
                }
            }
        });

        Spinner spinnerModel = findViewById(R.id.spinnerModel);
        spinnerModel.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> arg0, View arg1, int position, long id) {
                if (position != current_model) {
                    current_model = position;
                    reload();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> arg0) {
            }
        });

        reload();
    }

    private void reload() {
        boolean ret_init = yolov8ncnn.loadModel(getAssets(), current_model);
        if (!ret_init) {
            Log.e("MainActivity", "yolov8ncnn loadModel failed");
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        yolov8ncnn.setOutputWindow(holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    @Override
    public void onResume() {
        super.onResume();

        if (ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, REQUEST_CAMERA);
        }

        int facing = 1;
        yolov8ncnn.openCamera(facing);
    }

    @Override
    public void onPause() {
        super.onPause();
        yolov8ncnn.closeCamera();
    }

}

