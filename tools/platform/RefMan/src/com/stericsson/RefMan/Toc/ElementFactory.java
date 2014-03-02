/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Vector;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Main;
import com.stericsson.RefMan.Exception.NotAStdAPIException;
import com.stericsson.RefMan.Validator.XSDValidator;

/**
 * This class is responsible for creating Elements from XML toc files.
 *
 * @author Thomas Palmqvist
 *
 */
public class ElementFactory {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(ElementFactory.class);

    final static String path_to_schemas = "resources/";

    private static List<com.stericsson.RefMan.Toc.Element> interfaceNames = new Vector<com.stericsson.RefMan.Toc.Element>();

    /**
     * Private constructor to prevent users to instantiate this class.
     */
    private ElementFactory() {

    }

    /**
     * The method takes a folder as parameter and finds all xml files in this
     * folder. It merges and sorts the content of the xml files and creates a
     * tree structure of <code>Elements</code> which represents the content of
     * the xml-files.
     *
     * @param ImTocFiles
     *            The folder containing the toc files.
     * @return An <code>TocElement</code> representing the root element in the
     *         tree of <code>Elements</code> representing the content of the xml
     *         files.
     */
    public static TocElement getAssembledElement(File ImTocFiles) {
        com.stericsson.RefMan.Toc.TocElement apiToc = null;
        logger.debug("getAssembledElemet(): Resetting interfaceNames");
        interfaceNames = new Vector<com.stericsson.RefMan.Toc.Element>();
        try {
            List<com.stericsson.RefMan.Toc.Element> tocs = getElementArray(ImTocFiles);
            apiToc = assemble(tocs);
            merge(apiToc);
            sort(apiToc);
        } catch (IOException e) {
            logger.error("Failed to merge toc files", e);
        }
        logger
                .debug(
                        "getAssembledElemet(): calling SetInterfaces() with {} elements",
                        interfaceNames.size());
        apiToc.setInterfaces(interfaceNames);
        return apiToc;
    }

    /**
     * The method takes a folder as parameter and finds all xml-files in this
     * folder. It creates and returns a list of <code>Elements</code> which
     * represents the content of the xml-files.
     *
     * @param ImTocFiles
     *            The folder containing the toc files.
     * @return A <code>List</code> containing the <code>Elements</code>.
     * @throws IOException
     *             An <code>IOException</code> is thrown if ImTocPaths is not a
     *             path to a directory.
     */
    public static List<com.stericsson.RefMan.Toc.Element> getElementArray(
            File ImTocFiles) throws IOException {

        ArrayList<com.stericsson.RefMan.Toc.Element> tocs = new ArrayList<com.stericsson.RefMan.Toc.Element>();

        if (ImTocFiles.isDirectory()) {
            for (File imTocFile : ImTocFiles.listFiles(new FileFilter() {

                // Java 1.6 @Override
                public boolean accept(File pathname) {
                    return pathname.getName().toLowerCase().endsWith(".xml");
                }
            })) {
                boolean exists = false;
                com.stericsson.RefMan.Toc.Element elm = getElement(imTocFile, 0);
                for (com.stericsson.RefMan.Toc.Element toc : tocs) {
                    // Check if the toc name already exists.
                    // If it exists, the toc file will not be included in the
                    // manual.
                    if (toc.getTopics().get(0).getTopics().get(0).getTopics()
                            .get(0).getLabel().compareToIgnoreCase(
                                    elm.getTopics().get(0).getTopics().get(0)
                                            .getTopics().get(0).getLabel()) == 0) {
                        logger.error("({}) module name '{}' already exists",
                                imTocFile.getName(), elm.getTopics().get(0)
                                        .getTopics().get(0).getTopics().get(0)
                                        .getLabel());
                        exists = true;
                        boolean deleted = imTocFile.delete();
                        if (!deleted) {
                            logger.error("Failed to delete {}", imTocFile
                                    .getName());
                        } else {
                            logger.info("Removed {}", imTocFile.getName());
                            break;
                        }
                    }
                }
                if (!exists) {
                    tocs.add(elm);
                }
                exists = false;
            }
        } else {
            throw new IOException("The given input path is not a directory.");
        }

        return tocs;
    }

