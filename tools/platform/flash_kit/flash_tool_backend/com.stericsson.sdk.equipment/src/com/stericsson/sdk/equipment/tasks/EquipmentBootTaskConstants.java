package com.stericsson.sdk.equipment.tasks;

/**
 * @author xtomzap
 * 
 */
public enum EquipmentBootTaskConstants {

    /** Public ROM CRC property name */
    PROP_PUBLIC_ROM_CRC("Public ROM CRC"),

    /** Secure ROM CRC property name */
    PROP_SECURE_ROM_CRC("Secure ROM CRC"),

    /** Root key hash property name */
    PROP_ROOT_KEY_HASH("Root Key Hash"),

    /** Secure mode property name */
    PROP_SECURE_MODE("Secure Mode"),

    /** Public ID property name */
    PROP_PUBLIC_ID("Public ID"),

    /** Nomadik ID property name */
    PROP_NOMADIK_ID("Nomadik ID"),

    /** Asic version property name */
    PROP_ASIC_VERSION("ASIC Version"),

    /** Chip option property name */
    PROP_CHIP_OPTION("Chip Option"),

    /** Chip customer ID */
    PROP_CHIP_CUSTOMER_ID("Chip Customer ID"),

    /** Issw version property name */
    PROP_ISSW_VERSION("ISSW Version"),

    /** Xloader version property name */
    PROP_XLOADER_VERSION("X-LOADER Version"),

    /** Power management property name */
    PROP_PWR_MGT_VERSION("PWR_MGT Version"),

    /** MEM_INIT version property name */
    PROP_MEM_INIT_VERSION("MEM_INIT Version"),

    /** Change baud rate in ROM property name */
    PROP_CHANGE_BAUD_RATE_IN_ROM("Change Baud Rate in ROM");

    private String propertyName;

    private EquipmentBootTaskConstants(String pPropertyName) {
        propertyName = pPropertyName;
    }

    /**
     * @return name of property
     */
    public String getPropertyName() {
        return propertyName;
    }
}
