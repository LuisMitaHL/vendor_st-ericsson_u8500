package com.stericsson.sdk.equipment.m7x00.bsu;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.UnsupportedEncodingException;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Load module manifest. Parses XML describing version of the module, its features and dependencies.
 * Only basic manifest information like version, name and type are remembered, the rest of the
 * information in the xml is irrelevant for flashkit (as long as BSUHostManager is a separate tool
 * and is parsing it by its own). Raw manifest xml is stored as well, this is needed for
 * BSUHostManager verification.
 * 
 * @author xadazim
 * 
 */
public class LoadModuleManifest {

    private String name;

    private String version;

    private String type;

    /**
     * Manifest as raw data (can be converted to string).
     */
    private byte[] raw;

    /**
     * Gets name of the load module as defined in the XML (might be different than the actual file
     * name).
     * 
     * @return name of the module
     */
    public String getName() {
        return name;
    }

    /**
     * Gets version of the load module.
     * 
     * @return version of the load module
     */
    public String getVersion() {
        return version;
    }

    /**
     * Gets type of the load module (DPU or PSU currently).
     * 
     * @return type of the load module.
     */
    public String getType() {
        return type;
    }

    /**
     * Gets the XML constituting this manifest. These are raw bytes, as read from the ELF file. It
     * should be possible to convert it to a regular string though.
     * 
     * @return raw XML bytes (convertible to text)
     */
    public byte[] getRaw() {
        return raw;
    }

    /**
     * Constructs the manifest object from manifest bytes. These bytes are usually extracted from
     * load module ELF file. Manifest bytes should be convertible to text and should be a valid XML
     * description.
     * 
     * @param pManifestData
     *            raw bytes convertible to string containing XML data.
     * @throws Exception
     *             in case of parsing error
     */
    public LoadModuleManifest(byte[] pManifestData) throws Exception {
        raw = pManifestData;

        DefaultHandler dh = new DefaultHandler() {
            @Override
            public void startElement(String uri, String localName, String qName, Attributes attributes)
                throws SAXException {

                if (qName.equalsIgnoreCase("bsu")) {
                    name = attributes.getValue("id");
                    version = attributes.getValue("version");
                    type = attributes.getValue("type");
                }
            }
        };

        SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
        parser.parse(new ByteArrayInputStream(pManifestData), dh);
        if ((name == null) || (version == null) || (type == null)) {
            throw new Exception("Load module XML description parsing error. Either \"bsu\" element can not be found or "
                +"it does not contain the mandatory attributes: \"id\", \"version\", \"type\"");
        }
    }

    /**
     * Writes the load module manifest to a file.
     * 
     * @param pFile
     *            file to which the manifest should be written
     * @throws Exception
     *             in case of IO errors
     */
    public void writeToFile(File pFile) throws Exception {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(pFile);
            fos.write(raw);
        } finally {
            if (fos != null) {
                fos.close();
            }
        }
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String toString() {
        String newline = System.getProperty("line.separator");
        StringBuilder result = new StringBuilder();
        result.append(" Name=").append(name).append(newline);
        result.append(" Version=").append(version).append(newline);
        result.append(" Type=").append(type).append(newline);
        result.append(" Raw XML={").append(newline);
        try {
            result.append(new String(raw, "UTF-8"));
        } catch (UnsupportedEncodingException e) {
            result.append("Can not convert raw bytes to string using UTF-8 encoding");
        }
        result.append(newline).append("}").append(newline);
        return result.toString();
    }

}
