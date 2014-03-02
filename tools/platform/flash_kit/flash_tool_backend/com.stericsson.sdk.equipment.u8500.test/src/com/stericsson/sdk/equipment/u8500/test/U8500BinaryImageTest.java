package com.stericsson.sdk.equipment.u8500.test;

import java.net.URL;
import java.util.Vector;

import junit.framework.TestCase;

import org.eclipse.core.runtime.FileLocator;
import org.junit.Test;

import com.stericsson.sdk.equipment.image.BinaryImage;
import com.stericsson.sdk.equipment.image.TableOfContents;
import com.stericsson.sdk.equipment.image.TableOfContentsItem;

/**
 * 
 * @author xolabju
 * 
 */
public class U8500BinaryImageTest extends TestCase {

    /** */
    public static final String BOOT_IMAGE = "/res/boot_image.bin";

    /**
     * 
     */
    @Test
    public void testCreateFromFile() {
        BinaryImage image = null;
        try {
            image = createImage();
        } catch (Exception e) {
            fail(e.getMessage());
        }

        TableOfContents toc = image.getTOC();
        assertNotNull(image);
        assertNotNull(toc.toString());

        int itemCount = toc.getItemCount();
        Vector<TableOfContentsItem> items = toc.getItems();

        assertEquals(itemCount, items.size());

        for (TableOfContentsItem item : items) {
            assertEquals(item.getSize(), image.getImageData(item.getFileNameString()).length);
        }

    }

    /**
     * 
     */
    @Test
    public void testTooManyItems() {
        BinaryImage image = null;

        try {
            image = createImage();
        } catch (Exception e) {
            fail(e.getMessage());
        }

        try {
            for (int i = 0; i < TableOfContents.MAX_NUMBER_OF_TOC_ITEMS; i++) {
                image.addImageItem(new TableOfContentsItem(), new byte[] {
                    0});
            }
            fail("should not get here");

        } catch (Exception e) {
            assertNotNull(e);
        }

    }

    /**
     * 
     */
    @Test
    public void testGetToc() {
        BinaryImage image = null;

        try {
            image = createImage();
            TableOfContents toc = image.getTOC();
            assertNotNull(toc.getData());
            Vector<TableOfContentsItem> items = toc.getItems();
            for (TableOfContentsItem item : items) {
                assertEquals(item, toc.getItem(item.getFileNameString()));
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    private BinaryImage createImage() throws Exception {
        URL url = getClass().getResource(BOOT_IMAGE);
        URL find = null;
        find = FileLocator.toFileURL(url);

        return BinaryImage.createFromFile(find.getFile());
    }
}
