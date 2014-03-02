/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import org.jdom.Attribute;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Toc.DocumentFactory;
import com.stericsson.RefMan.Toc.ElementFactory;

public class CreateEclipsePlugin {

    /** The template file for Manifest.mf. */
    private static final String MANIFEST_TEMPLATE = "resources/eclipseplugin/MANIFEST.MF";

    /** The template file for plugin.xml. */
    private static final String PLUGIN_TEMPLATE = "resources/eclipseplugin/plugin.xml";

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(CreateEclipsePlugin.class);

    /**
     * The path to the folder where the documentation will be copied. And the
     * merged Toc file.
     */
    private final File outDir;

    /**
     * The path to the folder where the HTML documentation will be copied.
     */
    private final File htmlDir;

    /**
     * The name of the platform. Should be used in the documentation.
     */
    private final String platform;

    /**
     * The <code>String</code> that will be used as header in reference manual.
     */
    private final String manual_header;

    /**
     * The <code>String</code> that will be used as version in reference manual.
     */
    private final String versionTag;

    /**
     * Constructor for the <code>CreateEclipsePlugin</code>.
     *
     * @param outDir
     *            The output folder where the plugin files will be placed.
     * @param htmlDir
     *            The html folder where the html files will be plpaced.
     * @param platform
     *            The name of the platform.
     */
    public CreateEclipsePlugin(File outDir, File htmlDir, String platform,
            String header, String versionTag) {
        this.outDir = outDir;
        this.htmlDir = htmlDir;
        this.platform = platform;
        this.manual_header = header;
        this.versionTag = versionTag;
    }

    /**
     * A method which copies the files specific for the eclipse plugin, such as
     * plugin.xml and Manifest.mf, to the output folder.
     *
     * @param main_toc
     *            A String representing the path to the xml file with the
     *            content of the eclipse plugin.
     */
    public void createPlugin(String main_toc) {

        generateManifestFile();
        generatePluginFile(main_toc);
        generateMainHtmlpageForplugin();
    }

    /**
     * Method which generates Manifest.mf file for the eclipse plug-in in the
     * output folder.
     */
    private void generateManifestFile() {
        File manifestFolder = new File(outDir, "META-INF");
        manifestFolder.mkdirs();
        File manifestOutput = new File(manifestFolder, "MANIFEST.MF");
        File manifest = new File(MANIFEST_TEMPLATE);
        // If the file already exists, delete it.
        if (manifestOutput.exists()) {
            manifestOutput.delete();
        }
        try {
            // Parse the file and add the platform name at appropiate places in
            // the file. 'Bundle-Name' and 'Bundle-SymbolicName' should be
            // followed by the name of the platform.
            Scanner scan = new Scanner(manifest);
            while (scan.hasNextLine()) {
                String line = scan.nextLine();
                if (line.contains("Bundle-Name:")) {
                    line += " " + platform;
                } else if (line.contains("Bundle-SymbolicName:")) {
                    int index = line.indexOf(";");
                    String tempLine = line.substring(0, index);
                    tempLine += platform;
                    tempLine += line.substring(index, line.length());
                    line = tempLine;
                } else if (line.contains("Bundle-Version:")) {
                    String outDirName = outDir.getName();
                    line += outDirName.substring(outDirName.lastIndexOf("_") + 1);
                }
                FileWriter fw = new FileWriter(manifestOutput, true);
                fw.append(line + "\n");
                fw.flush();
                fw.close();
            }
            logger.info("Successfully created 'MANIFEST.MF'");
        } catch (FileNotFoundException e) {
            logger.error("Can not find '{}'", manifest.getAbsolutePath());
        } catch (IOException e) {
            logger.error("Could not create a FileWriter for '{}'",
                    manifestOutput.getAbsolutePath(), e);
        }
    }

