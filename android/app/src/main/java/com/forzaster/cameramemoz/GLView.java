package com.forzaster.cameramemoz;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by n-naka on 2016/12/29.
 */

public class GLView extends GLSurfaceView implements GLSurfaceView.Renderer {
    public interface IListener {
        void onTextureGenerated(int tex);
        void onPreDrawFrame();
        void onPostDrawFrame();
    }

    private IListener mListener;

    public GLView(Context context) {
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(2);
        setRenderer(this);
    }

    public void setListener(IListener listener) {
        mListener = listener;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLMain.instance().init(width, height);
        if (mListener != null) {
            mListener.onTextureGenerated(GLMain.instance().genTexture());
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (mListener != null) {
            mListener.onPreDrawFrame();
        }
        GLMain.instance().draw();
        if (mListener != null) {
            mListener.onPostDrawFrame();
        }
    }
}
