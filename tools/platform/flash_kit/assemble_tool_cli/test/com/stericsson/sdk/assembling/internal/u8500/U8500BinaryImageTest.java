package com.stericsson.sdk.assembling.internal.u8500;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

import java.util.Vector;

import org.junit.Test;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.cli.AssembleTest;

/**
 * 
 * @author xolabju
 * 
 */
public class U8500BinaryImageTest {

    /**
     * 
     */
    @Test
    public void testCreateFromFile() {
        U8500BinaryImage image = null;
        try {
            image = U8500BinaryImage.createFromFile(AssembleTest.BOOT_IMAGE);
        } catch (AssemblerException e) {
            fail(e.getMessage());
        }

        U8500TableOfContents toc = image.getTOC();
        assertNotNull(image);
        assertNotNull(toc.toString());

        int itemCount = toc.getItemCount();
        Vector<U8500TableOfContentsItem> items = toc.getItems();

        assertEquals(itemCount, items.size());

        for (U8500TableOfContentsItem item : items) {
            assertEquals(item.getSize(), image.getImageData(item.getFileNameString()).length);
        }

    }

    /**
     * 
     */
    @Test
    public void testTooManyItems() {
        U8500BinaryImage image = null;
        try {
            image = U8500BinaryImage.createFromFile(AssembleTest.BOOT_IMAGE);
        } catch (AssemblerException e) {
            fail(e.getMessage());
        }

        try {
            for (int i = 0; i < U8500TableOfContents.MAX_NUMBER_OF_TOC_ITEMS; i++) {
                image.addImageItem(new U8500TableOfContentsItem(), new byte[] {
                    0});
            }
            fail("should not get here");

        } catch (AssemblerException e) {
            assertNotNull(e);
        }
    }

    /**
     * 
     */
    @Test
    public void testTableOfContents() {
        U8500TableOfContentsItem item = new U8500TableOfContentsItem();
        assertNotNull(item.toString());

        U8500TableOfContents toc = new U8500TableOfContents();
        for (int i = 0; i < U8500TableOfContents.MAX_NUMBER_OF_TOC_ITEMS; i++) {
            toc.add(new U8500TableOfContentsItem());
        }
        try {
            toc.add(new U8500TableOfContentsItem());
            fail("should not get here");
        } catch (IndexOutOfBoundsException e) {
            e.printStackTrace();
        }

        assertNull(toc.getItem("invalidItem"));
        toc = new U8500TableOfContents();
        item.setFileName("fileName");
        toc.add(item);
        assertNotNull(toc.getItem("fileName"));

        try {
            U8500TableOfContents.createTableOfContents(null);
            fail("should not get here");
        } catch (AssemblerException e) {
            e.printStackTrace();
        }
    }
}
