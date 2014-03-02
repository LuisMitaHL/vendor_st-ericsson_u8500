/**
 * 
 */
package com.stericsson.sdk.common.ui.formatters;

/**
 * Null Object for IForrmater
 * 
 * @author mbodan01
 * @param <Type>
 *            generic type
 */
public class NullFormatter<Type> implements IFormatter<Type> {

    /**
     * @param o
     *            non formated input
     * @return formated output
     */
    public String format(Type o) {
        return o.toString();
    }

}
