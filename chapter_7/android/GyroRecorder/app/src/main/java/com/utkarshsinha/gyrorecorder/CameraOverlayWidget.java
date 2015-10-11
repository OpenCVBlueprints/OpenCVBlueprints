package com.utkarshsinha.gyrorecorder;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Button;

/**
 * Created by usinha on 9/7/15.
 */
public class CameraOverlayWidget extends SurfaceView implements GestureDetector.OnGestureListener, SurfaceHolder.Callback {
    public static String TAG= "SFOCV::Overlay";
    protected Paint paintSafeExtents;
    protected Button btn;
    protected GestureDetector mGestureDetector;

    private long sizeWidth = 0, sizeHeight = 0;

    // Stuff required to paint the recording sign
    protected boolean mRecording = false;
    protected Paint paintRecordCircle;
    protected Paint paintRecordText;

    // Record button
    private Paint paintCalibrateText;
    private Paint paintCalibrateTextOutline;

    private Paint paintTransparentButton;

    private RenderThread mPainterThread;
    private boolean bStopPainting = false;

    private Point3 omega;
    private Point3 drift;
    private Point3 theta;

    public static final double SAFETY_HORIZONTAL = 0.15;
    public static final double SAFETY_VERTICAL = 0.15;

    protected GestureDetector.OnGestureListener mCustomTouchMethods = null;
    protected OverlayEventListener mOverlayEventListener = null;

    public CameraOverlayWidget(Context ctx, AttributeSet attrs) {
        super(ctx, attrs);

        // Position at the very top and I'm the event handler
        setZOrderOnTop(true);
        getHolder().addCallback(this);

        // Load all the required objects
        initializePaints();

        // Setup the required handlers/threads
        mPainterThread = new RenderThread();
        mGestureDetector = new GestureDetector(ctx, this);
    }

    /**
     * Initializes all paint objects.
     */
    protected void initializePaints() {
        paintSafeExtents = new Paint();
        paintSafeExtents.setColor(Color.WHITE);
        paintSafeExtents.setStyle(Paint.Style.STROKE);
        paintSafeExtents.setStrokeWidth(3);

        paintRecordCircle = new Paint();
        paintRecordCircle.setColor(Color.RED);
        paintRecordCircle.setStyle(Paint.Style.FILL);

        paintRecordText = new Paint();
        paintRecordText.setColor(Color.WHITE);
        paintRecordText.setTextSize(20);

        paintCalibrateText = new Paint();
        paintCalibrateText.setColor(Color.WHITE);
        paintCalibrateText.setTextSize(35);
        paintCalibrateText.setStyle(Paint.Style.FILL);

        paintCalibrateTextOutline = new Paint();
        paintCalibrateTextOutline.setColor(Color.BLACK);
        paintCalibrateTextOutline.setStrokeWidth(2);
        paintCalibrateTextOutline.setTextSize(35);
        paintCalibrateTextOutline.setStyle(Paint.Style.STROKE);

        paintTransparentButton = new Paint();
        paintTransparentButton.setColor(Color.BLACK);
        paintTransparentButton.setAlpha(128);
        paintTransparentButton.setStyle(Paint.Style.FILL);
    }



    public void setRecording() {
        mRecording = true;
    }

    public void unsetRecording() {
        mRecording = false;
    }

    @Override
    public void onSizeChanged(int w,int h,int oldw,int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        sizeWidth = w;
        sizeHeight = h;
    }

    public void setCustomTouchMethods(GestureDetector.SimpleOnGestureListener c){
        mCustomTouchMethods = c;
    }

    public void setOverlayEventListener(OverlayEventListener listener) {
        mOverlayEventListener = listener;
    }

    public void setOmega(Point3 omega) {
        this.omega = omega;
    }

    public void setDrift(Point3 drift) {
        this.drift = drift;
    }

    public void setTheta(Point3 theta) {
        this.theta = theta;
    }

    /**
     * This method is called during the activity's onResume. This ensures a wakeup
     * re-instantiates the rendering thread.
     */
    public void resume() {
        bStopPainting = false;
        mPainterThread = new RenderThread();
    }

