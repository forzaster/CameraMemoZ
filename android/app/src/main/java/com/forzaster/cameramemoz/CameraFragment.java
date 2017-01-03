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

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        ViewGroup view = (ViewGroup)inflater.inflate(R.layout.camera_fragment, null);
        Activity activity = getActivity();
        TextureView textureView = (TextureView)view.findViewById(R.id.texture);
        if (textureView.getVisibility() == View.VISIBLE) {
            Log.d(TAG, "Visible!!");
        }
        mCamera = new Camera(activity.getApplicationContext(), textureView);
        mCamera.init(activity);
        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

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
