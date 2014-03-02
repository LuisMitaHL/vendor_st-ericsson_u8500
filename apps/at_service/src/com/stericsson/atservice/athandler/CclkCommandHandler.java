/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import java.util.Calendar;
import java.util.TimeZone;
import android.os.SystemClock;
import android.content.Context;
import android.util.Log;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;
import com.stericsson.atservice.CmeErrorCodeConstants;

import org.apache.harmony.luni.internal.util.TimezoneGetter;
import android.app.AlarmManager;

/**
 * The CclkCommandHandler handles command AT+CCLK.
 * <p>
 * Possible alternatives are AT+CCLK=<time>, AT+CCLK? and AT+CCLK=?
 */
public class CclkCommandHandler extends AtCommandHandler {

    /**
     * Value used to convert milliseconds to quarters. 1000 to seconds, 60 to
     * minutes, 15 to quarters
     */
    private final static int CONVERSION_MILLISECONDS_QUARTERS = 900000;

    /**
     * The max value allowed for time zone.
     */
    private final static int TIMEZON_MAX = 48;

    /**
     * The min value allowed for time zone.
     */
    private final static int TIMEZON_MIN = -47;

    /**
     * The length of the <time> string
     */
    private final static int TIME_STRING_LENGTH = 24;

    /**
     * Constructor for a CclkCommandHandler.
     *
     * @param context the context
     * @param atParser the parser
     */
    public CclkCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "+CCLK";
        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(false),
        };
    }

    /**
     * Handle command AT+CCLK command.
     *
     * @return The result of this command.
     */
    public AtCommandResponse handleSetCommand() {
        if (!checkArgumentsValidSetDefault()) {
            // only checks that argument is one and a string - more syntax check
            // later when a CME Error will be sent
            Log.e(AtService.LOG_TAG, "Arguments to " + commandName + " is invalid");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        String time = (String) atCommand.getArguments()[0];
        Calendar calendar = parseTime(time);

        if (null == calendar) {
            return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                         CmeErrorCodeConstants.INVALID_CHARACTERS_IN_TEXT_STRING);
        }

        try {
            long when = calendar.getTimeInMillis();

            // Do things the same way as in DateTimeSettings
            if (when / 1000 < Integer.MAX_VALUE) {
                Log.d(commandName, "Value before " + SystemClock.currentThreadTimeMillis()
                      + "CurrentTimeMillis set:" + when);
                SystemClock.setCurrentTimeMillis(when);
                Log.d(commandName, "CurrentTimeMillis after:"
                      + SystemClock.currentThreadTimeMillis());
                // set time zone - done this way in GsmServiceStateTracker
                AlarmManager alarm = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);
                if (alarm != null) {
                    alarm.setTimeZone(calendar.getTimeZone().getID());
                    return new AtCommandResponse(AtCommandResponse.OK);
                } else {
                    return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                                 CmeErrorCodeConstants.OPERATION_NOT_ALLOWED);
                }
            }
        } catch (Exception e) {
            // set values outside intervals
            return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                         CmeErrorCodeConstants.INVALID_CHARACTERS_IN_TEXT_STRING);
        }

        return new AtCommandResponse(AtCommandResponse.CME_ERROR, CmeErrorCodeConstants.UNKNOWN);
    }

    /**
     * Returns a non lenient Calendar or null if the syntax of time is faulty.
     * Format: yyyy/mm/dd,hh:mm:ss+|-zz
     * <p>
     * The Calandar will throw an Exception when calling getTimeMillis() if
     * values from time are not in the correct intervals.
     *
     * @return a Calendar or null
     */
    private Calendar parseTime(String time) {
        if (time.length() == TIME_STRING_LENGTH) { // check length
            // remove " "
            time = time.substring(1, TIME_STRING_LENGTH - 1);
            int startIndex = 0;
            int nextIndex = 5;
            String year = time.substring(startIndex, nextIndex - 1); // skip /
            startIndex = nextIndex;
            nextIndex += 3;
            String month = time.substring(startIndex, nextIndex - 1);
            startIndex = nextIndex;
            nextIndex += 3;
            String day = time.substring(startIndex, nextIndex - 1);
            startIndex = nextIndex;
            nextIndex += 3;
            String hours = time.substring(startIndex, nextIndex - 1);
            startIndex = nextIndex;
            nextIndex += 3;
            String minutes = time.substring(startIndex, nextIndex - 1);
            startIndex = nextIndex;
            nextIndex += 2;
            String seconds = time.substring(startIndex, nextIndex);

            TimeZone timeZone = getTimeZone(time.substring(nextIndex));

            if (null == timeZone) {
                // faulty syntax
                return null;
            }

            Calendar calendar = Calendar.getInstance(timeZone);
            // will throw exception then calling getTimeMillis() if values not
            // in correct intervals
            calendar.setLenient(false);

            try {
                // month in calendar 0 to 11
                calendar.set(Integer.parseInt(year), Integer.parseInt(month) - 1, Integer
                             .parseInt(day), Integer.parseInt(hours), Integer.parseInt(minutes), Integer
                             .parseInt(seconds));
            } catch (NumberFormatException e) {
                // fault syntax not integers - must still check numbers in
                // correct intervals
                return null;
            }

            return calendar;
        }

        return null;

    }

    /**
     * Converts the string timeZone to the corresponding TimeZone. The format of
     * the string must be +|-zz. Returns null if syntax of timeZone faulty.
     *
     * @return a TimeZone or null
     */
    private TimeZone getTimeZone(String timeZone) {
        // validate syntax
        if (timeZone.length() != 3 || !Character.isDigit(timeZone.charAt(1))
                || !Character.isDigit(timeZone.charAt(2))) {
            return null;
        }

        char sign = timeZone.charAt(0);

        // check sign valid
        if (sign == '+' || sign == '-') {
            int nbrQuarters = Integer.parseInt(timeZone.substring(1));

            // check value between -47...+48
            if (nbrQuarters >= TIMEZON_MIN && nbrQuarters <= TIMEZON_MAX) {
                int hours = nbrQuarters / 4;
                int minutes = (nbrQuarters % 4) * 15; // remove hours
                StringBuilder zon = new StringBuilder("GMT");
                // format: GMT+|-hh[mm]
                zon.append(sign);

                // format hh
                if (hours < 10) {
                    // add extra zero
                    zon.append(0);
                }

                zon.append(hours);

                if (minutes > 0) {
                    // at least 15 minutes - has always two digits
                    zon.append(minutes);
                }

                return TimeZone.getTimeZone(zon.toString());
            }
        }

        // faulty sign or value
        return null;
    }

    /**
     * Handle command AT+CCLK? command.
     *
     * @return The result of this command.
     */
    public AtCommandResponse handleReadCommand() {
        Calendar calendar = Calendar.getInstance(getTimeZone());
        calendar.setTimeInMillis(System.currentTimeMillis());
        StringBuilder time = new StringBuilder();
        time.append('"');
        // add date
        time.append(getDateString(calendar));
        // add time
        time.append(getTimeString(calendar));
        // add time zone
        time.append(getTimeZoneString(getTimeZone().getOffset(calendar.getTimeInMillis())));
        time.append('"');

        return new AtCommandResponse(AtCommandResponse.OK, "+CCLK: " + time.toString());
    }

    /**
     * Returns a date as a string from a Calendar. Format: yyyy/mm/dd,
     *
     * @param calendar the calendar
     * @return a data string
     */
    private String getDateString(Calendar calendar) {
        StringBuilder date = new StringBuilder();
        int year = calendar.get(Calendar.YEAR);

        // format yyyy
        if (year < 10) {
            date.append("000");
        } else if (year < 100) {
            date.append("00");
        } else if (year < 1000) {
            date.append('0');
        }

        date.append(year);
        date.append('/');
        int month = calendar.get(Calendar.MONTH);
        // month in calendar 0 to 11
        month++;

        // format mm
        if (month < 10) {
            date.append('0');
        }

        date.append(month);
        date.append('/');
        int day = calendar.get(Calendar.DATE);

        // format dd
        if (day < 10) {
            date.append('0');
        }

        date.append(day);
        date.append(',');
        return date.toString();
    }

    /**
     * Returns the time from a Calendar as a string. Format: hh:mm:ss
     *
     * @param calendar the calendar
     * @return the time string
     */
    private String getTimeString(Calendar calendar) {
        StringBuilder time = new StringBuilder();
        int hour = calendar.get(Calendar.HOUR_OF_DAY);

        // format hh
        if (hour < 10) {
            time.append('0');
        }

        time.append(hour);
        time.append(':');
        int minute = calendar.get(Calendar.MINUTE);

        // format mm
        if (minute < 10) {
            time.append('0');
        }

        time.append(minute);
        time.append(':');
        int seconds = calendar.get(Calendar.SECOND);

        // format ss
        if (seconds < 10) {
            time.append('0');
        }

        time.append(seconds);
        return time.toString();
    }

    /**
     * Returns the TimeZone of the currently set time.
     *
     * @return the time zone
     */
    private TimeZone getTimeZone() {
        String timezoneId = TimezoneGetter.getInstance().getId();
        TimeZone timeZone;

        if (timezoneId.length() == 0) {
            // no time zone set - use default - not sure about this
            timeZone = TimeZone.getDefault();
        } else {
            // create time zone from id
            timeZone = TimeZone.getTimeZone(timezoneId);
        }

        return timeZone;
    }

    /**
     * Return the time zone as a string, syntax +|-zz, a value in rage -47...+48
     *
     * @param millisecondsOffset the time offset from GSM in milliseconds
     * @return the time zone string
     */
    private String getTimeZoneString(long millisecondsOffset) {
        StringBuilder timeZone = new StringBuilder();

        if (millisecondsOffset >= 0) {
            timeZone.append('+');
        } else {
            timeZone.append('-');
        }

        // need to know the number of quarters
        long nbrOfQuarters = millisecondsOffset / CONVERSION_MILLISECONDS_QUARTERS;

        // check value between -47 and +48
        if (nbrOfQuarters > TIMEZON_MAX) {
            // fault value - be nice and simply set max value
            timeZone.append(TIMEZON_MAX);
        } else if (nbrOfQuarters < TIMEZON_MIN) {
            // fault value - be nice and simply set min value
            timeZone.append(TIMEZON_MIN);
        } else {
            // valid value
            if (nbrOfQuarters < 10) {
                // always use two digits
                timeZone.append(0);
            }

            if (nbrOfQuarters < 0) {
                // don't want to write minus sign again - cannot write it here
                // ex - nbrOfQuarters = -5 -> 0-5
                nbrOfQuarters = Math.abs(nbrOfQuarters);
            }

            timeZone.append(nbrOfQuarters);

        }

        return timeZone.toString();
    }

    /**
     * Handle command AT+CCLK=?.
     *
     * @return The result of this command.
     */
    protected AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK);
    }
}
