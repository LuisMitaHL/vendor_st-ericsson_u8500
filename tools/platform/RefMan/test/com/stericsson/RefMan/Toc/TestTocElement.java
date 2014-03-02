/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.util.List;
import java.util.Vector;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.RefMan.UmlExportFilter.TestUmlExport;

public class TestTocElement {

    private TopicElement child1;
    private TopicElement child2;
    private TocElement elmCorrect1;
    private TocElement elmCorrect2;
    private TocElement elmIncorrect;
    private TocElement elmNoTopics;

    /**
     * Used by TestRemoveInterfaces() tests
     */
    final String testFilesDirName = "test//com//stericsson//RefMan//Toc//removeinterfaces-files";

    @Before
    public void setUp() throws Exception {
        child1 = new TopicElement();
        child1.setLabel("child");
        Vector<Element> v1 = new Vector<Element>();
        v1.add(child1);
        elmCorrect1 = new TocElement();
        elmCorrect1.setLabel("label_correct");
        elmCorrect1.setHref("href_correct");
        elmCorrect1.setTopics(v1);
        child2 = new TopicElement();
        child2.setLabel("child");
        Vector<Element> v2 = new Vector<Element>();
        v2.add(child2);
        elmCorrect2 = new TocElement();
        elmCorrect2.setLabel("label_correct");
        elmCorrect2.setHref("href_correct");
        elmCorrect2.setTopics(v2);
        elmIncorrect = new TocElement();
        elmIncorrect.setLabel("label_correct");
        elmNoTopics = new TocElement();
        elmNoTopics.setLabel("label_correct");
        elmNoTopics.setHref("href_correct");
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.TocElement#equals(Object)}.
     */
    @Test
    public void testEquals() {
        assertTrue(elmCorrect1.equals(elmCorrect2));
        assertTrue(!elmCorrect1.equals(elmIncorrect));
        assertTrue(!elmCorrect1.equals(elmNoTopics));

    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TocElement#hashCode()}.
     */
    @Test
    public void testHashcode() {
        assertTrue(elmCorrect1.hashCode() == elmCorrect2.hashCode());
        assertTrue(elmCorrect1.hashCode() != elmIncorrect.hashCode());
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TocElement#toString()}.
     */
    @Test
    public void testToString() {
        String correct = "type: TOC; label: label_correct; href: href_correct; topics: 1";
        String string = elmCorrect1.toString();

        assertTrue(string.equals(correct));
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TocElement#toString()}.
     *
     * No value for href and no topics added.
     */
    @Test
    public void testToStringNoHrefNoTopics() {
        String correct = "type: TOC; label: label_correct; href: ; topics: 0";
        String string = elmIncorrect.toString();

        assertTrue(string.equals(correct));
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TocElement#getType()}.
     *
     * Test getType() to return 'TOC'.
     */
    @Test
    public void testGetType() {
        assertTrue(elmCorrect1.getType().equals("TOC"));
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_first() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-cbs+ss"));

        String[] expectedInterfaceNames = { "Telephony/User space/Cell Broadcast Server" };

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("SMS Server");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_second() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-cbs+ss"));

        String[] expectedInterfaceNames = { "Telephony/User space/SMS Server" };

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("Cell Broadcast Server");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_both() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-cbs+ss"));

        String[] expectedInterfaceNames = {};

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("Cell Broadcast Server");
        interfaceNamesToRemove_SS.add("SMS Server");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_twostacks_adm() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-adm+cbs+ss"));

        String[] expectedInterfaceNames = { "Telephony/User space/SMS Server",
                "Telephony/User space/Cell Broadcast Server" };

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("Audio Device Manager");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_twostacks_cbs() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-adm+cbs+ss"));

        String[] expectedInterfaceNames = { "Telephony/User space/SMS Server",
                "Multimedia/User space/Audio Device Manager" };

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("Cell Broadcast Server");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<
     * java.lang.String>)}.
     *
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     */
    @Test
    public void TestRemoveInterfaces_twostacks_ss() {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-adm+cbs+ss"));

        String[] expectedInterfaceNames = {
                "Telephony/User space/Cell Broadcast Server",
                "Multimedia/User space/Audio Device Manager" };

        Vector<String> interfaceNamesToRemove_SS = new Vector<String>();
        interfaceNamesToRemove_SS.add("SMS Server");

        API.removeInterfaces(interfaceNamesToRemove_SS);
        List<String> foundInterfaceNames = API.getInterfaceNames();
        TestUmlExport.checkNamesAgainstExpected(foundInterfaceNames,
                expectedInterfaceNames, "API.removeInterfaces('SMS Server')");
    }

}