    /**
     * The method creates and returns an <code>Element</code> representing the
     * content of the toc XML file provided as an input parameter.
     *
     * @param imTocFile
     *            The XML file containing the toc data.
     * @param depth
     *            A value describing the elements level in the tree structure.
     * @return The root <code>Element</code> representing the content of a toc
     *         file.
     */
    public static com.stericsson.RefMan.Toc.Element getElement(File imTocFile,
            int depth) {

        XSDValidator validator;
        Document imToc = null;
        // Validate the input xml file to our schema.
        try {
            // Build a JDom document from the input file
            SAXBuilder builder = new SAXBuilder();
            imToc = builder.build(imTocFile);
            String namespace = imToc.getRootElement().getNamespaceURI();
            validator = new XSDValidator(new File(path_to_schemas
                    + namespace.substring(namespace.lastIndexOf("/") + 1)));
            validator.validate(imTocFile.getAbsolutePath());
        } catch (IOException e1) {
            logger.error("Failed to validate toc file '{}': {}", imTocFile
                    .getAbsolutePath(), e1);
        } catch (SAXException e1) {
            logger.error("Invalid toc file '{}': {}", imTocFile
                    .getAbsolutePath(), e1);
        } catch (JDOMException e) {
            logger.error("Failed to build a JDom document for file '{}': {}",
                    imTocFile.getAbsolutePath(), e);
        }

        // Create a toc Element for the input file
        org.jdom.Element jDomRoot = imToc.getRootElement();
        com.stericsson.RefMan.Toc.Element tocRoot = getElement(jDomRoot, depth);
        return tocRoot;
    }

    /**
     * A method which recursively extracts and inserts the content of the
     * <code>JDOM Element</code> topic to a new <code>RefMan Element</code>
     * which is returned.
     *
     * @param topic
     *            The <code>JDOM Element</code>
     * @param depth
     *            A value describing the elements level in the tree structure.
     * @return An <code>RefMan Element</code> representing the content of the
     *         <code>JDOM Element</code>.
     */
    private static com.stericsson.RefMan.Toc.Element getElement(
            org.jdom.Element topic, int depth) {
        String label = topic.getAttributeValue("label");
        String href = topic.getAttributeValue("href");
        Vector<com.stericsson.RefMan.Toc.Element> topics = new Vector<com.stericsson.RefMan.Toc.Element>();
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<org.jdom.Element> topicsElm = topic.getChildren();
        if (topicsElm.size() > 0) {
            for (org.jdom.Element topicElm : topicsElm) {
                // Check if the child of an element is a link element. If so,
                // insert the elements specified by the linked file.
                Element child = topicElm.getChild("link", topicElm
                        .getNamespace());
                if (child != null) {
                    topics.add(getElement(new File(Main.tempDir, child
                            .getAttributeValue("toc")), depth + 1));
                } else {
                    // The structure of a standard API differs from proprietary
                    // tocs, therefore a different method is used.
                    if (topicElm.getName().equals("standardapi")) {
                        try {
                            topics.add(getStdAPIElement(topicElm));
                        } catch (NotAStdAPIException e) {
                            logger
                                    .error("The element has not been included in the toc");
                        }
                    } else {
                        topics.add(getElement(topicElm, depth + 1));
                    }

                }
            }
        }
        com.stericsson.RefMan.Toc.Element toc = null;
        if (depth == 0) {
            toc = new com.stericsson.RefMan.Toc.TocElement(label, href, topics);
        } else {
            toc = new com.stericsson.RefMan.Toc.TopicElement(label, href,
                    topics);
            if (depth == 3) {
                interfaceNames.add(toc);
                logger.debug("Added inteface '{}' for depth {}", label, depth);
            }
        }
        return toc;
    }

    /**
     * A method which extracts and the content of the <code>JDOM Element</code>
     * topic and inserts it to a new <code>StdAPIElement</code> which is
     * returned.
     *
     * @param topic
     *            The <code>JDOM Element</code>
     * @return A <code>StdAPIElement</code> representing the content of the
     *         <code>JDOM Element</code>.
     * @throws NotAStdAPIException
     *             If the <code>Element</code> is not a
     *             <code>StdAPIElement</code>.
     */

