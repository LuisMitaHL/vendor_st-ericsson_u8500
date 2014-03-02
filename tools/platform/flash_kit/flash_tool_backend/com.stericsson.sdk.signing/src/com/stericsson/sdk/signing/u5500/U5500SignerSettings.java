package com.stericsson.sdk.signing.u5500;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;

/**
 * @author xtomzap
 *
 */
public class U5500SignerSettings extends GenericSignerSettings {

    /**
     * @param infile
     *            Input filename
     * @param outfile
     *            Path to the file to save in.
     * @throws SignerException
     *            If a signer settings related error occurred
     */
    public U5500SignerSettings(String infile, String outfile) throws SignerException {
        super(infile, outfile);
    }

}