    /**
     * Method which generates the plugin.xml file for the eclipse plug-in in the
     * output folder.
     */
    private void generatePluginFile(String main_toc) {
        SAXBuilder builder = new SAXBuilder();
        try {
            // The structure of the plugin.xml template file:
            //
            // <?xml version="1.0" encoding="UTF-8"?>
            // <?eclipse version="3.2"?>
            // <plugin>
            // <extension point="org.eclipse.help.toc">
            // <toc category="ST-Ericsson Reference Manual" file=""
            // primary="true">
            // </toc>
            // </extension>
            // </plugin>

            Document document = builder.build(new File(PLUGIN_TEMPLATE));
            // Add the platform name in the 'toc' element.
            Element rootElm = document.getRootElement();
            Element extensionPoint = rootElm.getChild("extension");
            if (extensionPoint != null) {
                // Add attributes to the 'toc' element. 'category' should be the
                // platform name and 'file' should be the platform name followed
                // by '.xml'.
                Element toc = extensionPoint.getChild("toc");
                if (toc != null) {
                    toc.setAttribute("file", platform + ".xml");
                    XMLOutputter outputter = new XMLOutputter();
                    FileWriter writer = new FileWriter(new File(outDir,
                            "plugin.xml"));
                    outputter.output(rootElm, writer);
                    writer.close();
                    logger.info("Successfully created 'plugin.xml'");
                } else {
                    logger.error("Can not find <toc> in plugin template,"
                            + " failed to generate 'plugin.xml'");
                }
            } else {
                logger.error("Can not find <extension> in plugin template,"
                        + " failed to generate 'plugin.xml'");
            }
        } catch (JDOMException e) {
            logger.error("Failed to build JDom document for 'plugin.xml'");
        } catch (IOException e) {
            logger.error("Failed to build JDom document for 'plugin.xml'");
        }

        // Create a main xml file for the plugin. The file should be named
        // platform.xml, for example U8500.xml
        com.stericsson.RefMan.Toc.Element mainToc = ElementFactory.getElement(
                new File(main_toc), 0);
        Document mainDoc = new DocumentFactory().getXmlToc(mainToc, platform
                + "_" + versionTag);

        addIndexPages(mainDoc);

        try {
            XMLOutputter outputter = new XMLOutputter();
            FileWriter writer = new FileWriter(new File(outDir, platform
                    + ".xml"));
            outputter.output(mainDoc, writer);
            writer.close();
            logger.info("Successfully created '{}.xml'", platform);
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to '{}.xml'",
                    platform, e);
        }
    }

    /**
     * Method which generates the main html page for the eclipse plugin. This
     * page will be visible in the top levelof the eclipse plugin.
     */
    private void generateMainHtmlpageForplugin() {
        Element htmlRoot = new Element("html");
        Element head = new Element("head");
        // Add a stylesheet
        Element link = new Element("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", "style.css");
        link.setAttribute("type", "text/css");
        head.addContent(link);
        htmlRoot.addContent(head);
        // Create a body
        Element body = new Element("body");
        body.setAttribute("class", "mainindexpage");
        Element logo = new Element("img");
        logo.setAttribute("src", "../images/stelogo.png");
        logo.setAttribute("align", "right");
        body.addContent(logo);
        // Information about which platform the API manual is intended for.
        Element h1 = new Element("H1");
        h1.addContent(manual_header);
        body.addContent(h1);
        htmlRoot.addContent(body);
        // Create the html file on disk.
        Document doc = new Document(htmlRoot);
        XMLOutputter outputter = new XMLOutputter();
        outputter.setFormat(Format.getPrettyFormat());
        try {
            FileWriter writer = new FileWriter(new File(htmlDir,
                    "Mainpage.html"));
            outputter.output(doc, writer);
            writer.close();
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to "
                    + outDir.getAbsolutePath() + File.separator
                    + "Mainpage.html", e);
        }
    }

    /**
     * Method which generates index pages for the eclipse plugin. These pages is
     * needed to give the plugin a homogeneous ST-Ericsson look.
     *
     * @param doc
     *            JDom <code>Document</code> representing the plugin file.
     */
    private void addIndexPages(Document doc) {
        Element elm = doc.getRootElement();
        // Add a reference to the the Mainpage in the toc element.
        elm.setAttribute(new Attribute("topic", "html/Mainpage.html"));
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> l = elm.getChildren();
        for (Element e : l) {
            addIndexPages(e, "");
        }
    }

    /**
     *
     * Add references to the index pages to the topic elements in the toc file.
     *
     * @param elm
     *            An <code>Element</code> which the index page should be added
     *            to.
     * @param topicName
     *            The name of the current 'topic'. Used to seperate for example
     *            different 'User space' from different categories.
     */
    private void addIndexPages(Element elm, String topicName) {
        @SuppressWarnings("unchecked")
        // JDom doesn't support generics.
        List<Element> l = elm.getChildren();
        ArrayList<Element> hrefs = new ArrayList<Element>();
        // Add the current label to topicName.
        topicName += elm.getAttributeValue("label");
        // Create a link to all the children of this element in the index page.
        for (Element e : l) {
            Element hrefElm = new Element("a");
            // If a child is the lowest leaf, use its link, otherwise create a
            // link to the childs index page.
            if (e.getAttribute("href") != null) {
                String href = e.getAttributeValue("href");
                if (href.startsWith("html")) {
                    href = href.substring(href.indexOf("/") + 1);
                }
                hrefElm.setAttribute("href", href);
            } else {
                hrefElm.setAttribute(new Attribute("href", topicName + "_"
                        + e.getAttributeValue("label") + ".html"));
            }
            hrefElm.addContent(e.getAttributeValue("label"));
            hrefs.add(hrefElm);
        }
        if (elm.getAttributeValue("href") != null) {
		elm.setAttribute(new Attribute("href", elm
				.getAttributeValue("href")));
	} else {
		elm.setAttribute(new Attribute("href", "html/" + topicName
				+ ".html"));
		}
		generateIndexHtmlPage(topicName + ".html", elm
				.getAttributeValue("label"), hrefs);
		for (Element e : l) {
			if (e.getChildren().size() > 0) {
				addIndexPages(e, topicName + "_");
		}
	}
    }

    /**
     * Create the indexpage for a element.
     *
     * @param fileName
     *            The name of the page
     * @param header
     *            A <code>String</code> that will be printed in the
     *            <code>h1</code> tag. For example 'User Space'.
     * @param l
     *            A list containing the <code>href</code> elements.
     */
    private void generateIndexHtmlPage(String fileName, String header,
            List<Element> l) {
        Element htmlRoot = new Element("html");
        Element head = new Element("head");
        Element link = new Element("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", "style.css");
        link.setAttribute("type", "text/css");
        head.addContent(link);
        htmlRoot.addContent(head);
        Element body = new Element("body");
        body.setAttribute(new Attribute("class", "mainindexpage"));
        Element h1 = new Element("H1");
        h1.addContent(header);
        body.addContent(h1);
        // Add the href elements to the html page.
        for (Element e : l) {
            body.addContent(e);
            body.addContent(new Element("br"));
        }
        htmlRoot.addContent(body);
        // Create the html file on disk.
        Document doc = new Document(htmlRoot);
        XMLOutputter outputter = new XMLOutputter();
        outputter.setFormat(Format.getPrettyFormat());
        FileWriter writer;
        try {
            writer = new FileWriter(new File(htmlDir, fileName));
            outputter.output(doc, writer);
            writer.close();
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to "
                    + outDir.getAbsolutePath() + File.separator + fileName, e);
        }
    }
}
