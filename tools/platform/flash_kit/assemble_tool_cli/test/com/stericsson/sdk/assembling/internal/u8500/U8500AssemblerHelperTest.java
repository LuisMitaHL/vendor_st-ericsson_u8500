package com.stericsson.sdk.assembling.internal.u8500;

import static org.junit.Assert.*;

import org.junit.Test;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;

/**
 * 
 * @author pkutac01
 * 
 */
public class U8500AssemblerHelperTest {

    private static final String DEFAULT_TARGET = "/flash0";

    /**
     * 
     */
    @Test
    public void testGetTarget() {
        // Test to get default target when there are no attributes in the entry.
        U8500ConfigurationEntry entry = new U8500ConfigurationEntry("entryName", "entryType");
        try {
            assertEquals(DEFAULT_TARGET, U8500AssemblerHelper.getTarget(entry));
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }

        // Test to get default target when there is a null attribute in the entry.
        entry.addAttribute(null);
        try {
            assertEquals(DEFAULT_TARGET, U8500AssemblerHelper.getTarget(entry));
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }

        // Test to get default target when there is a non-null attribute in the entry.
        entry.addAttribute(new U8500ConfigurationEntryAttribute("attributeName"));
        try {
            assertEquals(DEFAULT_TARGET, U8500AssemblerHelper.getTarget(entry));
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }

        // Test to get non-default target when there is a target attribute in the entry.
        entry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET));
        try {
            assertEquals(DEFAULT_TARGET, U8500AssemblerHelper.getTarget(entry));
            fail("Should not get here.");
        } catch (AssemblerException e) {
            System.out.println(e.getMessage());
        }

        // Test to get specified target attribute
        entry = new U8500ConfigurationEntry("entryName", "entryType");
        String attributeValue = DEFAULT_TARGET + "/boot";
        entry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET,
            attributeValue));
        try {
            assertEquals(attributeValue, U8500AssemblerHelper.getTarget(entry));
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }

        // Test to get specified target attribute
        entry = new U8500ConfigurationEntry("entryName", "entryType");
        String attributeValueWithOffset = attributeValue + ", 0x00000000";
        entry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET,
            attributeValueWithOffset));
        try {
            assertEquals(attributeValue, U8500AssemblerHelper.getTarget(entry));
        } catch (AssemblerException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
    }

}
