package com.stericsson.sdk.assembling.internal.u8500;

import static org.junit.Assert.*;

import org.junit.Test;

/**
 * 
 * @author pkutac01
 *
 */
public class U8500TableOfContentsTest {

    /**
     * 
     */
    @Test
    public void testCreateTableOfContents() {
        try {
            assertNull(U8500TableOfContents.createTableOfContents(new byte[] {}));
            fail("Should not get here");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
