package com.forzaster.cameramemoz;

/**
 * Created by n-naka on 2016/12/30.
 */

public class GLMain {
    private static GLMain sInstance;

    private GLMain(){
    }

    public static GLMain instance() {
        if (sInstance == null) {
            sInstance = new GLMain();
        }
        return sInstance;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void init(int width, int height);
    public native boolean resize(int width, int height);
    public native void draw();
    public native int genTexture();
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
