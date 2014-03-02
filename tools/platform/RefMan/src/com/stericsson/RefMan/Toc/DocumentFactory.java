/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.io.File;

import org.jdom.Attribute;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Namespace;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class is responsible for creating <code>JDOM documents</code> from a
 * <code>com.stericsson.RefMan.Toc.Element</code> tree structure.
 *
 * @author Thomas Palmqvist
 *
 */
public class DocumentFactory {

    /** The schema to validate the output toc file */
    final String namespace_API_output_Toc = "http://www.stericsson.com/refman/API_output_Toc.xsd";

    /** The logger */
    final Logger logger = LoggerFactory.getLogger(DocumentFactory.class);

    private final static String TOCType = "TOC";

    /**
     * The <code>Document</code> that will be populated by the
     * <code>Element</code> objects when generating the xml document.
     */
    private Document jDomApiTocXml = null;

    /**
     * The <code>Document</code> that will be populated by the
     * <code>Element</code> objects when generating a <code>HTML</code>
     * document.
     */
    private Document jDomApiTocHtml = null;

    /** An <code>Element</code> that is used in <code>getInApiToc</code> methods */
    private Element found;

    /**
     * Constructor for the <code>DocumentFactory</code>
     */
    public DocumentFactory() {
    }

    /**
     * Method to generate and return a <code>JDOM Document</code> that contains
     * the API toc from the tree structure representing the toc files.
     *
     * @param elm
     *            The root element to convert to a <code>JDOM Document</code>.
     * @param levelOneLabel
     *            The <code>String</code> that will be set as <code>label</code>
     *            attribute for first level <code>Element</code>.
     * @return An <code>JDOM Document</code> that contains the API Toc
     * @throws Exception
     */
    public Document getXmlToc(com.stericsson.RefMan.Toc.Element elm,
            String levelOneLabel) {
        Element root = new Element("toc", Namespace
                .getNamespace(namespace_API_output_Toc));
        root.setAttribute(new Attribute("label", levelOneLabel));
        jDomApiTocXml = new Document(root);
        populateXmlToc(elm);
        return jDomApiTocXml;
    }

    /**
     * Method to generate and return a <code>JDOM Document</code> that contains
     * the API toc in <code>HTML</code> format from the tree structure
     * representing the toc files.
     *
     * @param elm
     *            The root element to convert to a <code>JDOM Document</code>.
     * @param levelOneLabel
     *            The <code>String</code> that will be set in the first level
     *            header of the <code>html</code> document.
     * @return A <code>JDOM Document</code> that contains the API Toc in
     *         <code>HTML</code> format.
     */
    public Document getHtmlToc(com.stericsson.RefMan.Toc.Element elm,
            String levelOneLabel) {
        Element htmlRoot = new Element("html");
        Element head = new Element("head");
        Element link = new Element("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", "html/style.css");
        link.setAttribute("type", "text/css");
        head.addContent(link);
        htmlRoot.addContent(head);
        Element body = new Element("body");
        body.setAttribute("class", "mainindexpage");
        Element logo = new Element("img");
        logo.setAttribute("src", "images/stelogo.png");
        logo.setAttribute("align", "right");
        body.addContent(logo);
        htmlRoot.addContent(body);
        populateHtmlDoc(body, elm, levelOneLabel, 1);
        jDomApiTocHtml = new Document(htmlRoot);
        return jDomApiTocHtml;
    }

