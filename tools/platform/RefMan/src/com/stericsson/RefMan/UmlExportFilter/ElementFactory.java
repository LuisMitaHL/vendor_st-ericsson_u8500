/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.Vector;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Toc.TocElement;
import com.stericsson.RefMan.Validator.XSDValidator;

/**
 * This class is responsible for creating a <code>UmlExport</code> from a UML
 * export XML file.
 *
 * @author Fredrik Lundström
 *
 */
public class ElementFactory {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(ElementFactory.class);

    final static String path_to_schemas = "resources/";

    /**
     * Private constructor to prevent users to instantiate this class.
     */
    private ElementFactory() {

    }

    /**
     * The method creates and returns an <code>UmlExport</code> representing the
     * content of the UML export XML file provided as an input parameter.
     *
     * @param umlExportFile
     *            The XML file containing the UML export data.
     * @param api
     *            The unfiltered API Reference Manual
     * @param platformName
     *            The name of the platorm.
     * @return The <code>UmlExport</code> representing the content of a UML
     *         export XML file.
     * @throws SAXException
     *             if the file failed to validate.
     */
    public static com.stericsson.RefMan.UmlExportFilter.UmlExport getElement(
            File umlExportFile, TocElement api, String platformName)
            throws SAXException {

        XSDValidator validator;
        Document umlExportDocument = null;
        // Validate the input xml file to our schema.
        boolean validationSucceeded = false;
        try {
            // Build a JDom document from the input file
            SAXBuilder builder = new SAXBuilder();
            umlExportDocument = builder.build(umlExportFile);
            String namespace = umlExportDocument.getRootElement()
                    .getNamespaceURI();
            validator = new XSDValidator(new File(path_to_schemas
                    + namespace.substring(namespace.lastIndexOf("/") + 1)));
            validationSucceeded = validator.validate(umlExportFile
                    .getAbsolutePath());
        } catch (IOException e1) {
            logger.error("Failed to validate exported uml file. ", e1);
            throw new SAXException(e1);
        } catch (SAXException e1) {
            logger.error("Invalid exported uml file. ", e1);
            throw new SAXException(e1);
        } catch (JDOMException e) {
            logger.error("Failed to build a JDom document", e);
            throw new SAXException(e);
        }
        if (validationSucceeded) {
            // Create a root Element for the input file
            Element root = umlExportDocument.getRootElement();
            return createUmlExport(root, api, platformName);
        } else {
            String errormessage = umlExportFile.getAbsolutePath()
                    + ": Invalid format. See previous log entries for details.";
            logger.error(errormessage);
            throw new SAXException(errormessage);
        }
    }

    /**
     * Creates a <code>UmlExport</code> representing the content of a UML export
     * XML file by recursively extracting and inserting the content of the
     * <code>JDOM Element</code>.
     *
     * @param root
     *            The <code>JDOM Element</code>
     * @param api
     *            The unfiltered API Reference Manual
     * @param platformName
     *            The name of the platorm.
     * @return An <code>UmlExport</code> representing the the content of a UML
     *         export XML file.
     */

    private static com.stericsson.RefMan.UmlExportFilter.UmlExport createUmlExport(
            Element root, TocElement api, String platformName) {
        UmlExport rootElement = new UmlExport(api, platformName);

        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> rootsElm = root.getChildren();
        if (rootsElm.size() > 0) {
            for (Element rootElm : rootsElm) {
                if (rootElm.getName().compareTo("export-meta-data") == 0) {
                    logger.debug("Got <export-meta-data>, ignoring content");
                } else if (rootElm.getName().compareTo("interfaces") == 0) {
                    logger.debug("Got <interfaces>");
                    populateInterfaces(rootElement, rootElm);
                } else {
                    logger.warn("Got unsupported element '" + rootElm.getName()
                            + "'");
                }
            }

            return rootElement;
        }

        throw new RuntimeException("root is not initialized.");
    }

