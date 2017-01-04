package com.forzaster.cameramemoz;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by n-naka on 2017/01/03.
 */

public class CameraFragment extends Fragment {
    public static final String TAG = "CameraFragment";
    private Camera mCamera;
    private GLView mGLView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.camera_fragment, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        /*
        Activity activity = getActivity();
        TextureView textureView = (TextureView)view.findViewById(R.id.texture);
        mCamera = new Camera(activity.getApplicationContext(), textureView);
        mCamera.init(activity);
        */
        Activity activity = getActivity();
        mGLView = new GLView(activity);
        mGLView.setListener(new GLView.IListener() {
            @Override
            public void onTextureGenerated(int tex) {
                Log.d(TAG, "wxh=" + mGLView.getWidth() + "x" + mGLView.getHeight());
                mCamera.createSurfaceTexture(mGLView.getWidth(), mGLView.getHeight(), tex);
            }

            @Override
            public void onPreDrawFrame() {
                mCamera.updateTexImage();
            }

            @Override
            public void onPostDrawFrame() {

            }
        });
        ((ViewGroup)view).addView(mGLView);
        mCamera = new Camera(activity.getApplicationContext(), null);
        mCamera.init(activity);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (mCamera != null) {
            mCamera.finalize();
            mCamera = null;
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
