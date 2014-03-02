/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.IO;
import com.stericsson.RefMan.Toc.ElementFactory;

public class CreateHtmlManual {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(CreateHtmlManual.class);

    /** Path to the javaScript for the tree view. */
    private final static String javascript = "resources/html/treescript.js";

    /** Path to the index html file. */
    private final static String indexPage = "resources/html/index.html";

    /**
     * The folder where the documentation will be copied.
     */
    private final File outDir;

    /**
     * The folder where the HTML documentation will be copied.
     */
    private final File htmlDir;

    /**
     * The name of the platform. Should be used in the documentation.
     */
    private final String platformName;

    /**
     * The <code>String</code> that will be used as header in the reference
     * manual.
     */
    private final String manual_header;

    /**
     * The version of the platform.
     */
    private final String platformVersion;

    /** A counter used to give unique classes in the tree veiw html file. */
    private int count;

    /**
     * Constructor for <code>CreateHtmlManual</code>
     *
     * @param outDir
     *            The folder where the documentation will be copied.
     * @param htmlDir
     *            The folder where the html documentation will be copied.
     * @param platformName
     *            The name of the platform
     * @param header
     *            The <code>String</code> that will be used as header in the
     *            reference manual.
     * @param platformVersion
     *            The version of the platform.
     */
    public CreateHtmlManual(File outDir, File htmlDir, String platformName,
            String header, String platformVersion) {
        this.outDir = outDir;
        this.htmlDir = htmlDir;
        this.platformName = platformName;
        this.manual_header = header;
        this.platformVersion = platformVersion;
    }

    /**
     * A method which generates the different html pages used in the index page
     * and stores them in the output folder.
     *
     * @param main_toc
     *            A String representing the path to the xml file with the
     *            content of the manual.
     */
    public void createManual(String main_toc) {
        copyJavaScript();
        createMainHeader();
        copyIndexPage();
        createTreeView(main_toc);
    }

    /**
     * Method which copies the javascript file to the <code>htmlDir</code>
     */
    private void copyJavaScript() {
        File srcFile = new File(javascript);
        File destFile = new File(outDir, "treescript.js");
        try {
            IO.copyFile(srcFile, destFile);
        } catch (IOException e) {
            logger.warn("Failed to copy the javascript file {} to {}", srcFile
                    .getAbsolutePath(), outDir.getAbsolutePath());
        }
    }

