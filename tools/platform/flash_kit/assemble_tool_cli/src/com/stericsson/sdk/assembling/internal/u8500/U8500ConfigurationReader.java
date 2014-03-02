package com.stericsson.sdk.assembling.internal.u8500;

import java.io.IOException;
import java.util.List;
import java.util.Vector;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.Profile;

/**
 * The U8500ConfigurationReader will parse an XML configuration to retrieve information about the
 * entries
 * 
 * @author xolabju
 */
public class U8500ConfigurationReader extends ConfigurationReader {

    /** State when parsing flash archive */
    private static final int PARSE_FLASH_ARCHIVE = 1;

    /** State when parsing binary image */
    private static final int PARSE_RAM_IMAGE = 2;

    /** State when parsing preflash image */
    private static final int PARSE_PREFLASH_IMAGE = 3;

    /** State when parsing entry element */
    private static final int PARSE_ENTRY = 5;

    /** State for configuration parsing */
    private int state;

    /** Configuration entry instance used during entry parsing */
    private U8500ConfigurationEntry archiveEntry;

    /** String buffer instance used during character parsing */
    private StringBuffer stringBuffer;

    /** Flash archive entry attribute instance used during attribute parsing */
    private U8500ConfigurationEntryAttribute entryAttribute;

    /** A vector (list) of currently parsed entries */
    private Vector<U8500ConfigurationEntry> entries;

    private String elementToParse;

    private Profile profile;

    private String blockSize;

    private String blockPattern;

    /**
     * Constructor.
     * 
     * @param configurationFilename
     *            path to the configuration file name
     * @param elementName
     *            the element in the configuration file to parse
     * @throws AssemblerException
     *             TBD
     */
    public U8500ConfigurationReader(final String configurationFilename, String elementName) throws AssemblerException {
        elementToParse = elementName;
        entries = new Vector<U8500ConfigurationEntry>();
        state = 0;
        try {
            XMLReader reader = XMLReaderFactory.createXMLReader();
            reader.setContentHandler(this);
            reader.parse(configurationFilename);
        } catch (SAXException e) {
            throw new AssemblerException(e);
        } catch (IOException e) {
            throw new AssemblerException(e);
        }
    }

    /**
     * Called by the SAX XML parser when a start element is parsed.
     * 
     * @param uri
     *            Name space location.
     * @param local
     *            Element name (without name space prefix).
     * @param qualified
     *            Qualified element name (with name space prefix).
     * @param attributes
     *            Element attributes.
     * @throws SAXException
     *             In the case of an SAX XML parser error.
     */
    public void startElement(String uri, String local, String qualified, Attributes attributes) throws SAXException {
        if (uri.equalsIgnoreCase(URI_NAME)) {
            if ((state == 0) && local.equalsIgnoreCase(elementToParse)
                && ELEMENT_NAME_FLASH_ARCHIVE.equals(elementToParse)) {
                state = PARSE_FLASH_ARCHIVE;
            } else if ((state == 0) && local.equalsIgnoreCase(elementToParse)
                && ELEMENT_NAME_RAM_IMAGE.equals(elementToParse)) {
                state = PARSE_RAM_IMAGE;
            } else if ((state == 0) && local.equalsIgnoreCase(elementToParse)
                && ELEMENT_NAME_PREFLASH_IMAGE.equals(elementToParse)) {
                state = PARSE_PREFLASH_IMAGE;
            } else {
                startElementCtd(local, attributes);
            }
        }
    }

    /**
     * checkstyle fix
     */
    private void startElementCtd(String local, Attributes attributes) {
        if (state == PARSE_FLASH_ARCHIVE) {
            parseImageElement(local, attributes);
        } else if (state == PARSE_RAM_IMAGE) {
            parseImageElement(local, attributes);
        } else if (state == PARSE_PREFLASH_IMAGE) {
            parseImageElement(local, attributes);
        } else if (state == PARSE_ENTRY) {
            parseEntry(local, attributes);
        }
    }

