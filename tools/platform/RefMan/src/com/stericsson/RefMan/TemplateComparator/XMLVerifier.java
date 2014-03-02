/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.TemplateComparator;

import java.io.File;
import java.util.List;

import org.jdom.Attribute;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.input.SAXBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A class that verifies XML documents by comparing them to templates in xml
 * files
 */
public class XMLVerifier {

    /** logger */
    final static Logger logger = LoggerFactory.getLogger(XMLVerifier.class);

    /** examined file name holder - for error printouts */
    String fileName = null;

    /**
     * Searches the sub elements of <code>elemRoot</code> to find an element
     * that matches <code>templateElem</code>.
     *
     * @param templateElem
     *            template element used as a pattern
     * @param elemRoot
     *            root element, children of this element are traversed
     * @return matching element if found, otherwise null
     */
    private Element findSubElement(Element templateElem, Element elemRoot) {

        /** First find elements with a matching name */
        /** JDom doesn't support generics. */
        @SuppressWarnings("unchecked")
        List<Element> subElems = elemRoot.getChildren(templateElem.getName());

        /**
         * Scan all elements with a matching name, if matching element is found,
         * return true
         */
        for (int i = 0; i < subElems.size(); i++) {
            Element elem = subElems.get(i);
            if (compareElementAndAttributesToTmp(templateElem, elem)) {
                return elem;
            }
        }
        return null;
    }

    /**
     * Compares element <code>elem</code> to a template element
     * <code>tmpElem</code>. The element <code>elem</code> is considered
     * matching only if it has the same name and contains all the attributes
     * that the template component has (can though contain more)
     *
     * @param templateElem
     *            template element used as a pattern
     * @param elem
     *            element being compared to the template
     * @return true if element is matching, false otherwise
     */
    private boolean compareElementAndAttributesToTmp(Element templateElem,
            Element elem) {

        // if the name doesn't match then return error
        if (!templateElem.getName().equalsIgnoreCase(elem.getName())) {
            return false;
        }
        // check if template element has any attributes
        @SuppressWarnings("unchecked")
        // JDom does not support generics
        List<Attribute> tmpAttributes = templateElem.getAttributes();

        // if it does not then match - return first element from the list
        if (tmpAttributes.size() == 0) {
            return true;
        }

        // if attributes present in the template then find elem which matches on
        // all attributes
        boolean allAttributesPresent = true;
        for (int i = 0; i < tmpAttributes.size(); i++) {
            Attribute tmpAttr = tmpAttributes.get(i);
            // check if name and value matches
            Attribute elemAttr = elem.getAttribute(tmpAttr.getName());
            if (elemAttr == null)
                allAttributesPresent = false;
            else {
                if (!elemAttr.getValue().equalsIgnoreCase(tmpAttr.getValue()))
                    allAttributesPresent = false;
            }
        }
        return allAttributesPresent;

    }

    /**
     * Returns "path" to the element, all predecessors are listed in xml style
     *
     * @param e
     *            element
     * @return list of all predecessors in xml style
     */
    private String getPath(Element e) {
        String path = "";
        Element parent = e.getParentElement();
        while (parent != null) {
            path = getSyntax(parent) + path;
            parent = parent.getParentElement();
        }
        return path;

    }

    /**
     * Returns the actual syntax (xml style) for the <code>Element</code>
     * including all the attributes
     *
     * @param e
     *            element
     * @return element syntax
     */
    private String getSyntax(Element e) {
        String text = "<" + e.getName();

        @SuppressWarnings("unchecked")
        // JDom does not support generics
        List<Attribute> atts = e.getAttributes();
        for (int i = 0; i < atts.size(); i++) {
            Attribute a = atts.get(i);
            text = text + " " + a.getName() + "=\"" + a.getValue() + "\"";
        }

        text = text + ">";
        return text;
    }

    /**
     * Returns message to be produced when an element from template is not found
     *
     * @param e
     *            element
     * @return error message
     */
    private String missingElementErrorMessage(Element e) {
        String errorMsg;
        /**
         * for readability reasons the message varies for root element and other
         * elements
         */
        if (!e.isRootElement()) {
            errorMsg = "In file: " + fileName + " missing element: "
                    + getSyntax(e) + " that is expected in: " + getPath(e);
        } else {
            errorMsg = "In file: " + fileName + " missing root element: "
                    + getSyntax(e);
        }
        return errorMsg;

    }

