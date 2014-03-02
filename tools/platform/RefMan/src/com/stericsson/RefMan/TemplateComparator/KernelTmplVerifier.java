/**
 * (c) Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.TemplateComparator;

import java.io.File;
import java.util.List;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.input.SAXBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class KernelTmplVerifier {

    /** logger */
    final static Logger logger = LoggerFactory
            .getLogger(KernelTmplVerifier.class);

    /**
     * Template file for proprietary linux kernel tmpl files.
     */
    final private static File tmplXMLTemplate = new File("resources"
            + File.separator + "kernel_tmpl_template.xml");

    /**
     * Template file for proprietary linux kernel driver tmpl files.
     */
    final private static File tmplDriverXMLTemplate = new File("resources"
            + File.separator + "kernel_driver_tmpl_template.xml");

    /**
     * The program takes a Kernel tmpl file as input parameter and verifies that
     * all mandatory chapters are present.
     *
     * @param args
     *            Command-line arguments provided by the user
     */
    public static void main(String[] args) {

        boolean result = true;

        if (args.length == 0) {
            logger.error("A tmpl file is needed as input parameter");
        } else if (args.length > 1) {
            logger.error("only one parameter is needed");
        } else {
            File tmplFile = new File(args[0]);
            if (!tmplFile.isFile()) {
                logger
                        .error(
                                "Failed to verify {}, file does not exist or is not a file",
                                tmplFile.getAbsolutePath());
            }
            logger.info("Verifying {} against guidelines", tmplFile
                    .getAbsolutePath());

            SAXBuilder builder = new SAXBuilder();
            builder
                    .setFeature(
                            "http://apache.org/xml/features/nonvalidating/load-external-dtd",
                            false);
            Document tmpl = null;
            Element tmplRoot;
            boolean driver = false;
            // Parse the tmpl file to see if it is documenting a driver module.
            try {
                tmpl = builder.build(tmplFile.getAbsolutePath());
                tmplRoot = tmpl.getRootElement();
                // If the file has a chapter with id 'driver-configuration',
                // then it should be validated against the driver template.
                @SuppressWarnings("unchecked")
                // JDom does not support generics
                List<Element> chapters = tmplRoot.getChildren("chapter");
                for (Element elm : chapters) {
                    if (elm.getAttributeValue("id").equalsIgnoreCase(
                            "driver-configuration")) {
                        driver = true;
                    }
                }

                XMLVerifier xmlVerifier = new XMLVerifier();
                if (!driver) {
                    result = xmlVerifier.compareElementToTemplateFile(
                            tmplXMLTemplate, tmplRoot, tmplFile
                                    .getAbsolutePath());
                } else {
                    result = xmlVerifier.compareElementToTemplateFile(
                            tmplDriverXMLTemplate, tmplRoot, tmplFile
                                    .getAbsolutePath());
                }
            } catch (Exception e) {
                logger.error("Error reading template xml file {}. Error: {}",
                        tmplFile.getAbsolutePath(), e.getMessage());
            }

            if (!result) {
                logger.error(
                        "Kernel tmpl file: {} does not follow guidelines. "
                                + "See log above for further details.",
                        tmplFile.getAbsolutePath());
                System.exit(1);
            }
        }
    }
}
