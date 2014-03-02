/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.HashSet;

import android.util.Log;

/**
 * This class holds the properties of an argument to an AT command. It stores
 * the default value (the argument is optional), allowed or forbidden values and
 * information about the argument's type.
 */
public class AtArgumentProperties {

    /**
     * The default value of the argument.
     */
    private Object defaultValue;

    /**
     * Values that if they are defined are either allowed or forbidden depending
     * on the value of <code>isAllowed</code>.
     */
    private HashSet<Object> specifiedValues;

    /**
     * Indicated if the specified values are allowed or forbidden
     */
    private boolean isAllowed;

    /**
     * Indicates if the argument is an <code>Integer</code> and not a
     * <code>String</code>.
     */
    private boolean isInteger;

    /**
     * Indicates if this argument is optional.
     */
    private boolean isOptionalArgument;

    /**
     * Constructor for an <code>AtArgumentProperties</code>. Note that the types
     * of the <code>defaultValue</code> and the <code>speciedValues</code> are
     * not tested. However their type should correspond to the one
     * <code>isInteger</code> indicates.
     * <p>
     * Note: To support a basic command without an argument set default value ""
     *
     * @param isInteger indicates if the argument type is integer and not string
     * @param defaultValue the default value of the argument if such is defined,
     *            a string should start and end with ", "\"default\"". A default
     *            value means that the argument is optional
     * @param specifiedValues values that if they are defined are either allowed
     *            or forbidden depending on the value of <code>isAllowed</code>.
     *            A string should start and end with "
     * @param isAllowed indicated if the specified values, are allowed or
     *            forbidden
     */
    public AtArgumentProperties(boolean isInteger, Object defaultValue,
                                HashSet<Object> specifiedValues, boolean isAllowed) {
        // Add control of type for all values?
        this.isInteger = isInteger;

        if (specifiedValues != null) {
            this.specifiedValues = specifiedValues;
        } else {
            this.specifiedValues = new HashSet<Object>();
        }

        if (defaultValue != null) {
            this.defaultValue = defaultValue;
            this.isOptionalArgument = true;
        }

        if (isOptionalArgument && isAllowed) {
            // add the default value to the allowed values if it is not
            // already present
            this.specifiedValues.add(defaultValue);
            // optional arguments must be allowed ex arg: "",12,"three"
            this.specifiedValues.add("");
        }

        this.isAllowed = isAllowed;

        if (isAllowed && defaultValue == null && specifiedValues.isEmpty()) {
            Log.e(AtService.LOG_TAG,
                  "Warning - creating ArgumentProperty with no allowed values at all");
        }

    }

    /**
     * Creates an ArgumentProperty allowing either all string or all integer
     * values depending on the value of isInteger.
     *
     * @param isInteger indicates if the argument type is integer and not string
     */
    public AtArgumentProperties(boolean isInteger) {
        this(isInteger, null, null, false);
    }

    /**
     * Create an AtArgumentProperty for no argument.
     * <p>
     * Ex: ATA, AT+CGMI
     */
    public AtArgumentProperties() {
        this(false, "", null, true);
    }

    /**
     * Indicates if <code>value</code> is valid for this argument. Note that it
     * does not check if a string value starts and ends with ".
     */
    public boolean isValid(Object value) {
        if (value == null) {
            return false;
        }

        // if is optional - then "" is allowed - "" is not allowed as value in
        // general
        if (isOptionalArgument && value.toString().length() == 0) {
            return true;
        } else if (!isOptionalArgument && value.toString().length() == 0) {
            return false;
        }

        // is not on the allowed list
        if (isAllowed && !specifiedValues.contains(value)) {
            return false;
        }

        // in the forbidden list
        if (!isAllowed && specifiedValues.contains(value)) {
            return false;
        }

        // incorrect type
        if (value instanceof Integer && !isInteger || value instanceof String && isInteger) {
            return false;
        }

        // is a valid value
        return true;

    }

    /**
     * Returns the default value of this argument if such a value exist and
     * otherwise <code>null</code>.
     *
     * @return the default value or <code>null</code>
     */
    public Object getDefaultValue() {
        return defaultValue;
    }

    /**
     * Returns the default value of this argument as a string. If no such value
     * exist it returns an empty string.
     *
     * @return a string
     */
    public String getDefaultValueAsString() {
        if (null == defaultValue) {
            return "";
        }

        if (isInteger) {
            return String.valueOf(((Integer) defaultValue).intValue());
        } else {
            return (String) defaultValue;
        }
    }

}
