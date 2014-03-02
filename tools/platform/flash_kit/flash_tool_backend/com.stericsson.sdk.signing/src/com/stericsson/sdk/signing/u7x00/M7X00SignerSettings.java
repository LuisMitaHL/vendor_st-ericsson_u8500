package com.stericsson.sdk.signing.u7x00;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;

/**
 * @author xtomzap
 *
 */
public class M7X00SignerSettings extends GenericSignerSettings {

    /**
     * @param infile
     *            Input filename
     * @param outfile
     *            Path to the file to save in.
     * @throws SignerException
     *            If a signer settings related error occurred
     */
    public M7X00SignerSettings(String infile, String outfile) throws SignerException {
        super(infile, outfile);
    }

}
