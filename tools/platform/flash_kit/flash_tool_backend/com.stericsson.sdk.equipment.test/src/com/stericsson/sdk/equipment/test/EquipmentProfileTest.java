package com.stericsson.sdk.equipment.test;

import org.junit.Test;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

import junit.framework.TestCase;

/**
 * @author ezaptom
 */
public class EquipmentProfileTest extends TestCase {

    /** */
    private static final String TEST_PROFILE_ALIAS = "TEST";

    /**
     * test write to xml file and read from xml file
     * 
     * @throws Exception TBD
     */
    @Test
    public void testEquipmentProfile() throws Exception {

        ServiceReference reference =
            TestFragmentActivator.getBundleContext().getServiceReference(IEquipmentProfileManagerService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }

        IEquipmentProfileManagerService service =
            (IEquipmentProfileManagerService) TestFragmentActivator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IEquipmentProfile profile = service.getProfile(TEST_PROFILE_ALIAS, true);
        if (profile == null) {
            throw new Exception("Should not get here!");
        }

        TestFragmentActivator.getBundleContext().ungetService(reference);

        assertEquals("TEST", profile.getAlias());
        assertEquals("(platform=u8500)", profile.getPlatformFilter());
        assertEquals(System.getProperty("loaders.root") + "\\loader.ldr", profile.getSofwarePath("Loader"));
    }
}
