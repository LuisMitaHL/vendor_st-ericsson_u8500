package com.stericsson.sdk.signing.cli.loadmodules;

import org.junit.Assert;
import org.junit.Test;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.signing.SignerException;

/**
 * @author xtomzap
 * 
 */
public class LoadModulesConfigurationReaderTest {

    private static final String CONFIG_FILE_FILENAME_CORRUPTED =
        ResourcePathManager.getResourceFilePath("test_files/loadmodules/loadmodules_corrupted.xml");

    private static final String CONFIG_FILE_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/loadmodules/loadmodules.xml");

    /***/
    @Test
    public void testEndElement() {
        LoadModulesConfigurationReader reader;
        try {
            reader = new LoadModulesConfigurationReader("");
        } catch (SignerException e) {
            Assert.assertTrue(true);
        }

        try {
            reader = new LoadModulesConfigurationReader(CONFIG_FILE_FILENAME_CORRUPTED);
        } catch (SignerException e) {
            Assert.assertTrue(true);
        }

        try {
            reader = new LoadModulesConfigurationReader(CONFIG_FILE_FILENAME);
        } catch (SignerException e) {
            Assert.assertTrue(false);
        }

        try {
            reader = new LoadModulesConfigurationReader(CONFIG_FILE_FILENAME);
            reader.setTargetConfiguration(false);
            reader.setSignEntry(new LoadModuleSignEntry("platform"));
            reader.endElement("pUri", "loadmodule", "pQName");
        } catch (SignerException e) {
            Assert.assertTrue(false);
        } catch (SAXException e) {
            Assert.assertTrue(true);
        }

        try {
            reader = new LoadModulesConfigurationReader(CONFIG_FILE_FILENAME);
            reader.setTargetConfiguration(false);
            reader.setSignEntry(null);
            reader.endElement("pUri", "loadmodule", "pQName");

            try {
                reader.endElement("pUri", "source", "pQName");
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.endElement("pUri", "sign", "pQName");
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.endElement("pUri", "attribute", "pQName");
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            reader.endElement("pUri", "fake", "pQName");
        } catch (SignerException e) {
            Assert.assertTrue(false);
        } catch (SAXException e) {
            Assert.assertTrue(false);
        }
    }

    /***/
    @Test
    public void testStartElement() {
        LoadModulesConfigurationReader reader;

        try {
            reader = new LoadModulesConfigurationReader(CONFIG_FILE_FILENAME);
            reader.setTargetConfiguration(false);
            // reader.setSignEntry(new LoadModuleSignEntry("platform"));
            try {
                reader.startElement("pUri", "loadmodule", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.startElement("pUri", "source", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.startElement("pUri", "sign", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.startElement("pUri", "attribute", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            try {
                reader.startElement("pUri", "fake", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            reader.setLoadModuleEntry(new LoadModuleEntry());
            try {
                reader.startElement("pUri", "sign", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

            reader.setSignEntry(new LoadModuleSignEntry("platform"));
            try {
                reader.startElement("pUri", "attribute", "pQName", getAttributes());
            } catch (SAXException e) {
                Assert.assertTrue(true);
            }

        } catch (SignerException e) {
            Assert.assertTrue(false);
        }
    }

    private Attributes getAttributes() {
        return new Attributes() {

            public String getValue(String pUri, String pLocalName) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getValue(String pQName) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getValue(int pIndex) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getURI(int pIndex) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getType(String pUri, String pLocalName) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getType(String pQName) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getType(int pIndex) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getQName(int pIndex) {
                // TODO Auto-generated method stub
                return null;
            }

            public String getLocalName(int pIndex) {
                // TODO Auto-generated method stub
                return null;
            }

            public int getLength() {
                // TODO Auto-generated method stub
                return 0;
            }

            public int getIndex(String pUri, String pLocalName) {
                // TODO Auto-generated method stub
                return 0;
            }

            public int getIndex(String pQName) {
                return -1;
            }
        };
    }
}
