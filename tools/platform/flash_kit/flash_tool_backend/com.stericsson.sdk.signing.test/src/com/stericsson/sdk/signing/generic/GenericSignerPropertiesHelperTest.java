package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.test.Activator;

import junit.framework.TestCase;


/**
 * @author Xxvs0002
 *
 */
public class GenericSignerPropertiesHelperTest extends TestCase{

    private static final String UNCHECKED_BLOB_FILENAME = "/test.txt";

    /**
     * 
     */
    public void testReadUncheckedBlob(){
        try {
            GenericSignerPropertiesHelper.readUncheckedBlob(getResourceFilePath(UNCHECKED_BLOB_FILENAME));
        } catch (SignerException e) {
            e.printStackTrace();
            fail("Should not get here");
        }
    }

    private String getResourceFilePath(String filepath) {
        return Activator.getResourcesPath() + filepath;
    }
}
