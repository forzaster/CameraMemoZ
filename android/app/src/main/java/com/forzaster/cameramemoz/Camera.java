package com.forzaster.cameramemoz;

import android.content.Context;
import android.hardware.camera2.CameraManager;

/**
 * Created by n-naka on 2016/12/29.
 */

public class Camera {
    private final Context mContext;
    private final CameraManager mCameraManager;

    public Camera(Context context) {
        mContext = context;
        mCameraManager = (CameraManager)mContext.getSystemService(Context.CAMERA_SERVICE);
    }

    public void init() {


    }
}