    /**
     * This method is called during the activity's onPause method. This ensures
     * going to sleep pauses the rendering.
     */
    public void pause() {
        bStopPainting = true;

        try {
            mPainterThread.join();
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }
        mPainterThread = null;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        getHolder().setFormat(PixelFormat.RGBA_8888);

        // We created the thread earlier - but we should start it only when
        // the surface is ready to be drawn on.
        if(mPainterThread != null && !mPainterThread.isAlive()) {
            mPainterThread.start();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // Required for implementation
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // Required for implementation
    }

    @Override
    public boolean onTouchEvent(MotionEvent motionEvent) {
        boolean result = mGestureDetector.onTouchEvent(
                motionEvent);
        return result;
    }

    @Override
    public boolean onDown(MotionEvent motionEvent) {
        MotionEvent.PointerCoords coords =
                new MotionEvent.PointerCoords();

        motionEvent.getPointerCoords(0, coords);

        // Handle these only if there is an event listener
        if(mOverlayEventListener!=null) {
            if(coords.x >= (1-SAFETY_HORIZONTAL)*sizeWidth &&
                    coords.x<sizeWidth &&
                    coords.y >= (1-SAFETY_VERTICAL)*sizeHeight &&
                    coords.y<sizeHeight) {
                return mOverlayEventListener.onCalibrate(
                        motionEvent);
            }
        }

        // Didn't match? Try passing a raw event - just in case
        if(mCustomTouchMethods!=null)
            return mCustomTouchMethods.onDown(motionEvent);

        // Nothing worked - let it bubble up
        return false;
    }

    @Override
    public void onShowPress(MotionEvent motionEvent) {
        if(mCustomTouchMethods!=null)
            mCustomTouchMethods.onShowPress(motionEvent);
    }

    @Override
    public boolean onFling(MotionEvent motionEvent,
                           MotionEvent motionEvent2,
                           float v, float v2) {
        Log.d(TAG, "onFling");

        if(mCustomTouchMethods!=null)
            return mCustomTouchMethods.onFling(motionEvent,
                    motionEvent2,
                    v, v2);

        return false;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
        Log.d(TAG, "onLongPress");

        if(mCustomTouchMethods!=null)
            mCustomTouchMethods.onLongPress(motionEvent);
    }

    @Override
    public boolean onScroll(MotionEvent motionEvent,
                            MotionEvent motionEvent2,
                            float v, float v2) {
        Log.d(TAG, "onScroll");

        if(mCustomTouchMethods!=null)
            return mCustomTouchMethods.onScroll(motionEvent,
                    motionEvent2,
                    v, v2);

        return false;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
        Log.d(TAG, "onSingleTapUp");

        if(mCustomTouchMethods!=null)
            return mCustomTouchMethods.onSingleTapUp(motionEvent);

        return false;
    }

    class RenderThread extends Thread {
        private long start = 0;
        @Override
        public void run() {
            super.run();

            start = SystemClock.uptimeMillis();

            while(!bStopPainting && !isInterrupted()) {
                long tick = SystemClock.uptimeMillis();
                renderOverlay(tick);
            }
        }

        /**
         * A renderer for the overlay with no state of its own.
         * @returns nothing
         */
        public void renderOverlay(long tick) {
            Canvas canvas = getHolder().lockCanvas();

            long width = canvas.getWidth();
            long height = canvas.getHeight();

            // Clear the canvas
            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);

            // Draw the bounds
            long lSafeW = (long)(width * SAFETY_HORIZONTAL);
            long lSafeH = (long)(height * SAFETY_VERTICAL);
            canvas.drawRect(lSafeW, lSafeH, width - lSafeW, height - lSafeH, paintSafeExtents);

            if(mRecording) {
                // Render this only on alternate 500ms intervals
                if(((tick-start) / 500) % 2 == 1) {
                    canvas.drawCircle(100, 100, 20, paintRecordCircle);
                    final String s = "Recording";
                    canvas.drawText(s, 0, s.length(), 130, 110, paintRecordText);
                }
            }

            canvas.drawRect((float) (1 - SAFETY_HORIZONTAL) * sizeWidth, (float) (1 - SAFETY_VERTICAL) * sizeHeight, sizeWidth, sizeHeight, paintTransparentButton);

            final String strCalibrate = "Calibrate";
            canvas.drawText(strCalibrate, 0, strCalibrate.length(), width - 200, height - 200, paintCalibrateText);
            canvas.drawText(strCalibrate, 0, strCalibrate.length(), width - 200, height - 200, paintCalibrateTextOutline);

            if(omega!=null) {
                final String strO = "O: ";
                canvas.drawText(strO, 0, strO.length(), width - 200, 200, paintCalibrateText);
                String strX = Math.toDegrees(omega.x) + "";
                String strY = Math.toDegrees(omega.y) + "";
                String strZ = Math.toDegrees(omega.z) + "";
                canvas.drawText(strX, 0, strX.length(), width - 200, 250, paintCalibrateText);
                canvas.drawText(strY, 0, strY.length(), width - 200, 300, paintCalibrateText);
                canvas.drawText(strZ, 0, strZ.length(), width - 200, 350, paintCalibrateText);
            }

            if(theta!=null) {
                final String strT = "T: ";
                canvas.drawText(strT, 0, strT.length(), width - 200, 500, paintCalibrateText);
                String strX = Math.toDegrees(theta.x) + "";
                String strY = Math.toDegrees(theta.y) + "";
                String strZ = Math.toDegrees(theta.z) + "";
                canvas.drawText(strX, 0, strX.length(), width - 200, 550, paintCalibrateText);
                canvas.drawText(strY, 0, strY.length(), width - 200, 600, paintCalibrateText);
                canvas.drawText(strZ, 0, strZ.length(), width - 200, 650, paintCalibrateText);
            }
            // Flush out the canvas
            getHolder().unlockCanvasAndPost(canvas);
        }
    }

    public interface OverlayEventListener {
        public boolean onCalibrate(MotionEvent e);
    }
}
