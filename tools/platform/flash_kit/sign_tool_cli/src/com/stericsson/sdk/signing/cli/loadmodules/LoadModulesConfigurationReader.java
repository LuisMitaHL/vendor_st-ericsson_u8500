package com.stericsson.sdk.signing.cli.loadmodules;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

import com.stericsson.sdk.signing.SignerException;

/**
 * @author xtomzap
 * 
 */
public class LoadModulesConfigurationReader extends DefaultHandler {

    private static final String TARGET_CONFIGURATION = "target_configuration";

    private static final String LOAD_MODULE = "loadmodule";

    private static final String SOURCE = "source";

    private static final String SIGN = "sign";

    private static final String ATTR_PLATFORM = "platform";

    private static final String ATTR_NAME = "name";

    private static final String ATTRIBUTE = "attribute";

    private boolean targetConfiguration = false;

    private List<LoadModuleEntry> loadModules;

    private LoadModuleEntry loadModuleEntry = null;

    private LoadModuleSignEntry signEntry = null;

    private String attributeName = null;

    /** String buffer instance used during character parsing */
    private StringBuffer stringBuffer;

    /**
     * @param configFileName path to xml configuration file
     * @throws SignerException exception when parsing fails
     */
    public LoadModulesConfigurationReader(String configFileName) throws SignerException {

        loadModules = new ArrayList<LoadModuleEntry>();

        try {
            XMLReader reader = XMLReaderFactory.createXMLReader();
            reader.setContentHandler(this);
            reader.parse(configFileName);
        } catch (SAXException e) {
            throw new SignerException(e);
        } catch (IOException e) {
            throw new SignerException(e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void characters(char[] ch, int start, int length) throws SAXException {
        if (stringBuffer != null) {
            stringBuffer.append(ch, start, length);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void endElement(String pUri, String pLocalName, String pQName) throws SAXException {
        if (pLocalName.equalsIgnoreCase(TARGET_CONFIGURATION)) {
            targetConfiguration = false;
        } else if (pLocalName.equalsIgnoreCase(LOAD_MODULE)) {
            if (!targetConfiguration && signEntry != null) {
                throw new SAXException("Unexpected end of element \"" + LOAD_MODULE + "\".");
            }
            loadModules.add(loadModuleEntry);
            loadModuleEntry = null;
        } else if (pLocalName.equalsIgnoreCase(SOURCE)) {
            if (loadModuleEntry == null) {
                throw new SAXException("Unexpected end of element \"" + SOURCE + "\".");
            }
            loadModuleEntry.addSource(stringBuffer.toString());
            stringBuffer = null;
        } else if (pLocalName.equalsIgnoreCase(SIGN)) {
            if (loadModuleEntry == null) {
                throw new SAXException("Unexpected end of element \"" + SIGN + "\".");
            }
            loadModuleEntry.setSignEntry(signEntry);
            signEntry = null;
        } else if (pLocalName.equalsIgnoreCase(ATTRIBUTE)) {
            if (signEntry == null) {
                throw new SAXException("Unexpected end of element \"" + ATTRIBUTE + "\".");
            }
            signEntry.addAttribute(attributeName, stringBuffer.toString());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void startElement(String pUri, String pLocalName, String pQName, Attributes pAtts) throws SAXException {
        if (pLocalName.equalsIgnoreCase(TARGET_CONFIGURATION)) {
            targetConfiguration = true;
        } else if (pLocalName.equalsIgnoreCase(LOAD_MODULE)) {
            if (!targetConfiguration) {
                throw new SAXException("\"" + LOAD_MODULE + "\" element needs to be enclosed in \""
                    + TARGET_CONFIGURATION + "\" element");
            }
            loadModuleEntry = new LoadModuleEntry();
        } else if (pLocalName.equalsIgnoreCase(SOURCE)) {
            if (loadModuleEntry == null) {
                throw new SAXException("\"" + SOURCE + "\" element needs to be enclosed in \"" + LOAD_MODULE + "\" element");
            }
            stringBuffer = new StringBuffer();
        } else if (pLocalName.equalsIgnoreCase(SIGN)) {
            if (loadModuleEntry == null) {
                throw new SAXException("\"" + SIGN + "\" element needs to be enclosed in \"" + LOAD_MODULE + "\" element");
            }
            if (pAtts.getIndex(ATTR_PLATFORM) == -1) {
                throw new SAXException("Attribute \"" + ATTR_PLATFORM + "\" is required in \"" + SIGN + "\" element");
            }
            signEntry = new LoadModuleSignEntry(pAtts.getValue(ATTR_PLATFORM));
        } else if (pLocalName.equalsIgnoreCase(ATTRIBUTE)) {
            if (signEntry == null) {
                throw new SAXException("\"" + ATTRIBUTE + "\" element needs to be enclosed in \"" + SIGN + "\" element");
            }
            if (pAtts.getIndex(ATTR_NAME) == -1) {
                throw new SAXException("Attribute \"" + ATTR_NAME + "\" is required in \"" + ATTRIBUTE + "\" element");
            }
            attributeName = pAtts.getValue(ATTR_NAME);
            stringBuffer = new StringBuffer();
        }
    }

    /**
     * @return list of parsed load modules entries
     */
    public List<LoadModuleEntry> getLoadModules() {
        return loadModules;
    }

    /**
     * @param pTargetConfiguration target configuration
     */
    public void setTargetConfiguration(boolean pTargetConfiguration) {
        targetConfiguration = pTargetConfiguration;
    }

    /**
     * @param pEntry entry
     */
    public void setSignEntry(LoadModuleSignEntry pEntry) {
        signEntry = pEntry;
    }

    /**
     * @param entry entry
     */
    public void setLoadModuleEntry(LoadModuleEntry entry) {
        loadModuleEntry = entry;
    }
}
