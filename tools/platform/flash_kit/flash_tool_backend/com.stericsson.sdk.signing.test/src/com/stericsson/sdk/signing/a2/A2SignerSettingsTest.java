package com.stericsson.sdk.signing.a2;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.test.Activator;

import junit.framework.TestCase;


/**
 * @author Xxvs0002
 *
 */
public class A2SignerSettingsTest extends TestCase{

    private static final String INPUT_FILE = "/a2/CXC1725333_R1E.bin";
    /**
     * 
     */
    public void testSetFrom(){
        try {
            String inputFile = Activator.getResourcesPath() + INPUT_FILE;

            ISignerSettings signerSettings = new A2SignerSettings(inputFile, "test.out");

            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE, "testSW");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_DEBUG, "true");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU, "testCPU");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_EXT_LEVEL, "1");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_ETX_LEVEL_IN_HEADER, "11");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT, "111");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC, "set");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_FORMAT, "format");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_HDR_SECURITY, "security");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS, "address");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE, "ptype");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SHORT_MAC, "mac");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, "swtype");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, "mode");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE, "swversionmac");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_ANTI_ROLL_REQUIRED, "antiroll");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION, "swversion");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION, "loader");
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION, "loaderenc");

            ISignerSettings signerSettingsToSet = new A2SignerSettings(inputFile, "test.out");
            signerSettingsToSet.setFrom(signerSettings);

        } catch (SignerSettingsException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }
}
