package com.stericsson.sdk.equipment.u8500.test;

import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.tasks.EquipmentLoaderCommunicatioInitTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xtomzap
 * 
 */
public class EquipmentLoaderCommunicationInitTaskTest extends TestCase {

    private EquipmentLoaderCommunicatioInitTask taskInit;

    private static final int RESULT_SUCCESS = 0;

    /**
     * 
     */
    @Test
    public void testInit() {
        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        IEquipmentProfile profile = new IEquipmentProfile() {

            public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                return new Hashtable<String, EquipmentProperty>();
            }

            public String getSofwarePath(String type) {
                return null;
            }

            public String getProperty(String key) {
                return "true";
            }

            public String getPlatformFilter() {
                return null;
            }

            public String getAlias() {
                return null;
            }
        };
        eq.setProfile(profile);

        taskInit = new EquipmentLoaderCommunicatioInitTask(eq);

        taskInit.setArguments(null);
        EquipmentTaskResult result = taskInit.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
    }
}