    /**
     * Compares xml element to a template element. The element is considered
     * matching when the actual element is matching (same name and all
     * attributes) as well as all elements children are matching. The method
     * reports all errors that are not related to other errors (e.g. in case an
     * element with a number of subelements is required, and the root element is
     * missing then only the missing root element will be reported)
     *
     * @param templateElem
     *            template element
     * @param elem
     *            element to compare to the template
     * @return true is element matches the template, false otherwise
     */
    private boolean compareElementToTemplate(Element templateElem, Element elem) {

        if (!compareElementAndAttributesToTmp(templateElem, elem)) {
            logger.error(missingElementErrorMessage(templateElem));
            return false;
        }

        @SuppressWarnings("unchecked")
        // JDom does not support generics
        List<Element> subTmpElems = templateElem.getChildren();
        boolean allSubElementsPresent = true;

        for (int i = 0; i < subTmpElems.size(); i++) {
            Element tmpSubElem = subTmpElems.get(i);
            Element subElem = findSubElement(tmpSubElem, elem);
            if (subElem == null) {
                logger.error(missingElementErrorMessage(tmpSubElem));
                allSubElementsPresent = false;
            } else {
                if (!compareElementToTemplate(tmpSubElem, subElem)) {
                    allSubElementsPresent = false;
                }
            }

        }

        return allSubElementsPresent;
    }

    /**
     * Front end method for the verifier. Takes in template and a file to
     * verify, returns true in case the file matches the template, false in case
     * of error or no match. the template file is a standard xml file that
     * contains only mandatory tags, attributes and their values. File matches
     * the template when it contains at least all the elements that the template
     * has in the same places in the structure (additional elements are
     * allowed). Comments and element values (e.g. text) in the template are
     * ignored. The order of tags on the same level is not significant.
     *
     * @param templateXmlFileName
     *            template xml file
     * @param xmlFileName
     *            file to verify
     * @return true if file matches template, false in other cases
     */
    public boolean compareFileToTemplateFile(File templateXmlFileName,
            File xmlFileName) {

        // open template file
        Element templateRoot;
        Document templateXML = null;
        Element root;
        Document xml = null;
        fileName = xmlFileName.getAbsolutePath();
        SAXBuilder builder = new SAXBuilder();
        builder
                .setFeature(
                        "http://apache.org/xml/features/nonvalidating/load-external-dtd",
                        false);
        try {
            templateXML = builder.build(templateXmlFileName);
            templateRoot = templateXML.getRootElement();
        } catch (Exception e) {

            logger.error("Error reading template xml file "
                    + templateXmlFileName.getAbsolutePath() + ". Error: "
                    + e.getMessage());
            return false;
        }

        try {
            xml = builder.build(xmlFileName);
            root = xml.getRootElement();
        } catch (Exception e) {
            logger.error("Error reading xml file "
                    + xmlFileName.getAbsolutePath() + ". Error: "
                    + e.getMessage());
            return false;
        }

        return compareElementToTemplate(templateRoot, root);
    }

    /**
     * Front end method for the verifier. Takes in template and an
     * <code>Element</code> to verify, returns true in case the
     * <code>Element</code> matches the template, false in case of error or no
     * match. The template file is a standard xml file that contains only
     * mandatory tags, attributes and their values. <code>Element</code> matches
     * the template when it contains at least all the elements that the template
     * has in the same places in the structure (additional elements are
     * allowed). Comments and element values (e.g. text) in the template are
     * ignored. The order of tags on the same level is not significant.
     *
     * @param templateXmlFileName
     *            template xml file
     * @param xmlRootElement
     *            <code>Element</code> to verify
     * @return true if file matches template, false in other cases
     */
    public boolean compareElementToTemplateFile(File templateXmlFileName,
            Element xmlRootElement, String fileName) {

        this.fileName = fileName;
        // open template file
        Element templateRoot;
        Document templateXML = null;
        SAXBuilder builder = new SAXBuilder();
        builder
                .setFeature(
                        "http://apache.org/xml/features/nonvalidating/load-external-dtd",
                        false);
        try {
            templateXML = builder.build(templateXmlFileName);
            templateRoot = templateXML.getRootElement();
            templateRoot.removeAttribute("id");
        } catch (Exception e) {

            logger.error("Error reading template xml file "
                    + templateXmlFileName.getAbsolutePath() + ". Error: "
                    + e.getMessage());
            return false;
        }

        return compareElementToTemplate(templateRoot, xmlRootElement);
    }
}
