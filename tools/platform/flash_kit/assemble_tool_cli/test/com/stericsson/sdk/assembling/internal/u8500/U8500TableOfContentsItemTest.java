package com.stericsson.sdk.assembling.internal.u8500;

import static org.junit.Assert.*;

import org.junit.Test;

/**
 * 
 * @author pkutac01
 *
 */
public class U8500TableOfContentsItemTest {

    /**
     * 
     */
    @Test
    public void testGetFileNameString() {
        U8500TableOfContentsItem item = new U8500TableOfContentsItem();
        item.setFileName("");
        assertEquals("", item.getFileNameString());
    }

}
