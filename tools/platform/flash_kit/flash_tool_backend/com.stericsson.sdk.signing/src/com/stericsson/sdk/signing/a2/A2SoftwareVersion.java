package com.stericsson.sdk.signing.a2;

import com.stericsson.sdk.signing.cops.COPSControlFields;

/**
 * This class is "Software Version" field in A2 Header. SW_VERSION is a 4-byte field that contains
 * the software version.
 * 
 * @author ezaptom
 * 
 */
public class A2SoftwareVersion {

    /***/
    public static final int ANTI_ROLLBACK_MODE_1SBC = 1;

    /***/
    public static final int ANTI_ROLLBACK_MODE_2SBC = 2;

    /***/
    public static final int ANTI_ROLLBACK_MODE_3SBC = 3;

    /***/
    public static final int ANTI_ROLLBACK_MODE_4SBC = 4;

    /***/
    public static final int ANTI_ROLLBACK_MODE_SW = 5;

    /***/
    public static final int MAX_SW_VERSION = 255;

    /***/
    public static final int ANTI_ROLLBACK_REQUIRED = 1;

    /***/
    public static final int ANTI_ROLLBACK_NOT_REQUIRED = 0;

    /***/
    private int cpu;

    /***/
    private int antiRollbackMode;

    /***/
    private boolean antiRollbackRequired;

    /***/
    private int swVersion = 0;

    /***/
    public A2SoftwareVersion() {

    }

    /**
     * Sets the CPU variable, i.e the processor which the software module is intended for.
     * 
     * @param cpuI
     *            0 (Access CPU) or 1 (Application CPU).
     */
    public void setCPU(int cpuI) {
        cpu = cpuI;
    }

    /**
     * Sets the boot stage (translated to anti-rollback stage)
     * 
     * @param mode
     *            Anti-rollback mode.
     */
    public void setMode(int mode) {
        antiRollbackMode = mode;
    }

    /**
     * Sets the flag whether anti-rollback protection is required or not.
     * 
     * @param aRollRequired
     *            True if anti-rollback is required, false if not.
     */
    public void setAntiRollbackRequired(int aRollRequired) {
        if (aRollRequired == ANTI_ROLLBACK_REQUIRED) {
            antiRollbackRequired = true;
        } else {
            antiRollbackRequired = false;
        }
    }

    /**
     * Sets the software version.
     * 
     * @param version
     *            The software version to set.
     */
    public void setSoftwareVersion(int version) {
        if ((version < 0) || (version > MAX_SW_VERSION)) {
            throw new IllegalArgumentException("Invalid A2_SW_VERSION in header, SW Version: " + version);
        }

        swVersion = version;
    }

    /**
     * Returns the software version.
     * 
     * @param hid
     *          the header version
     * @return The software version.
     */
    public int getSoftwareVersion(int hid) {
        int version = 0;

        if (hid != COPSControlFields.HID_BABE0300) {
            version += cpu << 13;
            version += antiRollbackMode << 10;
            if (antiRollbackRequired) {
                version += ANTI_ROLLBACK_REQUIRED << 8;
                version += swVersion << 0;
            }
        }

        return version;
    }
}
