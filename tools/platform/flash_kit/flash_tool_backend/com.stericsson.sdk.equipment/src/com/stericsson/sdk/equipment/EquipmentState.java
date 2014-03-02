package com.stericsson.sdk.equipment;

import java.util.Locale;

/**
 * @author xtomlju
 */
public enum EquipmentState {
    /** */
    IDLE,

    /** */
    BUSY,

    /** */
    ERROR,

    /** */
    FLASHING,

    /** */
    DUMPING,

    /** */
    LC_NOT_INITIALIZED,

    /** */
    BOOTING,

    /** */
    ERASING,

    /** */
    AUTHENTICATING;

    /**
     * 
     * @return a nicer formated version of the state
     */
    public String niceName() {
        String name = name();
        switch (this) {
            case LC_NOT_INITIALIZED:
                return "Loader Communication N/A";
            default:
                return name.length() == 1 ? name : name.substring(0, 1)
                    + name.substring(1, name.length()).toLowerCase(Locale.getDefault());
        }
    }
}