    private static com.stericsson.RefMan.Toc.Element getStdAPIElement(
            Element topic) throws NotAStdAPIException {
        if (!topic.getName().equals("standardapi")) {
            logger.warn("The element is not a standardapi element"
                    + ", it is a {} element", topic.getName());
            throw new NotAStdAPIException();
        }
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        // Standard APIs differ between includedirectory and includefile and
        // they need to be handled in defferent ways.
        List<Element> includes = topic.getChildren();
        Vector<IncludeDoc> includeDirs = new Vector<IncludeDoc>();
        Vector<IncludeDoc> includeFiles = new Vector<IncludeDoc>();
        Vector<Script> externalScripts = new Vector<Script>();
        for (Element elm : includes) {
            if (elm.getName().equals("includedirectory")) {
                includeDirs.add(new IncludeDoc(elm.getAttributeValue("name"),
                        elm.getAttributeValue("destination"), true));
            } else if (elm.getName().equals("includefile")) {
                includeFiles.add(new IncludeDoc(elm.getAttributeValue("name"),
                        elm.getAttributeValue("destination"), false));
            } else if (elm.getName().equals("externalScript")) {
                externalScripts.add(new Script(elm.getAttributeValue("script"),
                        elm.getAttributeValue("startDir")));
            }
        }
        com.stericsson.RefMan.Toc.Element toc = new com.stericsson.RefMan.Toc.StdAPIElement(
                topic.getAttributeValue("title"), topic
                        .getAttributeValue("documentation"), topic
                        .getAttributeValue("modulename"), topic
                        .getAttributeValue("modulepath"), topic
                        .getAttributeValue("implementationpath"), includeDirs,
                includeFiles, externalScripts);
        return toc;
    }

    /**
     * Creates and returns an <code>TocElement</code> that will have all topics
     * of each <code>Element</code> topics in the input list.
     *
     * @param tocs
     *            The <code>Elements</code> who's topics will be put into the
     *            new <code>Element</code>.
     * @return The assembled <code>TocElement</code>.
     */
    private static TocElement assemble(
            List<com.stericsson.RefMan.Toc.Element> tocs) {
        com.stericsson.RefMan.Toc.TocElement apiToc = new com.stericsson.RefMan.Toc.TocElement();
        apiToc.setLabel("API");
        for (com.stericsson.RefMan.Toc.Element toc : tocs) {
            apiToc.addTopics(toc.getTopics());
        }
        return apiToc;
    }

    /**
     * Sorts according to the
     * {@link com.stericsson.RefMan.Toc.ElementComparator#compare(com.stericsson.RefMan.Toc.Element, com.stericsson.RefMan.Toc.Element)}
     * .
     * <p>
     * Uses recursion to sort child Element to.
     *
     * @param tocElm
     *            Element that will be sorted
     */
    public static void sort(com.stericsson.RefMan.Toc.Element tocElm) {
        Collections.sort(tocElm.getTopics(), new ElementComparator());
        for (com.stericsson.RefMan.Toc.Element toc : tocElm.getTopics()) {
            sort(toc);
        }

    }

    /**
     * Merge elements with the same label and parent. Removes all but one
     * element with the same label and parent. Uses recursion to merge all child
     * Element to.
     *
     * @param tocElm
     *            Element that contains the Elements that will be merged
     */
    private static void merge(com.stericsson.RefMan.Toc.Element tocElm) {
        List<com.stericsson.RefMan.Toc.Element> tocs = tocElm.getTopics();
        int tocsSize = tocs.size();
        for (int i = tocsSize - 1; i >= 0; i--) {
            com.stericsson.RefMan.Toc.Element toc = tocs.get(i);
            int index = contains(tocs, toc);
            if (index > -1) {
                tocs.get(index).addTopics(toc.getTopics());
                tocs.remove(toc);
            } else {
                merge(toc);
            }
        }
    }

    /**
     * Looks if <code>tocs</code> contains <code>tocElm</code>.
     * <p>
     * The algorithm works like this, an int > -1 is returned if
     * <code>tocs</code> contains an <code>tocElm</code> element that has the
     * same label but is not the same element.
     * <p>
     * Does not use recursion so it will only look at the first level.
     *
     * @param tocs
     *            The list to be checked against.
     * @param tocElm
     *            The sought <code>Element</code>
     * @return The <code>Element</code>
     */
    private static int contains(List<com.stericsson.RefMan.Toc.Element> tocs,
            com.stericsson.RefMan.Toc.Element tocElm) {
        for (int i = 0; i < tocs.size(); i++) {
            com.stericsson.RefMan.Toc.Element toc = tocs.get(i);
            if (new ElementComparator().compare(toc, tocElm) == 0
                    && toc != tocElm)
                return i;
        }
        return -1;
    }
}
