/**
 * 
 */
package com.stericsson.sdk.equipment;


/**
 * @author xhelciz
 * 
 */
public class UnsupportedPropertyException extends EquipmentException {
    /**
     * 
     */
    private static final long serialVersionUID = 8586191636664994422L;

    /**
     * UnparsablePropertyException should be used when the given property, its value or name is not
     * possible to parse to gain needed data
     * @param message exception message
     */
    public UnsupportedPropertyException(String message) {
        super(message);
    }
}
