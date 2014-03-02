package com.stericsson.sdk.assembling;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import org.junit.Test;

/**
 * 
 * @author xolabju
 * 
 */
public class AssemblerSettingsTest {

    /**
     * 
     */
    @Test
    public void testAbstractAssemblerSettings() {
        AbstractAssemblerSettings settings = new AbstractAssemblerSettings() {
        };
        try {
            settings.put(IAssemblerSettings.KEY_CONFIGURATION_FILE, "testValue");
        } catch (AssemblerSettingsException e) {
            fail(e.getMessage());
        }
        assertNotNull(settings.get(IAssemblerSettings.KEY_CONFIGURATION_FILE));

        try {
            settings.put("INVALID_KEY", "test");
            fail("should not get here");
        } catch (AssemblerSettingsException e) {
            e.printStackTrace();
        }
    }
}
