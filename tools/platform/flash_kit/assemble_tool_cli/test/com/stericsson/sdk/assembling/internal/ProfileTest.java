package com.stericsson.sdk.assembling.internal;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import java.io.IOException;
import java.util.Properties;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.assembling.AssemblerException;

/**
 * @author ezaptom
 * 
 */
public class ProfileTest {

    private static final String PROFILES_ROOT = "profiles.root";

    /**
     * 
     */
    @Before
    public void setUp() {
        try {
            System.setProperty(Profile.PROFILES_ROOT, ResourcePathManager.getResourceFilePath("res"));
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testNewProfileWithInvalidPath() {

        try {
            new Profile("c:/test/INVALID.mesp");
            fail("AssemblerException should be thrown");
        } catch (AssemblerException e) {
            assertEquals("Assembling failed: File 'c:/test/INVALID.mesp' not found", e.getMessage());
        } catch (IOException e) {
            fail("AssemblerException should be thrown");
        }

    }

    /**
     * 
     */
    @Test
    public void testNewProfileWithInvalidFile() {

        try {
           new Profile("@profiles/invalid_preflash_profile.mesp");
            fail("IOException should be thrown");
        } catch (AssemblerException e) {
            fail("IOException should be thrown");
        } catch (IOException e) {
            assertEquals("Could not read profile", e.getMessage());
        }

    }

    /**
     * 
     */
    @Test
    public void testNewProfileWithInvalidArrayRecord() {

        try {
           new Profile("@profiles/invalid_preflash_profile_2.mesp");
            fail("IOException should be thrown");
        } catch (AssemblerException e) {
            fail("IOException should be thrown");
        } catch (IOException e) {
            assertEquals("Could not read profile", e.getMessage());
        }

    }

    /**
     * 
     */
    @Test
    public void testGetPreflashPath() {
        try {
            Profile profile = new Profile("@profiles/preflash_profile.mesp");
            Assert.assertNotNull(profile.getPreFlashToolPath());
        } catch (Throwable t) {
            fail(t.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetPreflashPathWithInvalidFile() {
        try {
            Profile profile = new Profile("@profiles/preflash_profile_empty.mesp");
            Assert.assertNull(profile.getPreFlashToolPath());

            profile = new Profile("@profiles/preflash_profile_invalid.mesp");
            Assert.assertNull(profile.getPreFlashToolPath());
        } catch (Throwable t) {
            fail(t.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testTranslatePathWithRelativePath() {
        try {
            assertEquals(System.getProperty(PROFILES_ROOT) + "/test.mesp", Profile.translatePath("@profiles/test.mesp"));
        } catch (Throwable t) {
            fail(t.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testTranslatePathWithAbsolutePath() {
        try {
            assertEquals("c:/test/test.mesp", Profile.translatePath("c:/test/test.mesp"));
        } catch (Throwable t) {
            fail(t.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testTranslatePathWithNoRootProperty() {
        try {
            Properties sysProps = System.getProperties();
            sysProps.remove(Profile.PROFILES_ROOT);
            assertEquals("/test.mesp", Profile.translatePath("@profiles/test.mesp"));
        } catch (Throwable t) {
            fail(t.getMessage());
        }
    }
}
