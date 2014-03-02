/**
 * 
 */
package com.stericsson.sdk.equipment.bootrom;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class BootRomTokensTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testGetTokenDescriptionUnknow() {
        String returns = BootRomTokens.getTokenDescription(new byte[] {
            0, 0, 5, 0, 4});
        String expected = "0x400050000: Unknown token";
        Assert.assertEquals(expected, returns);
    }

    /**
     * 
     */
    @Test
    public void testGetTokenDescription() {
        String returns = BootRomTokens.getTokenDescription(new byte[] {
            0, 4, 0, 0, 0});
        String expected = "0x400: Toc Address";
        Assert.assertEquals(expected, returns);

    }

    /**
     * 
     */
    @Test
    public void testGetTokenDescriptionEmptyArray() {
        String result = BootRomTokens.getTokenDescription(new byte[] {});
        String expected = "0x00: device code invalid";
        Assert.assertEquals(expected, result);
    }

}
