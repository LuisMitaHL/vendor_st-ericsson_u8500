/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import java.util.HashSet;

import android.content.Context;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.ServiceManager;
import android.os.PowerManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.view.IWindowManager;
import android.view.Display;
import android.hardware.input.InputManager;
import android.hardware.input.IInputManager;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;
import com.stericsson.atservice.CmeErrorCodeConstants;

/**
 * The CtsaCommandHandler handles the AT+CTSA command that emulates touch screen events.
 * The top left corner of the screen is defined as the 0,0 coordinate point regardless of display mode (portrait or landscape).
 * All coordinate values are non-negative integers.
 *
 * This command should be accepted (OK returned) before actually emulating the touch screen action.
 * <p>
 * Possible alternatives are AT+CTSA=<action><x><y> and AT+CTSA=? , the latter returns valid parameter settings.
 *
 * <action>:
*  0  Release. Emulates the user releasing the touch screen at the <x>, <y> coordinates.
*  1  Depress. Emulates the user depressing the touch screen at location <x>, <y>.
*     NOTE: Consecutive Depress actions will emulate dragging a stylus on the touch device to the new location.
*  2  Single Tap. Emulates the user performing a single tap action at the <x>, <y> location.
*     The timing required to emulate a single tap shall be handled by the mobile equipment.
*     NOTE: Duration of a single tap is defined by the constant SINGLE_TAP_TIMING.
*  3  Double Tap. Emulates the user performing a double tap action at the <x>, <y> location.
*     The timing required to emulate a double tap shall be handled by the mobile equipment.
*     NOTE: Time between the taps is defined by the constant DOUBLE_TAP_TIMING.
*100  A non-standard action extension allowing application to acquire a wakelock to keep system alive.
*     Normally the platform enters a sleep mode with disabled screen. In this mode the platform ignores
*     any touch screen events.
*101  A non-standard action extension allowing application to remove a previously acquired wakelock and return
*     to normal platform mode.
*
* <x> :
*     The horizontal x coordinate location of the action performed on the touch screen.
*     Supported x-ccords are  0 .. Xmax where Xmax depends on device orientation (portrait or landscape).
* <y>:
*     The vertical y coordinate location of the action performed on the touch screen.
*     Supported y-ccords are  0 .. Ymax where Ymax depends on device orientation (portrait or landscape).
 */
public class CtsaCommandHandler extends AtCommandHandler
{

    private TouchEventThread touchThread;               // A thread used to handle simulated touch events..

    private int     action;                      // The action
    private int     caction;                     // Internally coded action
    private int     xcoord;                      //The X coordinate
    private int     ycoord;                      // The Y coordinate
    private long    timeEventStarted;            // Time when event started
    private boolean isDepressed;                 // State depressed or not
    private long    timeDepressStarted;          // Time when last sequence starting with a Depress started
    private         PowerManager pm;             // Used to keep display active using a wake lock
    private         PowerManager.WakeLock wl;    // - " -
    private boolean hasTouchEventsToSimulate;    // Flag defining if any touch events to perform or not

    /**
     * Creates a CtsaCommandHandler, that handles the AT+CTSA command.
     *
     * @param context the context this handler was created in
     * @param atParser the AT parser this handler belong to
    */
    public CtsaCommandHandler(Context context, AtParser atParser)
    {
        super(context, atParser);
        commandName = "+CTSA";
        HashSet<Object> allowedValuesArg1 = new HashSet<Object>();
        allowedValuesArg1.add(new Integer("0"));
        allowedValuesArg1.add(new Integer("1"));
        allowedValuesArg1.add(new Integer("2"));
        allowedValuesArg1.add(new Integer("3"));
        allowedValuesArg1.add(new Integer("100"));
        allowedValuesArg1.add(new Integer("101"));

        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(true, null, allowedValuesArg1, true), // isInteger, no default, Supported values are 0-3, specified values are allowed
            new AtArgumentProperties(true, null, null             , false), // isInteger, no default, No defined valueset, NO specified values
            new AtArgumentProperties(true, null, null             , false), // isInteger, no default, No defined valueset, NO specified values
        };

        this.isDepressed = false;  // Start with released state
        this.pm          = (PowerManager) context.getSystemService(Context.POWER_SERVICE);

