/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;

/**
 * The AtdCommandHandler handles the ATD command.
 * <p>
 * Possible alternatives are ATD<dial_string>[G or g][I or i] and ATDL[G or g][I
 * or i]
 */
public class AtdCommandHandler extends AtCommandHandler {

    // TODO: Handle numbers not called with ATD
    // TODO: What should I use as initial value?
    private String lastCalledNbr = "0000";

    /**
     * Constructor for an <code>AtdCommandHandler</code>.
     *
     * @param context the context this handler was created in
     * @param atParser the AT parser this handler belong to
     */
    public AtdCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "D";
        // TODO: Set default value to "" to support ATD with "" argument
        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(false, "", null, false),
        };
    }

    /**
     * A voice call is an ATD command ended by a semicolon. Result code OK
     * should always be returned.
     */
    public AtCommandResponse handleActionCommand() {
        if (!checkArgumentsValidSetDefault()) {
            Log.e(AtService.LOG_TAG, "Arguments to " + commandName + " is invalid");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        Object[] argumentValues = atCommand.getArguments();

        if (argumentValues[0].toString().length() != 0) {
            // Two cases ATD or ATDL, ATDL is redial last number
            String dialString;

            if (argumentValues[0].toString().charAt(0) == 'L') { // check ATDL
                dialString = lastCalledNbr;
            } else { // ATD
                // the number is the first and only argument
                dialString = extractPhoneNbr(argumentValues[0].toString());
                // save number
                lastCalledNbr = dialString;
            }

            Log.d(AtService.LOG_TAG, "Dial string: " + dialString);

            try {
                Intent intent = new Intent(Intent.ACTION_CALL_PRIVILEGED, Uri.parse("tel:" + Uri.encode(dialString)));
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(intent);
                return new AtCommandResponse(AtCommandResponse.OK);
            } catch (Exception e) {
                // something is wrong return ERROR
                return new AtCommandResponse(AtCommandResponse.ERROR);
            }

        } else {
            // case ATD; do nothing and return OK
            return new AtCommandResponse(AtCommandResponse.OK);
        }

    }

    /**
     * Any characters appearing in the dial string that the DCE does not
     * recognize as a valid part of the call addressing information or as a
     * valid modifier shall be ignored.
     * <p>
     * Only keeps valid characters in a dial string. D, T, !, @ are
     * all ignored by the implementation and therefore also removed.
     *
     * @param parameter the parameter sent with ATD
     * @return the dial string
     */
    private String extractPhoneNbr(String parameter) {
        StringBuilder dialString = new StringBuilder();
        boolean foundPostDialSeparator = false;

        for (int i = 0; i < parameter.length(); i++) {
            // Android expects the characters ',' and ';' in the string
            // to know when to start sending DTMF tones
            char ch = convertPostDialChar(parameter.charAt(i), i);

            // Look for the first ',' in the string. It is called PostDialSeperator.
            // It is after the second ',' character the pause starts before sending the DTMF tones.
            if (isPostDialSeparator(ch) && !foundPostDialSeparator) {
                // Add the ',' character twice the first time it appears in the string
                dialString.append(ch);
                foundPostDialSeparator = true;
            }

            if (isValidCharacter(ch)) {
                dialString.append(ch);
            }
        }

        return dialString.toString();
    }


    private char convertPostDialChar(char ch, int pos) {
        if (ch == 'p' || ch == 'P') {
            if (pos == 0)
                return ch; // A leading 'P' means pulse dialling, not pause.
            else
                return ','; // Android uses ',' as the PAUSE character
        } else if (ch == 'w' || ch == 'W') {
            return ';'; // Android uses ; as the WAIT character
        } else {
            return ch;
        }
    }


    private boolean isPostDialSeparator(char ch) {
        if (ch == ',')
             return true;

        return false;
    }


    /**
     * Determine if a character is valid for the dial string.
     */
    private boolean isValidCharacter(char ch) {
        return ch == 'A' || ch == 'B' || ch == 'C' || ch == '*' || ch == '#'
               || Character.isDigit(ch) || ch == 'g' || ch == 'G' || ch == 'i' || ch == 'I'
               || ch == '+' || ch == ',' || ch == ';';
    }

}
