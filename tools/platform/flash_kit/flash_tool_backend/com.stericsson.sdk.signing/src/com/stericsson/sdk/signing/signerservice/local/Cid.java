package com.stericsson.sdk.signing.signerservice.local;

import java.util.HashMap;

/**
 * Cid object, contains all the chipids related to this Cid.
 * 
 * @author qjankma
 * 
 */

public class Cid {

    private HashMap<Integer, ChipId> mChipIds; // containing the chipids

    // assosiated with this cid

    /**
     * Constructor for Cid
     */
    public Cid() {
        mChipIds = new HashMap<Integer, ChipId>();
    }

    /**
     * Get the chipId assosiated with this chipid
     * 
     * @param chipId
     *            chip ID
     * @return ChipId assosiated with this chipid
     */
    public ChipId getChipId(int chipId) {
        return mChipIds.get(Integer.valueOf(chipId));
    }

    /**
     * Add chipkey to the object assosiated with the chipId
     * 
     * @param chipId
     *            chip ID
     * @param alias
     *            alias name an alias name identifying the chip
     * @param chipKey
     *            chip Key
     */
    public void addChipId(int chipId, String alias, int[] chipKey) {
        ChipId chip = getChipId(chipId);
        if (chip == null) {
            chip = new ChipId(chipId);
            mChipIds.put(Integer.valueOf(chipId), chip);
        }
        chip.addEncryptionKey(chipKey);
        chip.setAlias(alias);
    }

    /**
     * @return chipids assosiated with this CID
     */
    public int[] getChipIds() {
        Object[] chipids = mChipIds.keySet().toArray();
        int[] chipIdValues = new int[chipids.length];
        for (int i = 0; i < chipids.length; i++) {
            chipIdValues[i] = ((Integer) chipids[i]).intValue();
        }
        return chipIdValues;
    }

    /**
     * get The encryptedkey from the given chipId
     * 
     * @param chipId
     *            chip ID
     * @return key for given chip ID
     * @throws Exception
     *             if exception
     */
    public int[] getKey(int chipId) throws Exception {
        ChipId chip = (ChipId) mChipIds.get(Integer.valueOf(chipId));
        return chip.getEncryptionKey();
    }
}
