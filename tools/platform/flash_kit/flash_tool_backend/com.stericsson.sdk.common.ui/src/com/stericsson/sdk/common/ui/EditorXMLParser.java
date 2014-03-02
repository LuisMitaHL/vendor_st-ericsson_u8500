package com.stericsson.sdk.common.ui;

import java.io.InputStream;
import java.util.Locale;

import javax.management.modelmbean.XMLParseException;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

/**
 * SAX parser for profile and mesp XML files. These XML files describe how different entries in mesp
 * or prfl file should be represented in the GUI editor.
 * <p>
 * This parser can be replaced by JAXB parser after migration to Java6.
 * 
 * @author xadazim
 * 
 */
public final class EditorXMLParser {

    /**
     * Method parses XML file producing EditorXML object.
     * 
     * @param in
     *            stream with XML file contents
     * @return EditorXML object reflecting the contents of the XML file
     * @throws XMLParseException
     *             on parsing error
     */
    public static EditorXML parse(InputStream in) throws XMLParseException {
        try {
            XMLReader parser = XMLReaderFactory.createXMLReader();
            XMLContentHandler handler = new XMLContentHandler();
            parser.setContentHandler(handler);
            InputSource source = new InputSource(in);
            parser.parse(source);
            return handler.profile;
        } catch (Exception e) {
            throw new XMLParseException(e, "Profile editor xml parser error");
        }
    }

    private EditorXMLParser() {
    };
}

class XMLContentHandler implements ContentHandler {
    enum ElementNames {
        PROFILE, LAYOUT, SECTION, RECORD, ATTRIBUTE, VISUAL, OPTION;
    }

    private static final String ATTR_NAME = "name";

    private static final String ATTR_EXTENDABLE = "extendable";

    private static final String ATTR_MATCH = "match";

    private static final String ATTR_MAX_COUNT = "maxCount";

    private static final String ATTR_MIN_COUNT = "minCount";

    private static final String ATTR_TYPE = "type";

    private static final String ATTR_REGEXP_VALIDATOR = "regexpValidator";

    private static final String ATTR_SELECTED = "selected";

    private static final String ATTR_LABEL = "label";

    private static final String ATTR_VALUE = "value";

    private static final String ATTR_SHOW_GROUP = "showGroup";

    private static final String ATTR_COLUMNS = "columns";

    private static final String ATTR_COLUMNSPAN = "columnSpan";

    private static final String ATTR_LAYOUT = "layout";

    EditorXML profile;

    EditorXML.Section section;

    EditorXML.Section.Record record;

    EditorXML.Section.Record.Attribute attribute;

    EditorXML.Section.Record.Attribute.Visual visual;

    EditorXML.Section.Record.Attribute.Visual.Option option;

    boolean profileComplete = false;

    public void startPrefixMapping(String pArg0, String pArg1) throws SAXException {
    }

    public void startElement(String uri, String localName, String qName, Attributes attribs) throws SAXException {
        ElementNames element;
        try {
            element = ElementNames.valueOf(localName.toUpperCase(Locale.getDefault()));
        } catch (IllegalArgumentException e) {
            throw new SAXException("Unknown element \"" + localName + "\"");
        }

        switch (element) {
            case PROFILE:
                startProfile(attribs);
                break;
            case SECTION:
                startSection(attribs);
                break;
            case RECORD:
                startRecord(attribs);
                break;
            case ATTRIBUTE:
                startAttribute(attribs);
                break;
            case VISUAL:
                startVisual(attribs);
                break;
            case OPTION:
                startOption(attribs);
                break;
            default:
                throw new IllegalStateException("Unknown element type \"" + element + "\"");
        }
    }

    public void startDocument() throws SAXException {
    }

    public void skippedEntity(String pArg0) throws SAXException {
    }

    public void setDocumentLocator(Locator pArg0) {
    }

    public void processingInstruction(String pArg0, String pArg1) throws SAXException {
    }

    public void ignorableWhitespace(char[] pArg0, int pArg1, int pArg2) throws SAXException {
    }

    public void endPrefixMapping(String pArg0) throws SAXException {
    }

