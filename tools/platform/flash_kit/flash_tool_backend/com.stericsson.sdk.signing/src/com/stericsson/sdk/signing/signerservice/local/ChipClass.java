package com.stericsson.sdk.signing.signerservice.local;

import java.util.HashMap;

/**
 * ChipClass object
 * 
 * @author qjankma
 * 
 */

public class ChipClass {

    private HashMap<Integer, Cid> mCids; // containing the list of cids assosiated with this

    // chipclass

    private int mChipClassId; // identifier for the id

    /**
     * Constructor for chipclass
     * 
     * @param chipClassId
     *            chip class ID
     */

    public ChipClass(int chipClassId) {
        mCids = new HashMap<Integer, Cid>();
        mChipClassId = chipClassId;
    }

    /**
     * Add the a cid to the datastructure
     * 
     * @param cidId
     *            cid ID
     */

    public void addCid(int cidId) {
        Cid cid = (Cid) mCids.get(Integer.valueOf(cidId));
        if (cid == null) {
            cid = new Cid();
            mCids.put(Integer.valueOf(cidId), cid);
        }
    }

    /**
     * @param cidId
     *            cid ID
     * @return Cid with that cidId
     */

    public Cid getCid(int cidId) {
        return (Cid) mCids.get(Integer.valueOf(cidId));
    }

    /**
     * returns the Id of this ChipClass
     * 
     * @return id
     */

    public int getId() {
        return mChipClassId;
    }
}
