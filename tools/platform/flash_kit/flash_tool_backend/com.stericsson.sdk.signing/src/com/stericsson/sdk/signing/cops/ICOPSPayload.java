package com.stericsson.sdk.signing.cops;

import com.stericsson.sdk.signing.IPayload;

/**
 * Interface for COPS payload.
 * 
 * @author xtomlju
 */
public interface ICOPSPayload extends IPayload {

    /**
     * @param fields
     *            TBD
     */
    void updateCOPSControlFields(COPSControlFields fields);
}
