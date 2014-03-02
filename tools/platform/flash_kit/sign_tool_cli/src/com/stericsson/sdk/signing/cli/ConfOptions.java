package com.stericsson.sdk.signing.cli;


/**
 * 
 * enum
 */
public enum ConfOptions {
    /**
     * 
     */
    TARGET("TARGET"),
    /**
     * 
     */
    PACKAGE("PACKAGE"),
    /**
     * 
     */
    FORCE("FORCE"),
    /**
     * 
     */
    DRYRUN("DRYRUN"),
    /**
     * 
     */
    VERBOSE("VERBOSE"),
    /**
     * 
     */
    TEST("TEST"),
    /**
     * 
     */
    IN_FORMAT("IN-FORMAT"),
    /**
     * 
     */
    OUT_FORMAT("OUT-FORMAT"),
    /**
     * 
     */
    MAC_MODE("MAC-MODE"),
    /**
     * 
     */
    SW_TYPE("SW-TYPE"),
    /**
     * 
     */
    INTERACTIVE("INTERACTIVE"),
    /**
     * 
     */
    APP_SEC("APP-SEC"),
    /**
     * 
     */
    HDR_SEC("HDR-SEC"),
    /**
     * 
     */
    PTYPE("PTYPE"),
    /**
     * 
     */
    ADDR_FORMAT("ADDR-FORMAT"),
    /**
     * 
     */
    DEBUG("DEBUG"),
    /**
     * 
     */
    ETX_REAL("ETX-REAL"),
    /**
     * 
     */
    ETX_HEADER("ETX-HEADER"),
    /**
     * 
     */
    SHORT_MAC("SHORT-MAC"),
    /**
     * 
     */
    ANTI_ROLLBACK("ANTI-ROLLBACK"),
    /**
     * 
     */
    ALIGN("ALIGN"),
    /**
     * 
     */
    S3_LENGTH("S3-LENGTH"),
    /**
     * 
     */
    SW_VERSION("SW-VERSION");

    private String name;

    private ConfOptions(String newName) {
        this.name = newName;
    }

    /**
     * @return String name Get name.
     */
    public String getName() {
        return name;
    }
}