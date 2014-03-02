package com.stericsson.sdk.common.ui.wizards.logic;

import java.util.Vector;

/**
 * Class that is used to store data read from a GDF file.
 */
public final class GDFDataInfo {
    int id;

    int size;

    int block;

    Vector<String> data;

    /**
     * {@inheritDoc}
     */
    public boolean equals(Object o) {
        if (!(o instanceof GDFDataInfo)) {
            return false;
        } else {
            GDFDataInfo t = (GDFDataInfo) o;
            return (t.id == id && t.block == block);
        }
    }

    /**
     * Returns a unique hash for this object instance.
     * 
     * @return The hash code.
     */
    public int hashCode() {
        return super.hashCode();
    }

}