    public void endElement(String uri, String localName, String qName) throws SAXException {
        ElementNames element;
        try {
            element = ElementNames.valueOf(localName.toUpperCase(Locale.getDefault()));
        } catch (IllegalArgumentException e) {
            throw new SAXException("Unknown element \"" + localName + "\"");
        }

        switch (element) {
            case PROFILE:
                endProfile();
                break;
            case SECTION:
                endSection();
                break;
            case RECORD:
                endRecord();
                break;
            case ATTRIBUTE:
                endAttribute();
                break;
            case VISUAL:
                endVisual();
                break;
            case OPTION:
                endOption();
                break;
            default:
                throw new IllegalStateException("Unknown element type \"" + element + "\"");
        }
    }

    public void endDocument() throws SAXException {
        if (!profileComplete) {
            throw new SAXException("Element \"profile\" was not closed");
        }
    }

    public void characters(char[] ch, int start, int length) throws SAXException {
        String chrs = new String(ch, start, length).trim();
        if (chrs.length() != 0) {
            throw new SAXException("Characters not allowed: >" + new String(ch, start, length) + "<");
        }
    }

    // -----------------

    private void startProfile(Attributes attribs) throws SAXException {
        if (profile != null) {
            throw new SAXException("Multiple \"profile\" elements");
        }
        profile = new EditorXML();
    }

    private void startSection(Attributes attribs) throws SAXException {
        if (section != null) {
            throw new SAXException("Nested \"section\" elements");
        }
        if (profile == null) {
            throw new SAXException("\"section\" element needs to be enclosed in \"profile\" element");
        }
        section = new EditorXML.Section();

        if (-1 == attribs.getIndex(ATTR_NAME)) {
            throw new SAXException("Attribute \"" + ATTR_NAME + "\" is required in \"section\" element");
        }
        section.name = attribs.getValue(ATTR_NAME);

        section.extendable = Boolean.valueOf(attribs.getValue(ATTR_EXTENDABLE));

        int columns = 1;
        if (-1 != attribs.getIndex(ATTR_COLUMNS)) {
            columns = Integer.valueOf(attribs.getValue(ATTR_COLUMNS));
        }
        section.columns = columns;

        int columnSpan = 1;
        if (-1 != attribs.getIndex(ATTR_COLUMNSPAN)) {
            columnSpan = Integer.valueOf(attribs.getValue(ATTR_COLUMNSPAN));
        }
        section.columnSpan = columnSpan;

        String layout = "";
        if (-1 != attribs.getIndex(ATTR_LAYOUT)) {
            layout = attribs.getValue(ATTR_LAYOUT);
        }
        section.layout = layout;
    }

    private void startRecord(Attributes attribs) throws SAXException {
        if (record != null) {
            throw new SAXException("Nested \"record\" elements");
        }
        if (section == null) {
            throw new SAXException("\"record\" element needs to be enclosed in \"section\" element");
        }
        record = new EditorXML.Section.Record();

        if (-1 == attribs.getIndex(ATTR_MATCH)) {
            throw new SAXException("Attribute \"" + ATTR_MATCH + "\" is required in \"record\" element");
        }
        record.match = attribs.getValue(ATTR_MATCH);

        if (-1 != attribs.getIndex(ATTR_LABEL)) {
            record.label = attribs.getValue(ATTR_LABEL);
        }

        if (-1 != attribs.getIndex(ATTR_NAME)) {
            record.name = attribs.getValue(ATTR_NAME);
        }

        if (-1 == attribs.getIndex(ATTR_MAX_COUNT)) {
            throw new SAXException("Attribute \"" + ATTR_MAX_COUNT + "\" is required in \"record\" element");
        }
        record.maxCount = Integer.valueOf(attribs.getValue(ATTR_MAX_COUNT));

        if (-1 == attribs.getIndex(ATTR_MIN_COUNT)) {
            throw new SAXException("Attribute \"" + ATTR_MIN_COUNT + "\" is required in \"record\" element");
        }
        record.minCount = Integer.valueOf(attribs.getValue(ATTR_MIN_COUNT));

        record.showGroup = Boolean.valueOf(attribs.getValue(ATTR_SHOW_GROUP));

        int columns = 1;
        if (-1 != attribs.getIndex(ATTR_COLUMNS)) {
            columns = Integer.valueOf(attribs.getValue(ATTR_COLUMNS));
        }
        record.columns = columns;
    }

