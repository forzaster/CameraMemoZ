package com.forzaster.cameramemoz;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;

import java.util.Collections;

/**
 * Created by n-naka on 2016/12/29.
 */

public class Camera {
    private static final String TAG = "Camera";
    private final Context mContext;
    private final CameraManager mCameraManager;
    private final TextureView mTextureView;
    private final Handler mBackgroundHandler;
    private final HandlerThread mBackgroundThread;
    private final CameraDevice.StateCallback mDevStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            mDevice = camera;
            createCaptureSession();
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            camera.close();
            mDevice = null;
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            camera.close();
            mDevice = null;
        }
    };
    private final CameraCaptureSession.StateCallback mCapStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession session) {
            mCaptureRequest = mRequestBuilder.build();
            try {
                session.setRepeatingRequest(mCaptureRequest, mCaptureCallback, mBackgroundHandler);
            } catch (CameraAccessException e) {
            }

            mCaptureSession = session;
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            mCaptureSession = null;
        }
    };

    private final CameraCaptureSession.CaptureCallback mCaptureCallback = new CameraCaptureSession.CaptureCallback() {

    };

    private final TextureView.SurfaceTextureListener mTextureListener = new TextureView.SurfaceTextureListener() {

        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            createCaptureSession();
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        }
    };

    private CameraDevice mDevice;
    private CameraCaptureSession mCaptureSession;
    private boolean mIsInitialized;
    private CaptureRequest.Builder mRequestBuilder;
    private CaptureRequest mCaptureRequest;
    private int mTexture = -1;
    private SurfaceTexture mSurfaceTexture;
    private boolean mSurfaceUpdateRequest;
    private Rect mRect;
    private float mAspectRatio;

    public Camera(Context context, TextureView tv) {
        mContext = context;
        mCameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        mTextureView = tv;
        if (mTextureView != null) {
            mTextureView.setSurfaceTextureListener(mTextureListener);
        }

        mBackgroundThread = new HandlerThread("CameraBackgroundThread");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    public void init(Activity activity) {
        if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(activity, new String[]{"android.permission.CAMERA"}, 0);
            return;
        }

        String[] ids;
        try {
            ids = mCameraManager.getCameraIdList();
        } catch (CameraAccessException e) {
            Log.e(TAG, "getCameraIdList error");
            return;
        }
        if (ids == null) {
            return;
        }

        String targetId = null;
        for (String id : ids) {
            CameraCharacteristics characteristics = null;
            try {
                characteristics = mCameraManager.getCameraCharacteristics(id);
            } catch (CameraAccessException e) {
                return;
            }
            if (characteristics.get(CameraCharacteristics.LENS_FACING)
                    == CameraCharacteristics.LENS_FACING_BACK) {
                targetId = id;


                mRect = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
                mAspectRatio = (float)mRect.width()/mRect.height();
                break;
            }
        }
        if (targetId == null) {
            return;
        }

        try {
            mCameraManager.openCamera(targetId, mDevStateCallback, null);
        } catch (CameraAccessException e) {
            return;
        }

        mIsInitialized = true;
        createCaptureSession();
    }

    public void finish() {
        if (mDevice != null) {
            mDevice.close();
            mDevice = null;
        }
        if (mCaptureSession != null) {
            mCaptureSession.close();
            mCaptureSession = null;
        }

        mBackgroundThread.quit();
        try {
            mBackgroundThread.join();
        } catch (InterruptedException e) {
        }
    }

    public void createSurfaceTexture(int w, int h, int tex) {
        mTexture = tex;
        SurfaceTexture surface = new SurfaceTexture(mTexture);
        surface.setDefaultBufferSize(w, h);
        surface.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                synchronized (Camera.this) {
                    mSurfaceUpdateRequest = true;
                }
            }
        });
        synchronized (this) {
            mSurfaceTexture = surface;
        }
        createCaptureSession();
    }

    public void updateTexImage() {
        synchronized (this) {
            if (mSurfaceUpdateRequest && mSurfaceTexture != null) {
                mSurfaceTexture.updateTexImage();
                mSurfaceUpdateRequest = false;
            }
        }
    }

    public float getAspect() {
        return mAspectRatio;
    }

    private void createCaptureSession() {
        if (mDevice == null || !mIsInitialized) {
            return;
        }

        SurfaceTexture texture = null;
        if (mTextureView != null) {
            if (!mTextureView.isAvailable()) {
                return;
            }
            texture = mTextureView.getSurfaceTexture();
            texture.setDefaultBufferSize(mTextureView.getWidth(), mTextureView.getHeight());
            Log.d(TAG, "texture " + mTextureView.getWidth() + "x" + mTextureView.getHeight());
        } else {
            synchronized (this) {
                if (mSurfaceTexture == null) {
                    return;
                }
                texture = mSurfaceTexture;
            }
        }

        try {
            mRequestBuilder = mDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        } catch (CameraAccessException e) {
            return;
        }

        Surface surface = new Surface(texture);
        mRequestBuilder.addTarget(surface);
        try {
            mDevice.createCaptureSession(Collections.singletonList(surface), mCapStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            return;
        }
    }
}