    /**
     * Populates the <code>JDOM Document</code> with the <code>Element</code>
     * tocElm's contents.
     * <p>
     * Uses recursion to populate with child <code>Elements</code> to.
     *
     * @param tocElm
     *            The <code>Element</code> with the content.
     */
    private void populateXmlToc(com.stericsson.RefMan.Toc.Element tocElm) {
        for (com.stericsson.RefMan.Toc.Element elm : tocElm.getTopics()) {
            if (elm instanceof TocElement) {
                return;
            }
            Element topic = new Element(elm.getType().compareTo(TOCType) == 0
                    ? "toc"
                    : "topic", jDomApiTocXml.getRootElement().getNamespace());
            topic.setAttribute(new Attribute("label", elm.getLabel()));
            // If the element is a stdApi element, the documentation is placed
            // in moduleName/index.html as a result of transformation with
            // xmlto.
            // For example opengl/index.html
            String href = "";
            if (elm.getType().equalsIgnoreCase("stdApi")) {
                href = "html" + File.separator
                        + ((StdAPIElement) elm).getModuleName()
                        + File.separator + "index.html";
            } else {
                href = elm.getHref();
            }
            if (elm.getHref() != null) {
                topic.setAttribute(new Attribute("href", href));
            }
            Element apiTocElm = getInXmlToc(tocElm);
            if (apiTocElm != null) {
                apiTocElm.addContent(topic);
            } else {
                jDomApiTocXml.getRootElement().addContent(topic);
            }
            populateXmlToc(elm);
        }
    }

    /**
     * Populates the <code>JDOM Document</code> with the <code>Element</code>
     * tocElm's contents when creating a <HTML> document.
     * <p>
     * Uses recursion to populate with child <code>Elements</code>.
     *
     * @param htmlElm
     *            The <HTML> <code>Element</code> to populate.
     * @param tocElm
     *            The <code>Element</code> with the content.
     * @param leveloneLabel
     *            The <code>String</code> that will be set in the first level
     *            header of the <code>html</code> document.
     * @param level
     *            The header level in the <HTML> document.
     */
    private void populateHtmlDoc(Element htmlElm,
            com.stericsson.RefMan.Toc.Element tocElm, String levelOneLabel,
            int level) {
        Element htmlText = new Element("h" + level);
        String link = "";
        // If the element is a stdApi element, the documentation is placed
        // in moduleName/index.html as a result of transformation with xmlto.
        // For example opengl/index.html
        if (tocElm.getType().equalsIgnoreCase("stdapi")) {
            link = "html" + File.separator
                    + ((StdAPIElement) tocElm).getModuleName() + File.separator
                    + "index.html";
        } else {
            link = tocElm.getHref();
        }

        if (level == 1) {
            htmlText.addContent(levelOneLabel);
        } else if (link != null && !link.equals("")) {
            Element href = new Element("a");
            href.setAttribute("href", link);
            href.setText(tocElm.getLabel());
            htmlText.addContent(href);
        } else {
            htmlText.addContent(tocElm.getLabel());
        }
        htmlElm.addContent(htmlText);
        level++;
        for (com.stericsson.RefMan.Toc.Element elm : tocElm.getTopics()) {
            populateHtmlDoc(htmlElm, elm, levelOneLabel, level);
        }
    }

    /**
     * Looks for an <code>Element</code> that has the same label as tocElm.
     * <p>
     * Uses recursion to search the entire tree.
     *
     * @param tocElm
     *            The <code>Element</code> that contains the label we search
     *            after in <code>jDomApiToc</code>.
     * @return null if no <code>Element</code> is found.
     *         <p>
     *         If the <code>Element</code> is found the <code>Element</code> is
     *         returned.
     */
    private Element getInXmlToc(com.stericsson.RefMan.Toc.Element tocElm) {
        found = null;
        getInXmlToc(jDomApiTocXml.getRootElement(), tocElm);
        return found;
    }

    /**
     * Help method for
     * {@link com.stericsson.RefMan.Toc.DocumentFactory#getInXmlToc(com.stericsson.RefMan.Toc.Element)}
     * <p>
     * Compares elm's attribute label to node's label. Sets the variable found
     * to elm if <code>equalsIgnoreCase</code> returns <code>true</code>, Then
     * the method returns. Otherwise nothing is changed.
     *
     * @param elm
     *            The <code>JDOM</code> element that will be evaluated against
     *            node.
     * @param node
     *            The <code>Element</code> that will be evaluated against elm.
     */
    private void getInXmlToc(Element elm, com.stericsson.RefMan.Toc.Element node) {
        if (elm.getAttribute("label").getValue().equalsIgnoreCase(
                node.getLabel())) {
            found = elm;
            return;
        }
        if (elm.getChildren().size() > 0) {
            for (Object elm2 : elm.getChildren()) {
                getInXmlToc((Element) elm2, node);
            }
        }
    }
}
