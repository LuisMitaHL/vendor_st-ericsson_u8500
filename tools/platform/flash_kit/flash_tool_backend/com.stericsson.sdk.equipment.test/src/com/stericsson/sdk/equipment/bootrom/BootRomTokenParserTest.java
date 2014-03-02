/**
 *
 */
package com.stericsson.sdk.equipment.bootrom;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class BootRomTokenParserTest extends TestCase {

    private File createTempFile(String content) throws IOException {
        File f = File.createTempFile("bootrom", "xml");
        f.deleteOnExit();
        PrintWriter fw = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(f), "UTF-8")));
        fw.write(content);
        fw.close();
        return f;
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.bootrom.BootRomTokenParser#parseXMLErrorCodes(java.io.File)}
     * .
     * 
     * @throws Exception .
     */
    @Test(expected = Exception.class)
    public void testParseXMLErrorCodesNull() throws Exception {
        BootRomTokenParser.parseXMLErrorCodes(null);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.bootrom.BootRomTokenParser#parseXMLErrorCodes(java.io.File)}
     * .
     * 
     * @throws Exception .
     */
    @Test(expected = Exception.class)
    public void testParseXMLErrorCodesEmpty() throws Exception {
        File emptyF = createTempFile("");
        BootRomTokenParser.parseXMLErrorCodes(emptyF);
    }

    /**
     * 
     * @throws Exception .
     */
    @Test
    public void testParseXMLErrorCodesNoTokens() throws Exception {
        File emptyF = createTempFile("<?xml version=\"1.0\"?><tokens></tokens>");
        Map<Long, String> result = BootRomTokenParser.parseXMLErrorCodes(emptyF);
        Assert.assertNull(result);
    }

    /**
     * 
     * @throws Exception .
     */
    @Test
    public void testParseXMLErrorCodesOneToken() throws Exception {

        File xmlfile =
            createTempFile("<?xml version=\"1.0\"?>" + "<tokens>"
                + "<token id=\"a\" value=\"0x00000001\" descr=\"Toc Address\" />"
                + "<token id=\"b\" value=\"2\" descr=\"blekota\" />" + "</tokens>");

        Map<Long, String> result = BootRomTokenParser.parseXMLErrorCodes(xmlfile);
        Map<Long, String> expected = new HashMap<Long, String>();
        expected.put(0x00000001L, "Toc Address");
        expected.put(2L, "blekota");
        Assert.assertTrue(expected.equals(result));
    }

    /**
     * @throws Exception .
     */
    @Test
    public void testParseXMLErrorCodesBadNumber() throws Exception {

        File xmlfile =
            createTempFile("<?xml version=\"1.0\"?>" + "<tokens>"
                + "<token id=\"a\" value=\"zz\" descr=\"Toc Address\" />" + "</tokens>");

        Map<Long, String> result = BootRomTokenParser.parseXMLErrorCodes(xmlfile);
        Map<Long, String> expected = new HashMap<Long, String>();
        Assert.assertEquals(expected, result);
    }

    /**
     * @throws Exception .
     */
    @Test
    public void testParseXMLErrorCodesNoDocument() throws Exception {

        File xmlfile =
            createTempFile("<?xml version=\"1.0\"?>" + "<tokens>"
                + "<token id=\"a\" value=\"zz\" descr=\"Toc Address\" />" + "</tokens>");

        Map<Long, String> result = BootRomTokenParser.parseXMLErrorCodes(xmlfile);
        Map<Long, String> expected = new HashMap<Long, String>();
        Assert.assertEquals(expected, result);
    }
}
