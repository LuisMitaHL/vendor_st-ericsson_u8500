/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import android.content.Context;
import android.os.Build;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.ServiceManager;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.IWindowManager;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.hardware.input.InputManager;
import android.hardware.input.IInputManager;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;
import com.stericsson.atservice.CmeErrorCodeConstants;

/**
 * The CkpdCommandHandler handles the AT+CKPD command.
 * <p>
 * Possible alternatives are AT+CKPD=<keys>[,<time>[,<pause>]] and AT+CKPD=?
 */
public class CkpdCommandHandler extends AtCommandHandler {

    /**
     * A thread used to handle key presses.
     */
    private KeyPressThread keyThread;

    /**
     * A code used to signal pause.
     */
    private static final int KEYCODE_PAUSE = 100;

    /**
     * Used to convert pause and press time arguments to milliseconds.
     */
    private static final int FACTOR = 100;

    /**
     * The pause time.
     */
    private static int pauseTime;

    /**
     * The key press time.
     */
    private static int pressTime;

    /**
     * Creates a CkpdCommandHandler, that handles the AT+CKPD command.
     *
     * @param context the context this handler was created in
     * @param atParser the AT parser this handler belong to
     */
    public CkpdCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "+CKPD";
        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(false, null, null, false),
            new AtArgumentProperties(true, new Integer(1),
                                     // use 0.1 s as default press time
                                     null, false), new AtArgumentProperties(true, new Integer(1),
                                             // use 0.1 s as default pause time
                                             null, false),
        };
    }

    /**
     * Handle the set command AT+CKPD=<keys>[,<time>[,<pause>]].
     * <p>
     * Values C/c E/e P/p S/s [ ] has value defined by android API but nothing
     * happens when these keys are pressed.
     */
    public AtCommandResponse handleSetCommand() {
        if (!checkArgumentsValidSetDefault()) {
            Log.d(AtService.LOG_TAG, "Arguments to " + commandName + " is invalid");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        Object[] arguments = atCommand.getArguments();
        String keys = (String) arguments[0];
        // remove "
        keys = keys.substring(1, keys.length() - 1);

        if (!validateSyntax(keys)) {
            Log.d(AtService.LOG_TAG, "Syntax of arguments to " + commandName
                  + " is invalid. Arguments: " + keys);
            return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                         CmeErrorCodeConstants.INVALID_CHARACTERS_IN_TEXT_STRING);
        }

        // press time should be a value between 0..255
        int value = (Integer) arguments[1];

        if (value < 0 || value > 255) {
            Log.d(AtService.LOG_TAG, "The key press time is not valid, ouside the intevall 0..255");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.OPERATION_NOT_ALLOWED);

        }

        // save press time
        pressTime = value * FACTOR;

        // pause time should be a value between 0..255
        value = (Integer) arguments[2];
        if (value < 0 || value > 255) {
            Log.d(AtService.LOG_TAG, "The pause time is not valid, ouside the intevall 0..255");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.OPERATION_NOT_ALLOWED);

        }
        // save pause time
        pauseTime = (Integer) value * FACTOR;
        // create thread to handle key presses
        keyThread = new KeyPressThread(keys);
        keyThread.start();
        return new AtCommandResponse(AtCommandResponse.OK);
    }

    /**
     * Handles the test command AT+CKPD=?. Returns OK to indicate that the
     * command is supported.
     */
    public AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK);
    }

    /**
     * Handles key presses for a +CKPD command.
     */
    private class KeyPressThread extends Thread {

        /**
         * The string of keys to press.
         */
        private String keys;

        /**
         * The earliest time a new key should be pressed. Used to handle pauses.
         */
        private long timeForNextKeyPress;

        /**
         * The running OS version of Android.
         */
        private String androidOS = Build.VERSION.RELEASE;

        /**
         * Version number for Android release ICS.
         */
        private static final String ICS_RELEASE = "4";

        /**
         * Constructor for the KeyPressThread.
         *
         * @param keys the string of keys
         */
        public KeyPressThread(String keys) {
            this.keys = keys;
            timeForNextKeyPress = SystemClock.uptimeMillis();
        }

        /**
         * Handle all the key presses.
         */
        public void run() {
            int index = 0;
            char ch;
            // Keyevent containing keyCode and modifying keycode
            int[] keyEvent = {0,0};

            while (index < keys.length()) {
                ch = keys.charAt(index);

                if (ch == ';') {
                    // in alpha entry
                    int endIndex = getEndIndexAlphaEntry(keys, index);

                    if (endIndex > 1) {
                        for (int i = index + 1; i < endIndex; i++) {
                            char alphaEntryChar = keys.charAt(i);
                            // If double semicolons exists inside an alpha entry,
                            // advance and only print one of them
                            if (alphaEntryChar == ';') {
                                i++;
                            }
                            keyEvent = getCodeAlphaNumericEntry(alphaEntryChar);
                            executeKeyPress(keyEvent);
                        }

                        index = endIndex;
                    }
                } else {
                    // normal character
                    keyEvent[0] = getCode(ch);
                    executeKeyPress(keyEvent);
                }

                index++;
            }
        }

        /**
         * Executes a key press.
         *
         * @param keyCode the code of the key to press.
         */
        private void executeKeyPress(int[] keyEvent) {
            if (keyEvent[0] == KEYCODE_PAUSE) {
                timeForNextKeyPress = SystemClock.uptimeMillis() + pauseTime;
            } else if ((androidOS.compareTo(ICS_RELEASE) >= 0) &&
                       (keyEvent[0] == KeyEvent.KEYCODE_CALL)) {
                simulateSoftKeyCallPress();
            } else {
                long time;
                KeyEvent metaDown = null;
                KeyEvent metaUp = null;

                if (timeForNextKeyPress > SystemClock.uptimeMillis()) {
                    time = timeForNextKeyPress;
                } else {
                    time = SystemClock.uptimeMillis();
                }

                if (keyEvent[1] > 0) {
                    metaDown = new KeyEvent(time, time, KeyEvent.ACTION_DOWN, keyEvent[1], 0);
                    metaUp = new KeyEvent(time, time + pressTime, KeyEvent.ACTION_UP, keyEvent[1], 0);
                }

                KeyEvent down = new KeyEvent(time, time, KeyEvent.ACTION_DOWN, keyEvent[0], 0);
                KeyEvent up = new KeyEvent(time, time + pressTime, KeyEvent.ACTION_UP, keyEvent[0], 0);

                if (keyEvent[1] > 0) {
                    InputManager.getInstance().injectInputEvent(metaDown,
                    InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
                }
                InputManager.getInstance().injectInputEvent(down, InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
                InputManager.getInstance().injectInputEvent(up, InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
                if (keyEvent[1] > 0) {
                    InputManager.getInstance().injectInputEvent(metaUp, InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
               }
            }
        }

        /**
         * Simulates a press on softkey Call.
         * The implementation assumes portrait mode of the dial app and the actual
         * relative position of the softkey to be the same for all display sizes.
         */
        private void simulateSoftKeyCallPress() {
            /* Get the display pixel size. */
            WindowManager window = (WindowManager) context.getSystemService(context.WINDOW_SERVICE);
            Display display = window.getDefaultDisplay();
            int width = display.getWidth();
            int height = display.getHeight();
        
            /* Set the assumed position of the softkey. */
            Integer xposCall = width / 2;
            Integer yposCall = 7 * height / 8;

            /* Simulate a single tap key press by doing an action down followed by
               an action up 100 ms later. See the CtsaCommandHandler for more details. */
            MotionEvent me;
            long time = SystemClock.uptimeMillis();

            me = MotionEvent.obtain(time, time, MotionEvent.ACTION_DOWN, xposCall, yposCall, 0);
            InputManager.getInstance().injectInputEvent(me,
                InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
            me = MotionEvent.obtain(time, time + 100, MotionEvent.ACTION_UP, xposCall, yposCall, 0);
            InputManager.getInstance().injectInputEvent(me,
                InputManager.INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH);
        }
    }

    /**
     * Validate the syntax of the keys argument.
     *
     * @param keys the argument string to validate
     * @return true if valid and false otherwise
     */
    private boolean validateSyntax(String keys) {
        Log.d(AtService.LOG_TAG, "validateSyntax, parameter:" + keys);
        keys = removeManufacturerSpecificKey(keys);
        Log.d(AtService.LOG_TAG, "removed manufactorSpecificKeys, parameter:" + keys);
        return onlyValidCharacters(keys) || validateAlphaNumericEntries(keys);
    }

    /**
     * A manufacture specific key. A colon followed by any character. Is not
     * supported by this implementation but allowed syntax and therefore
     * ignored.
     *
     * @param str the string to remove manufacture specific keys from.
     * @return the string without manufacture specific keys
     */
    private String removeManufacturerSpecificKey(String str) {
        Log.d(AtService.LOG_TAG, "removeManufacturerSpecificKey: " + str);
        int index = 0;
        StringBuilder cleanedStr = new StringBuilder();

        do {
            int colonIndex = str.indexOf(':', index);
            Log.d(AtService.LOG_TAG, "colonIndex: " + colonIndex + " index:" + index);

            if (colonIndex != -1) {
                Log.d(AtService.LOG_TAG, "adding: " + str.substring(index, colonIndex));
                cleanedStr.append(str.substring(index, colonIndex));
                index = colonIndex + 2; // ignore colon and the character
                // following it
            } else {
                Log.d(AtService.LOG_TAG, "adding: " + str.substring(index));
                cleanedStr.append(str.substring(index)); // no colon
                return cleanedStr.toString();
            }

        } while (index < str.length() - 1);

        return cleanedStr.toString();
    }

    /**
     * Get code for a character. The character is from the +CKPD keys argument
     * but only outside alpha entries.
     * <p>
     * Supported characters: * 0-9 [ ] ; < > ^ v @ C D E M P Q S U W Y letters
     * in both upper and lower case.
     * <p>
     * Note that no key code in android for %. Characters with no
     * corresponding key code are not supported!
     */
    private int getCode(char ch) {
        int code;

        switch (ch) {
        case '*':
            code = KeyEvent.KEYCODE_STAR;
            break;
        case '#':
            code = KeyEvent.KEYCODE_POUND;
            break;
        case '@':
            code = KeyEvent.KEYCODE_AT;
            break;
        case '[':
            code = KeyEvent.KEYCODE_SOFT_LEFT;
            Log.d("CKPD", "keycode softkey left"); // Does not work!
            break;
        case ']':
            code = KeyEvent.KEYCODE_SOFT_RIGHT;
            Log.d("CKPD", "keycode softkey right"); // Does not work!
            break;
        case '<':
            code = KeyEvent.KEYCODE_DPAD_LEFT;
            break;
        case '>':
            code = KeyEvent.KEYCODE_DPAD_RIGHT;
            break;
        case '^':
            code = KeyEvent.KEYCODE_DPAD_UP;
            break;
        case 'v':
        case 'V':
            code = KeyEvent.KEYCODE_DPAD_DOWN;
            break;
        case '0':
            code = KeyEvent.KEYCODE_0;
            break;
        case '1':
            code = KeyEvent.KEYCODE_1;
            break;
        case '2':
            code = KeyEvent.KEYCODE_2;
            break;
        case '3':
            code = KeyEvent.KEYCODE_3;
            break;
        case '4':
            code = KeyEvent.KEYCODE_4;
            break;
        case '5':
            code = KeyEvent.KEYCODE_5;
            break;
        case '6':
            code = KeyEvent.KEYCODE_6;
            break;
        case '7':
            code = KeyEvent.KEYCODE_7;
            break;
        case '8':
            code = KeyEvent.KEYCODE_8;
            break;
        case '9':
            code = KeyEvent.KEYCODE_9;
            break;
        case 'C':
        case 'c':
            code = KeyEvent.KEYCODE_CLEAR; // clean display
            Log.d("CKPD", "keycode clear"); // Does not work!
            break;
        case 'D':
        case 'd':
            code = KeyEvent.KEYCODE_VOLUME_DOWN;
            break;
        case 'E':
        case 'e':
            code = KeyEvent.KEYCODE_ENDCALL; // connection end?
            Log.d("CKPD", "keycode end call"); // Does not work!
            break;
        case 'M':
        case 'm':
            code = KeyEvent.KEYCODE_MENU;
            break;
        case 'P':
        case 'p':
            code = KeyEvent.KEYCODE_POWER;
            Log.d("CKPD", "keycode power"); // Does not work!
            break;
        case 'Q':
        case 'q':
            code = KeyEvent.KEYCODE_MUTE;
            break;
        case 'S':
        case 's':
            code = KeyEvent.KEYCODE_CALL; // connection start?
            Log.d("CKPD", "keycode call"); // Does not work!
            break;
        case 'U':
        case 'u':
            code = KeyEvent.KEYCODE_VOLUME_UP;
            break;
        case 'Y':
        case 'y':
            code = KeyEvent.KEYCODE_DEL;
            break;
        case 'W':
        case 'w':
            code = KEYCODE_PAUSE;
        default:
            code = KeyEvent.KEYCODE_UNKNOWN;
            Log.d(AtService.LOG_TAG, "Unknown character " + ch);

        }

        return code;

    }

    /**
     * Get code for character from alphanumeric entry.
     * Support ascii from value 0x20 to 0x7E but 0x22 doublequote.
     * <p>
     * Alphanumeric entry: ;characters;
     * <p>
     * Ex: ;AB;;C; -> keys AB;C
     *
     * @param ch the character
     * @return the key event code and meta key code
     */
    private int[] getCodeAlphaNumericEntry(char ch) {
        int[] keyEvent = {0,0};

        // A to Z
        if (ch >= 65 && ch <= 90) {
            keyEvent[0] = ch - 36;
            keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
        }
        // a to z
        else if (ch >= 97 && ch <= 122) {
            keyEvent[0] = ch - 68;
        }
        // 0-9
        else if (ch >= 48 && ch <= 57) {
            keyEvent[0] = ch - 41;
        }
        // The rest of ascii printable signs from ' ' to '~' but '"'.
        else {
            switch (ch) {
            case ' ':
                keyEvent[0] = KeyEvent.KEYCODE_SPACE;
                break;
            case '!':
                keyEvent[0] = KeyEvent.KEYCODE_1;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '#':
                keyEvent[0] = KeyEvent.KEYCODE_POUND;
                break;
            case '$':
                keyEvent[0] = KeyEvent.KEYCODE_4;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '%':
                keyEvent[0] = KeyEvent.KEYCODE_5;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '&':
                keyEvent[0] = KeyEvent.KEYCODE_7;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '\'':
                keyEvent[0] = KeyEvent.KEYCODE_APOSTROPHE;
                break;
            case '(':
                keyEvent[0] = KeyEvent.KEYCODE_9;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case ')':
                keyEvent[0] = KeyEvent.KEYCODE_0;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '*':
                keyEvent[0] = KeyEvent.KEYCODE_STAR;
                break;
            case '+':
                keyEvent[0] = KeyEvent.KEYCODE_PLUS;
                break;
            case ',':
                keyEvent[0] = KeyEvent.KEYCODE_COMMA;
                break;
            case '-':
                keyEvent[0] = KeyEvent.KEYCODE_MINUS;
                break;
            case '.':
                keyEvent[0] = KeyEvent.KEYCODE_PERIOD;
                break;
            case '/':
                keyEvent[0] = KeyEvent.KEYCODE_SLASH;
                break;
            case ':':
                keyEvent[0] = KeyEvent.KEYCODE_SEMICOLON;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case ';':
                keyEvent[0] = KeyEvent.KEYCODE_SEMICOLON;
                break;
            case '<':
                keyEvent[0] = KeyEvent.KEYCODE_UNKNOWN;
                break;
            case '=':
                keyEvent[0] = KeyEvent.KEYCODE_EQUALS;
                break;
            case '>':
                keyEvent[0] = KeyEvent.KEYCODE_UNKNOWN;
                break;
            case '?':
                keyEvent[0] = KeyEvent.KEYCODE_SLASH;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '@':
                keyEvent[0] = KeyEvent.KEYCODE_AT;
                break;
            case '[':
                keyEvent[0] = KeyEvent.KEYCODE_LEFT_BRACKET;
                break;
            case '\\':
                keyEvent[0] = KeyEvent.KEYCODE_BACKSLASH;
                break;
            case ']':
                keyEvent[0] = KeyEvent.KEYCODE_RIGHT_BRACKET;
                break;
            case '^':
                keyEvent[0] = KeyEvent.KEYCODE_POWER;
                break;
            case '_':
                keyEvent[0] = KeyEvent.KEYCODE_MINUS;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '`':
                keyEvent[0] = KeyEvent.KEYCODE_GRAVE;
                break;
            case '{':
                keyEvent[0] = KeyEvent.KEYCODE_LEFT_BRACKET;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '|':
                keyEvent[0] = KeyEvent.KEYCODE_UNKNOWN;
                break;
            case '}':
                keyEvent[0] = KeyEvent.KEYCODE_RIGHT_BRACKET;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            case '~':
                keyEvent[0] = KeyEvent.KEYCODE_GRAVE;
                keyEvent[1] = KeyEvent.KEYCODE_SHIFT_LEFT;
                break;
            default:
                keyEvent[0] = KeyEvent.KEYCODE_UNKNOWN;
                Log.d(AtService.LOG_TAG, "Unknown character " + ch + " in alpha entry");
            }
        }

        return keyEvent;
    }

    /**
     * Checks that all the characters the keys argument to +CKPD are valid.
     *
     * @param keys the string to validate
     * @return true if valid and false otherwise
     */
    private boolean onlyValidCharacters(String keys) {
        for (int i = 0; i < keys.length(); i++) {
            if (!isDefinedCharacter(keys.charAt(i))) {
                Log.d(AtService.LOG_TAG, "Unknown character " + keys.charAt(i)
                      + " index of char is " + i);
                return false;
            }
        }

        return true;
    }

    /**
     * Checks the syntax of alphanumeric entries in the keys argument.
     *
     * @param keys the string to validate
     * @return true if valid and false otherwise
     */
    private boolean validateAlphaNumericEntries(String keys) {
        int startIndex = keys.indexOf(';');
        int endIndex;

        while (startIndex != -1) {
            endIndex = getEndIndexAlphaEntry(keys, startIndex);

            if (endIndex != -1) {
                // check valid values in alpha entry
                for (int i = startIndex + 1; i < endIndex; i++) {
                    char ch = keys.charAt(i);

                    if (!isValidInAlphaNumericEntry(ch)) {
                        return false;
                    }
                }
            } else {
                // illegal syntax only found beginning of alpha entry
                return false;
            }

            if (endIndex + 1 < keys.length()) {
                startIndex = keys.indexOf(';', endIndex + 1);
            } else {
                // end of string
                return true;
            }

        }
        return false;
    }

    /**
     * Method used to find the end index for an alpha entry.
     *
     * @param str to look for the end index in
     * @param startIndex the start index in the string of the alpha entry
     * @return the end index or -1
     */
    private int getEndIndexAlphaEntry(String str, int startIndex) {
        if (startIndex + 1 >= str.length()) {
            // start index is last index in str
            return -1;
        }

        int endIndex = str.indexOf(';', startIndex + 1);

        if (endIndex == -1) {
            // no semicolon after the first
            return -1;
        }

        int nextIndex = endIndex + 1;

        while (nextIndex < str.length() && endIndex != -1) {
            if (str.charAt(nextIndex) == ';') {
                // Found ;; in alpha entry - find next
                endIndex = str.indexOf(';', nextIndex + 1);

                if (endIndex == -1) {
                    // If no more ';' then expression is malformed,
                    // due to that double ';' is converted to one.
                    break;
                }
            } else {
                // found endIndex
                break;
            }

            nextIndex = endIndex + 1;
        }

        return endIndex;
    }

    /**
     * Checks if a character is valid in an alphanumeric entry.
     *
     * @param ch the character to test
     * @return true if valid and false otherwise
     */
    private boolean isValidInAlphaNumericEntry(char ch) {
        return definedLettersAlphaNumeric(ch);

    }

    /**
     * Tests if a given character is defined.
     * <p>
     * Supported characters: * 0-9 ; : [ ] < > ^ @ # A B C D E F L M P Q R S T U V
     * W X Y letters in both upper and lower case.
     *
     * @param ch the letter to test
     * @return true if defined and false otherwise
     */
    private boolean isDefinedCharacter(char ch) {
        return ch == '*' || ch == '<' || ch == '>' || ch == '^' || ch == '@' || ch == '['
               || ch == ']' || ch == ';' || ch == ':' || ch == '#' || Character.isDigit(ch)
               || definedLetters(ch);
    }

    /**
     * Tests if a given letter is defined.
     * <p>
     * Supported characters: A B C D E F L M P Q R S T U V W X Y letters in both
     * upper and lower case.
     *
     * @param ch the letter to test
     * @return true if defined and false otherwise
     */
    private boolean definedLetters(char ch) {
        return ch == 'A' || ch == 'a' || ch == 'B' || ch == 'b' || ch == 'C' || ch == 'c'
               || ch == 'D' || ch == 'd' || ch == 'E' || ch == 'e' || ch == 'F' || ch == 'f'
               || ch == 'L' || ch == 'l' || ch == 'M' || ch == 'm' || ch == 'P' || ch == 'p'
               || ch == 'Q' || ch == 'q' || ch == 'R' || ch == 'r' || ch == 'S' || ch == 's'
               || ch == 'T' || ch == 't' || ch == 'U' || ch == 'u' || ch == 'V' || ch == 'v'
               || ch == 'X' || ch == 'x' || ch == 'Y' || ch == 'y' || ch == 'W' || ch == 'w';
    }

    /**
     * Tests if a given letter is defined for alfanumeric input.
     * <p>
     * Supported characters: ascii characters from ' ' to '~' but '"'.
     *
     * @param ch the letter to test
     * @return true if defined and false otherwise
     */
    private boolean definedLettersAlphaNumeric(char ch) {
        return ch >= ' ' && ch <= '~' && ch != '"';
    }
}
