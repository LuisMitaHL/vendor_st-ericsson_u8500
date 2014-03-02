/**
 * 
 */
package com.stericsson.sdk.common.ui.formatters;

/**
 * General interface for formatters.
 * 
 * @author mbodan01
 * @param <Type>
 *            generic type
 */
public interface IFormatter<Type> {

    /**
     * Interface used for format generic type
     * 
     * @param o
     *            non formated value
     * @return formated value
     */
    String format(Type o);
}
