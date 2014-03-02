/**
 * 
 */
package com.stericsson.sdk.equipment;

/**
 * @author xhelciz
 * 
 */
public class UnparsablePropertyException extends UnsupportedPropertyException {

    /**
     * 
     */
    private static final long serialVersionUID = -5708786735537158475L;

    /**
     * UnparsablePropertyException should be used when the given property, its value or name is not
     * possible to parse to gain needed data
     * 
     * @param message
     *            exception message
     */
    public UnparsablePropertyException(String message) {
        super(message);
    }

}