    /**
     * Method which creates the main_header.html file and saves it in
     * <code>htmlDir</code>
     */
    private void createMainHeader() {
        Element html = new Element("html");
        Element head = new Element("head");
        Element link = new Element("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", "style.css");
        head.addContent(link);
        Element body = new Element("body");
        Element img = new Element("img");
        img.setAttribute("src", "../images/stelogo.png");
        img.setAttribute("align", "right");
        Element h1 = new Element("h1");
        h1.addContent(manual_header + ", " + platformVersion);
        body.addContent(img);
        body.addContent(h1);
        html.addContent(head);
        html.addContent(body);

        XMLOutputter outputter = new XMLOutputter();
        Format format = Format.getPrettyFormat();
        format.setExpandEmptyElements(true);
        outputter.setFormat(format);
        try {
            outputter.output(new Document(html), new FileWriter(new File(
                    htmlDir, "main_header.html")));
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to "
                    + htmlDir.getAbsolutePath() + File.separator
                    + "main_header.html", e);
        }
    }

    /**
     * Method which copies the index.html file to <code>htmlDir</code>
     */
    private void copyIndexPage() {
        File srcFile = new File(indexPage);
        File destFile = new File(outDir, "index.html");
        try {
            IO.copyFile(srcFile, destFile);
        } catch (IOException e) {
            logger.warn("Failed to copy index html file {} to {}", srcFile
                    .getAbsolutePath(), outDir.getAbsolutePath());
        }
    }

    /**
     * Method which generates a html file containing a tree view of the manual.
     *
     * @param main_toc
     *            The path to the main xml toc file.
     */
    private void createTreeView(String main_toc) {
        Element root = new Element("html");
        root.setAttribute("lang", "en");
        addHead(root);
        addBody(root, main_toc);

        XMLOutputter outputter = new XMLOutputter();
        Format format = Format.getPrettyFormat();
        format.setExpandEmptyElements(true);
        outputter.setFormat(format);
        try {
            outputter.output(new Document(root), new FileWriter(new File(
                    outDir, "tree.html")));
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to "
                    + outDir.getAbsolutePath() + File.separator + "tree.html",
                    e);
        }
    }

    /**
     * Add a head element to the <code>Element</code> <code>root</code>.
     *
     * @param root
     *            The <code>Element</code> that the head <code>Element</code>
     *            will be added to.
     */
    private void addHead(Element root) {
        Element head = new Element("head");
        Element link = new Element("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", "html/style.css");
        Element title = new Element("title");
        title.addContent("TreeView");
        Element script = new Element("script");
        script.setAttribute("type", "text/javascript");
        script.setAttribute("src", "treescript.js");
        head.addContent(link);
        head.addContent(title);
        head.addContent(script);
        root.addContent(head);
    }

    /**
     * Add a body element to the <code>Element</code> <code>root</code>.
     *
     * @param root
     *            The <code>Element</code> that the body <code>Element</code>
     *            will be added to.
     * @param main_toc
     *            A path to the toc xml file that contains the content that
     *            should be included in the body.
     */
    private void addBody(Element root, String main_toc) {
        com.stericsson.RefMan.Toc.Element rootTocElm = ElementFactory
                .getElement(new File(main_toc), 0);
        count = 1;
        // create the body tags. These tags are always the same, independent of
        // the content of the manual.
        Element body = new Element("body");
        Element divDir = new Element("div");
        divDir.setAttribute("class", "directory");
        body.addContent(divDir);
        Element h3 = new Element("h3");
        h3.setAttribute("class", "swap");
        Element span = new Element("span");
        span.addContent(platformName);
        h3.addContent(span);
        divDir.addContent(h3);
        Element divDisp = new Element("div");
        divDisp.setAttribute("style", "display: block;");
        divDir.addContent(divDisp);
        // Add the content of the manual, found in main_toc.
        List<com.stericsson.RefMan.Toc.Element> children = rootTocElm
                .getTopics();
        int nbrOfChildren = children.size();
        List<Boolean> level = new ArrayList<Boolean>();
        for (int i = 0; i < nbrOfChildren; i++) {
            if (i == nbrOfChildren - 1) {
                addContent(divDisp, children.get(i), level, true);
            } else {
                addContent(divDisp, children.get(i), level, false);
            }
        }

        root.addContent(body);
    }

    /**
     * A help method that adds the content of a
     * <code>com.stericsson.RefMan.Toc.Element</code> to a <code>Element</code>.
     *
     * @param htmlElm
     *            The <code>Element</code> to which the content should be added.
     * @param tocElm
     *            The <code>com.stericsson.RefMan.Toc.Element</code> which
     *            contains the content to add
     * @param lastElms
     *            A <code>List</code> containing <code>boolean</code>, where
     *            each element in the list represent a level in the tree. If the
     *            element is <code>true</code>, the corresponding element in the
     *            tree was the last element. This list is used to decide which
     *            picture to add in the tree view.
     * @param lastElm
     *            <code>true</code> if <code>tocElm</code> is the last element,
     *            otherwise <code>false</code>.
     */
    private void addContent(Element htmlElm,
            com.stericsson.RefMan.Toc.Element tocElm, List<Boolean> lastElms,
            boolean lastElm) {
        // If the element has no children, create a <a href> tag.
        if (tocElm.getTopics().size() == 0) {
            Element p = new Element("p");
            // Add different images to give the tree a nice look.
            // If there are more elements in the same level, add a vertical line
            // to it. If it is the last element, add a blank picture.
            for (int i = 0; i < lastElms.size(); i++) {
                if (lastElms.get(i) == false) {
                    addVerticalLine(p);
                } else {
                    addBlank(p);
                }
            }
            if (lastElm) {
                addLastNode(p);
            } else {
                addNode(p);
            }
            addDoc(p);
            Element a = new Element("a");
            a.setAttribute("class", "el");
            a.setAttribute("href", tocElm.getHref());
            a.setAttribute("target", "basefrm");
            a.addContent(tocElm.getLabel());
            p.addContent(a);
            htmlElm.addContent(p);
        } else {
            // If the element has children, go through then recursivley.
            Element p = new Element("p");
            // Add different images to give the tree a nice look.
            // If there are more elements in the same level, add a vertical line
            // to it. If it is the last element, add a blank picture.
            for (int i = 0; i < lastElms.size(); i++) {
                if (lastElms.get(i) == false) {
                    addVerticalLine(p);
                } else {
                    addBlank(p);
                }
            }
            if (lastElm) {
                addLastPlusNode(p, count);
                lastElms.add(true);
            } else {
                addPlusNode(p, count);
                lastElms.add(false);
            }
            addFolderClosed(p, count);
            p.addContent(tocElm.getLabel());
            htmlElm.addContent(p);
            Element div = new Element("div");
            div.setAttribute("id", "folder" + count);
            count++;
            htmlElm.addContent(div);
            // Recursively go through the children of tocElm.
            List<com.stericsson.RefMan.Toc.Element> children = tocElm
                    .getTopics();
            int nbrOfChildren = children.size();
            for (int i = 0; i < nbrOfChildren; i++) {
                List<Boolean> temp = new ArrayList<Boolean>(lastElms);
                if (i == nbrOfChildren - 1) {
                    addContent(div, children.get(i), temp, true);
                } else {
                    addContent(div, children.get(i), temp, false);
                }
            }
        }
    }

    /**
     * Add a Vertical line picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the line element picture will
     *            be added to.
     */
    private void addVerticalLine(Element elm) {
        Element vertLine = new Element("img");
        vertLine.setAttribute("src", "images/ftv2vertline.png");
        vertLine.setAttribute("alt", "|");
        elm.addContent(vertLine);
    }

    /**
     * Add a plus node picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the plus node picture element
     *            will be added to.
     * @param cnt
     *            A counter used to give <code>Element</code>'s unique ids.
     */
    private void addPlusNode(Element elm, int cnt) {
        Element pNode = new Element("img");
        pNode.setAttribute("src", "images/ftv2pnode.png");
        pNode.setAttribute("alt", "o");
        pNode
                .setAttribute("onclick", "toggleFolder('folder" + cnt
                        + "', this)");
        elm.addContent(pNode);
    }

    /**
     * Add a last plus node picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the last plus node picture
     *            element will be added to.
     * @param cnt
     *            A counter used to give <code>Element</code>'s unique ids.
     */
    private void addLastPlusNode(Element elm, int cnt) {
        Element pNode = new Element("img");
        pNode.setAttribute("src", "images/ftv2plastnode.png");
        pNode.setAttribute("alt", "\\");
        pNode
                .setAttribute("onclick", "toggleFolder('folder" + cnt
                        + "', this)");
        elm.addContent(pNode);
    }

    /**
     * Add a node picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the node picture element will
     *            be added to.
     */
    private void addNode(Element elm) {
        Element pNode = new Element("img");
        pNode.setAttribute("src", "images/ftv2node.png");
        pNode.setAttribute("alt", "o");
        elm.addContent(pNode);
    }

    /**
     * Add a last node picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the last node picture element
     *            will be added to.
     */
    private void addLastNode(Element elm) {
        Element pNode = new Element("img");
        pNode.setAttribute("src", "images/ftv2lastnode.png");
        pNode.setAttribute("alt", "\\");
        elm.addContent(pNode);
    }

    /**
     * Add a closed folder picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the closed folder picture
     *            element will be added to.
     * @param cnt
     *            A counter used to give <code>Element</code>'s unique ids.
     */
    private void addFolderClosed(Element elm, int cnt) {
        Element folderClosed = new Element("img");
        folderClosed.setAttribute("src", "images/ftv2folderclosed.png");
        folderClosed.setAttribute("alt", "+");
        folderClosed.setAttribute("onclick", "toggleFolder('folder" + cnt
                + "', this)");
        elm.addContent(folderClosed);
    }

    /**
     * Add a document picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the document picture element
     *            will be added to.
     */
    private void addDoc(Element elm) {
        Element doc = new Element("img");
        doc.setAttribute("src", "images/ftv2doc.png");
        doc.setAttribute("alt", "*");
        elm.addContent(doc);
    }

    /**
     * Add a blank picture <code>Element</code> to the tree view.
     *
     * @param elm
     *            The <code>Element</code> which the blank picture element will
     *            be added to.
     */
    private void addBlank(Element elm) {
        Element doc = new Element("img");
        doc.setAttribute("src", "images/ftv2blank.png");
        doc.setAttribute("alt", "&nbsp");
        elm.addContent(doc);
    }
}
