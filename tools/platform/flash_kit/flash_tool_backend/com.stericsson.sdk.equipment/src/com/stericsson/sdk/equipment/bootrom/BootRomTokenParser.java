package com.stericsson.sdk.equipment.bootrom;

import java.io.File;
import java.util.LinkedHashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * 
 * @author xolabju
 * 
 */
final class BootRomTokenParser {

    private static final String XML_TAG_TOKEN = "token";

    private static final String XML_ATTRIBUTE_VALUE = "value";

    private static final String XML_ATTRIBUTE_DESCR = "descr";

    private BootRomTokenParser() {
    }

    /**
     * 
     * @param xmlFile
     *            the xml file
     * @return the parsed values as a map
     * @throws Exception
     *             on errors
     */
    static Map<Long, String> parseXMLErrorCodes(File xmlFile) throws Exception {
        Document document = getDocument(xmlFile);
        Map<Long, String> map = null;

        if (document != null) {
            document.normalize();
            NodeList nl = document.getElementsByTagName(XML_TAG_TOKEN);

            if (nl != null && nl.getLength() > 0) {
                map = parseNodeList(nl);
            }
        }
        return map;
    }

    private static Document getDocument(File file) throws Exception {

        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db;
        Document dom = null;
        db = dbf.newDocumentBuilder();
        dom = db.parse(file);
        return dom;
    }

    private static Map<Long, String> parseNodeList(NodeList nl) {
        Map<Long, String> map = new LinkedHashMap<Long, String>();
        for (int i = 0; i < nl.getLength(); i++) {
            Node n = nl.item(i);
            if (n != null && n instanceof Element) {
                Element el = (Element) n;
                String valueStr = el.getAttribute(XML_ATTRIBUTE_VALUE);
                long value = 0;
                String descr = el.getAttribute(XML_ATTRIBUTE_DESCR);
                try {
                    if (valueStr != null) {
                        if (!valueStr.startsWith("0x")) {
                            valueStr = "0x" + valueStr;
                        }
                        value = Long.decode(valueStr);
                        map.put(value, descr.trim());
                    }
                } catch (NumberFormatException nfe) {
                    nfe.printStackTrace();
                }
            }
        }
        return map;
    }
}
