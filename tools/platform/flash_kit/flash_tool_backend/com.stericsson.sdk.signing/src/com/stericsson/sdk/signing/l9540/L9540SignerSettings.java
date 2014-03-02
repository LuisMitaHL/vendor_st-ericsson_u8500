package com.stericsson.sdk.signing.l9540;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;

/**
 * @author - Goran Gjorgoski(xxvs0005)  <Goran.Gjorgoski@seavus.com>
 *
 */
public class L9540SignerSettings extends GenericSignerSettings {

    /**
     * @param infile
     *            Input filename
     * @param outfile
     *            Path to the file to save in.
     * @throws SignerException
     *            If a signer settings related error occurred
     */
    public L9540SignerSettings(String infile, String outfile) throws SignerException {
        super(infile, outfile);
    }

}