        if (this.pm != null) {
            this.wl = this.pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK |
                                          PowerManager.ACQUIRE_CAUSES_WAKEUP   |
                                          PowerManager.ON_AFTER_RELEASE,
                                          "Executing AT+CTSA sequences");
        }

        if (this.wl == null) {
            Log.e(AtService.LOG_TAG, "CtsaCommandHandler FAILED acquiring a wake lock.");
        }
    }

    /**
     * Handle the set command AT+CTSA=<action><x><y>
     * <p>
    */
    public AtCommandResponse handleSetCommand()
    {
        if (!checkArgumentsValidSetDefault()) {
            Log.e(AtService.LOG_TAG, "Arguments to " + commandName + " is invalid");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        Object[] arguments = atCommand.getArguments();

        if (arguments.length != argumentProperties.length || arguments[0] == null || arguments[1] == null || arguments[2] == null ) {
            Log.e(AtService.LOG_TAG, "Mandatory argument to " + commandName + " is missing");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        action = (Integer) arguments[0];
        xcoord = (Integer) arguments[1];
        ycoord = (Integer) arguments[2];

        // Get xmax/ymax
        DisplayMetrics dm = context.getResources().getDisplayMetrics();
        int width         = dm.widthPixels;
        int height        = dm.heightPixels;

        // xcoord should be a value 0..Xmax (depends on device orientation)
        if (xcoord < 0 || xcoord > width) {
            Log.e(AtService.LOG_TAG, "The x-coordinate is outside the valid interval");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
        }

        // ycoord should be a value 0..Ymax (depends on device orientation)
        if (ycoord < 0 || ycoord > height) {
            Log.e(AtService.LOG_TAG, "The y-coordinate is outside the valid interval");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
        }

        hasTouchEventsToSimulate = true;    // Assume there are events to handle

        switch (action) {
        case 0:

            // Emulates the user releasing the touch screen at the given <x>, <y> coordinates, if state already is Release then issue error
            if (!isDepressed) {
                Log.e(AtService.LOG_TAG, "Release action can't be given when already released.");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
            }

            caction          = 0;
            timeEventStarted = SystemClock.uptimeMillis();
            isDepressed      = false;
            break;

        case 1:

            // Emulates the user depressing the touch screen at the given <x>, <y> coordinates.
            // Consecutive Depress actions will emulate dragging a stylus on the touch device to the new location.
            // If this is start of a new Depress action, then pickup current time and keep
            if (!isDepressed) {
                caction            = 1;
                timeDepressStarted = SystemClock.uptimeMillis();
                timeEventStarted   = timeDepressStarted;
                isDepressed        = true;
            } else {
                caction            = 4;
                timeEventStarted   = SystemClock.uptimeMillis();
            }

            break;

        case 2:

            // Single Tap. Emulates the user performing a single tap action at the given <x>, <y> location.
            if (isDepressed) {
                Log.e(AtService.LOG_TAG, "Tap action can't be given while already depressed.");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
            }

            caction            = 2;
            timeDepressStarted = SystemClock.uptimeMillis();
            timeEventStarted   = timeDepressStarted;
            break;

        case 3:

            // Double Tap. Emulates the user performing a double tap action at the given <x>, <y> location.
            if (isDepressed) {
                Log.e(AtService.LOG_TAG, "Double tap action can't be given while already depressed.");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
            }

            caction            = 3;
            timeDepressStarted = SystemClock.uptimeMillis();
            timeEventStarted   = timeDepressStarted;
            break;

        case 100:

            if (wl != null) {
                wl.acquire();
                Log.i(AtService.LOG_TAG, "AT+CTSA acquiring a wake lock.");
            } else {
                Log.e(AtService.LOG_TAG, "AT+CTSA FAILED trying to acquire a wake lock.");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
            }

            hasTouchEventsToSimulate = false;
            break;

        case 101:

            if (wl != null) {
                wl.release();
                Log.i(AtService.LOG_TAG, "AT+CTSA releasing a wake lock.");
            } else {
                Log.e(AtService.LOG_TAG, "AT+CTSA FAILED trying to release a wake lock.");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
            }

            hasTouchEventsToSimulate = false;
            break;

        default:
            Log.e(AtService.LOG_TAG, "Unknown action");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
        }

        if (hasTouchEventsToSimulate) {
            // Create thread to handle touch event
            touchThread = new TouchEventThread(caction, xcoord, ycoord, timeEventStarted, timeDepressStarted);
            touchThread.start();
        }

        return new AtCommandResponse(AtCommandResponse.OK);
    }

    // Handles the test command AT+CTSA=?  Returns supported actions
    public AtCommandResponse handleTestCommand()
    {
        // Get xmax/ymax
        DisplayMetrics dm = context.getResources().getDisplayMetrics();
        String str        = "+CTSA: (0-3,100-101),(0-" + dm.widthPixels + "),(0-" + dm.heightPixels + ")";

        return new AtCommandResponse(AtCommandResponse.OK, str);
    }

    // Handles touch events  for a +CTSA command.
    private class TouchEventThread extends Thread
    {
        private static final int EVENT_DELAY = 100;         // Timing used for movement before lifting stylus in msec
        private static final int SINGLE_TAP_TIMING = 100;   // Timing used for emulating a single tap in msec
        private static final int DOUBLE_TAP_TIMING = 300;   // Timing used for emulating a double tap in msec

        private int  action;               // Touch action to perform
        private int  xcoord;               // X coordinate to use
        private int  ycoord;               // Y coordinate to use
        private int  event;                // Touch event
        private long timeEventStarted;     // Time when this event started
        private long timeDepressStarted;   // Time when recent depress started

        /**
        * Constructor for the TouchEventThread.
        *
        * @param action               the touch event action
        * @param xcoord               the x coordinate
        * @param ycoord               the y coordinate
        * @param timeEventStarted     time for event start
        * @param timeDepressStarted   initial sequence start (Depress)
        */
        public TouchEventThread(int action, int xcoord, int ycoord, long timeEventStarted, long timeDepressStarted)
        {
            this.action              = action;
            this.xcoord              = xcoord;
            this.ycoord              = ycoord;
            this.timeEventStarted    = timeEventStarted;
            this.timeDepressStarted  = timeDepressStarted;
        }

        // Handles the touch event.
        public void run()
        {

            switch (action) {
            case 0: // Emulates the user releasing the touch screen at the given <x>, <y> coordinates.
                executeTouchEvent(MotionEvent.ACTION_MOVE, xcoord, ycoord, timeEventStarted              , timeDepressStarted);
                executeTouchEvent(MotionEvent.ACTION_UP  , xcoord, ycoord, timeEventStarted + EVENT_DELAY, timeDepressStarted);
                break;

            case 1: // Emulates the user depressing the touch screen at the given <x>, <y> coordinates.
                executeTouchEvent(MotionEvent.ACTION_DOWN, xcoord, ycoord, timeEventStarted, timeDepressStarted);
                break;

            case 2: // Single Tap. Emulates the user performing a single tap action at the given <x>, <y> location.
                executeTouchEvent(MotionEvent.ACTION_DOWN, xcoord, ycoord, timeEventStarted                    , timeDepressStarted);
                executeTouchEvent(MotionEvent.ACTION_UP  , xcoord, ycoord, timeEventStarted + SINGLE_TAP_TIMING, timeDepressStarted);
                break;

            case 3: // Double Tap. Emulates the user performing a double tap action at the given <x>, <y> location.
                executeTouchEvent(MotionEvent.ACTION_DOWN, xcoord, ycoord, timeEventStarted                                                            , timeDepressStarted);
                executeTouchEvent(MotionEvent.ACTION_UP  , xcoord, ycoord, timeEventStarted + SINGLE_TAP_TIMING                                        , timeDepressStarted);
                executeTouchEvent(MotionEvent.ACTION_DOWN, xcoord, ycoord, timeEventStarted + SINGLE_TAP_TIMING + DOUBLE_TAP_TIMING                    , timeDepressStarted);
                executeTouchEvent(MotionEvent.ACTION_UP  , xcoord, ycoord, timeEventStarted + SINGLE_TAP_TIMING + DOUBLE_TAP_TIMING + SINGLE_TAP_TIMING, timeDepressStarted);
                break;

            case 4: // Consecutive Depress actions will emulate dragging a stylus on the touch device to the new location.
                executeTouchEvent(MotionEvent.ACTION_MOVE, xcoord, ycoord, timeEventStarted, timeDepressStarted);
                break;

            default:
                Log.e(AtService.LOG_TAG, "Unknown action");
            }
        }

        //  Executes one touch event
        private void executeTouchEvent(int eventCode, int xcoord, int ycoord, long timeEventStarted, long timeDepressStarted)
        {
            MotionEvent me = MotionEvent.obtain(timeDepressStarted, timeEventStarted, eventCode, xcoord, ycoord, 0);

            InputManager.getInstance().injectInputEvent(me,
                InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
        }

    }
}