    /**
     * @param local
     *            Element name.
     * @param attributes
     *            Element attributes.
     */
    private void parseEntry(String local, Attributes attributes) {
        if (local.equalsIgnoreCase(ATTRIBUTE_ELEMENT_NAME)) {
            entryAttribute = new U8500ConfigurationEntryAttribute(attributes.getValue(ATTRIBUTE_NAME_ATTRIBUTE_NAME));
            stringBuffer = new StringBuffer();
        } else if (local.equalsIgnoreCase(SOURCE_ELEMENT_NAME)) {
            stringBuffer = new StringBuffer();
        } else if (local.equalsIgnoreCase(SKIP_ELEMENT_NAME)) {
            blockSize = attributes.getValue(BLOCK_SIZE_ATTRIBUTE_NAME);
            blockPattern = attributes.getValue(BLOCK_PATTERN_ATTRIBUTE_NAME);
        }
    }

    /**
     * @param local
     *            Element name.
     * @param attributes
     *            Element attributes.
     */
    private void parseImageElement(final String local, final Attributes attributes) {
        if (local.equalsIgnoreCase(ENTRY_ELEMENT_NAME)) {
            archiveEntry =
                new U8500ConfigurationEntry(attributes.getValue(ENTRY_NAME_ATTRIBUTE_NAME), attributes
                    .getValue(ENTRY_TYPE_ATTRIBUTE_NAME));
            state = PARSE_ENTRY;
        } else if (local.equalsIgnoreCase(PROFILE_ELEMENT_NAME)) {
            try {
                profile = new Profile(attributes.getValue(PROFILE_PATH_ATTRIBUTE_NAME));
            } catch (Exception e) {
                System.err.print("\nException technical details:\n");
                e.printStackTrace();
                profile = null;
            }
        }
    }

    /**
     * Called by the SAX XML parser when a end element is parsed.
     * 
     * @param uri
     *            Name space location.
     * @param local
     *            Element name (without name space prefix).
     * @param qualified
     *            Qualified element name (with name space prefix).
     * @throws SAXException
     *             In the case of an SAX XML parser error.
     */
    public void endElement(String uri, String local, String qualified) throws SAXException {
        if (uri.equalsIgnoreCase(URI_NAME)) {
            if (state == PARSE_FLASH_ARCHIVE) {
                if (local.equalsIgnoreCase(ELEMENT_NAME_FLASH_ARCHIVE)) {
                    state = 0;
                }
            } else if (state == PARSE_ENTRY) {
                parseEntryEnd(local);
            }
        }
    }

    /**
     * @param local
     *            Element name
     */
    private void parseEntryEnd(String local) {
        if (local.equalsIgnoreCase(ENTRY_ELEMENT_NAME)) {
            entries.add(archiveEntry);
            state = PARSE_FLASH_ARCHIVE;
        } else if (local.equalsIgnoreCase(ATTRIBUTE_ELEMENT_NAME)) {
            entryAttribute.setValue(stringBuffer.toString());
            archiveEntry.addAttribute(entryAttribute);
        } else if (local.equalsIgnoreCase(SOURCE_ELEMENT_NAME)) {
            archiveEntry.setSource(stringBuffer.toString());
        } else if (local.equalsIgnoreCase(SKIP_ELEMENT_NAME)) {
            archiveEntry.setSkipUnusedBlocks(true);
            archiveEntry.setBlockSize(blockSize);
            archiveEntry.setBlockPattern(blockPattern);
        }
    }

    /**
     * Called by the SAX XML parser when character data is parsed.
     * 
     * @param ch
     *            A buffer with character data.
     * @param start
     *            Offset into buffer where parsed data begins.
     * @param length
     *            Number of characters that was parsed.
     * @throws SAXException
     *             In the case of a SAX XML parser error.
     */
    public void characters(char[] ch, int start, int length) throws SAXException {
        if (stringBuffer != null) {
            stringBuffer.append(ch, start, length);
        }
    }

    /**
     * @return Parsed flash archive entries.
     */
    public List<U8500ConfigurationEntry> getEntries() {
        return entries;
    }

    /**
     * @return Parsed profile
     */
    public Profile getProfile() {
        return profile;
    }
}