    /**
     * Populate the UmlExport with the contents of the &lt;interfaces&gt; tag.
     *
     * @param rootElement
     *            The <code>UmlExport</code> representing the content of a UML
     *            export XML file.
     * @param interfacesElm
     *            the &lt;interfaces&gt; tag element.
     */
    private static void populateInterfaces(
            com.stericsson.RefMan.UmlExportFilter.UmlExport rootElement,
            Element interfacesElm) {
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> interfaceElmList = interfacesElm.getChildren();
        for (Element interfaceElm : interfaceElmList) {
            if (interfaceElm.getName().compareTo("interface") == 0) {
                logger.debug("Got <interface>");
                Interface i = createInterface(interfaceElm);
                rootElement.addInterface(i);
            } else {
                logger.warn("Got unsupported element '"
                        + interfaceElm.getName() + "'");
            }
        }

    }

    /**
     * Create an <code>Interface</code> by populating it with the contents of an
     * &lt;interface&gt; tag.
     *
     * @param interfaceElm
     *            the &lt;interface&gt; tag <code>JDOM Element</code>.
     * @return An <code>Interface</code> representing the content of the
     *         <code>JDOM Element</code>.
     */
    private static Interface createInterface(Element interfaceElm) {
        Interface i = new Interface();

        String name = interfaceElm.getAttributeValue("name", "");
        String packageName = interfaceElm.getAttributeValue("package", "");
        String guid = interfaceElm.getAttributeValue("guid", "");
        String classification = interfaceElm
                .getAttributeValue("classification");
        String stack = interfaceElm.getAttributeValue("stack");

        logger.debug("name='" + name + "'");
        logger.debug("package='" + packageName + "'");
        logger.debug("guid='" + guid + "'");
        logger.debug("classification='" + classification + "'");

        i.setName(name);
        i.setPackage(packageName);
        i.setGuid(guid);
        if (classification != null) {
            i.setClassification(classification);
        }

        i.setStack(stack);

        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> interfaceElmContentList = interfaceElm.getChildren();
        for (Element interfaceElmContent : interfaceElmContentList) {
            if (interfaceElmContent.getName().compareTo("platforms") == 0) {
                logger.debug("Got <platforms>");
                List<Platform> pl = createPlatformList(interfaceElmContent);
                i.setPlatforms(pl);
            } else {
                logger.warn("Got unsupported element '"
                        + interfaceElmContent.getName() + "'");
            }
        }

        return i;
    }

    /**
     * Create a List of <code>Platform</code>'s by populating it with the
     * contents of a &lt;platforms&gt; tag.
     *
     * @param platformsElm
     *            the &lt;platforms&gt; tag <code>JDOM Element</code>.
     * @return An List of <code>Platform</code> representing the content of the
     *         <code>JDOM Element</code>.
     */
    private static List<Platform> createPlatformList(Element platformsElm) {
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> platformsElmContentList = platformsElm.getChildren();
        List<Platform> platformsFound = new Vector<Platform>();
        for (Element platformElmContent : platformsElmContentList) {
            if (platformElmContent.getName().compareTo("platform") == 0) {
                logger.debug("Got <platform>");
                Platform p = createPlatform(platformElmContent);
                platformsFound.add(p);
            } else {
                logger.info("Got undefined element");
            }
        }
        return platformsFound;
    }

    /**
     * Create a <code>Platform</code> by populating it with the contents of a
     * &lt;platform&gt; tag.
     *
     * @param platformElm
     *            the &lt;platform&gt; tag <code>JDOM Element</code>.
     * @return <code>Platform</code> representing the content of the
     *         <code>JDOM Element</code>.
     */
    private static Platform createPlatform(Element platformElm) {
        Platform p = new Platform();
        String name = platformElm.getAttributeValue("name");
        String interface_internal_only = platformElm
                .getAttributeValue("interface_internal_only");

        logger.debug("name='" + name + "'");
        logger.debug("interface_internal_only='" + interface_internal_only
                + "'");

        p.setName(name);
        p.setInterfaceInternalOnly(interface_internal_only);

        return p;
    }

    /**
     * Used during test of parsing of exported file.
     *
     * @see getElement(File , TocElement , String )
     * @throws SAXException
     */

    static UmlExport getElement(File file) throws SAXException {
        return getElement(file, null, null);
    }

}