    private void startAttribute(Attributes attribs) throws SAXException {
        if (attribute != null) {
            throw new SAXException("Nested \"attribute\" elements");
        }
        if (record == null) {
            throw new SAXException("\"attribute\" element needs to be enclosed in \"record\" element");
        }
        attribute = new EditorXML.Section.Record.Attribute();

        if (-1 == attribs.getIndex(ATTR_NAME)) {
            throw new SAXException("Attribute \"" + ATTR_NAME + "\" is required in \"attribute\" element");
        }
        attribute.name = attribs.getValue(ATTR_NAME);
    }

    private void startVisual(Attributes attribs) throws SAXException {
        if (visual != null) {
            throw new SAXException("Nested \"visual\" elements");
        }
        if (attribute == null) {
            throw new SAXException("\"visual\" element needs to be enclosed in \"attribute\" element");
        }
        visual = new EditorXML.Section.Record.Attribute.Visual();

        if (-1 == attribs.getIndex(ATTR_LABEL)) {
            throw new SAXException("Attribute \"" + ATTR_LABEL + "\" is required in \"visual\" element");
        }
        visual.label = attribs.getValue(ATTR_LABEL);

        if (-1 == attribs.getIndex(ATTR_TYPE)) {
            throw new SAXException("Attribute \"" + ATTR_TYPE + "\" is required in \"visual\" element");
        }
        try {
            visual.type = VisualType.valueOf(attribs.getValue(ATTR_TYPE));
        } catch (IllegalArgumentException e) {
            throw new SAXException("Unknown visual type \"" + attribs.getValue(ATTR_TYPE) + "\"");
        }

        String regexp = ".*";
        if (-1 != attribs.getIndex(ATTR_REGEXP_VALIDATOR)) {
            regexp = attribs.getValue(ATTR_REGEXP_VALIDATOR);
        }
        visual.regexpValidator = regexp;

        String selected = "true";
        if (-1 != attribs.getIndex(ATTR_SELECTED)) {
            selected = attribs.getValue(ATTR_SELECTED);
        }
        visual.selected = selected;

    }

    private void startOption(Attributes attribs) throws SAXException {
        if (option != null) {
            throw new SAXException("Nested \"option\" elements");
        }
        if (visual == null) {
            throw new SAXException("\"option\" element needs to be enclosed in \"visual\" element");
        }
        option = new EditorXML.Section.Record.Attribute.Visual.Option();

        if ((visual.getType() == VisualType.COMBO) && (-1 == attribs.getIndex(ATTR_LABEL))) {
            throw new SAXException("Attribute \"" + ATTR_LABEL + "\" is required in \"option\" element");
        }
        option.label = attribs.getValue(ATTR_LABEL);

        if (-1 == attribs.getIndex(ATTR_VALUE)) {
            throw new SAXException("Attribute \"" + ATTR_VALUE + "\" is required in \"option\" element");
        }
        option.value = attribs.getValue(ATTR_VALUE);
    }

    // -----------------

    private void endProfile() throws SAXException {
        if (profile == null) {
            throw new SAXException("End of element \"profile\" without start");
        }
        profileComplete = true;
    }

    private void endSection() throws SAXException {
        if (section == null) {
            throw new SAXException("End of element \"section\" without start");
        }
        if (section.isExtendable() && section.getRecords().size() > 1) {
            for (EditorXML.Section.Record r : section.getRecords()) {
                if (r.getLabel() == null) {
                    throw new SAXException("\"label\" attribute is required in \"record\" element when \"section\" "
                        + "is extendable and contains more then one record type (section: \"" + section.getName()
                        + "\" record match: \"" + r.getMatch() + "\")");
                }
            }
        }
        profile.getSections().add(section);
        section = null;
    }

    private void endRecord() throws SAXException {
        if (record == null) {
            throw new SAXException("End of element \"record\" without start");
        }
        section.getRecords().add(record);
        record = null;
    }

    private void endAttribute() throws SAXException {
        if (attribute == null) {
            throw new SAXException("End of element \"attribute\" without start");
        }
        record.getAttributes().add(attribute);
        attribute = null;
    }

    private void endVisual() throws SAXException {
        if (visual == null) {
            throw new SAXException("End of element \"visual\" without start");
        }
        attribute.visual = visual;
        visual = null;
    }

    private void endOption() throws SAXException {
        if (option == null) {
            throw new SAXException("End of element \"option\" without start");
        }
        visual.getOptions().add(option);
        option = null;
    }
}
