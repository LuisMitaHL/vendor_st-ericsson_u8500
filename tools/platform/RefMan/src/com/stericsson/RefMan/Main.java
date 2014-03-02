/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;
import java.util.Vector;

import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.Namespace;
import org.jdom.input.SAXBuilder;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Commons.IO;
import com.stericsson.RefMan.Exception.DirectoryNotFoundException;
import com.stericsson.RefMan.Toc.DocumentFactory;
import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.ElementFactory;
import com.stericsson.RefMan.Toc.TocElement;
import com.stericsson.RefMan.USDoc.FindTocLocationRecursively;
import com.stericsson.RefMan.USDoc.FindTocLocationsFiles;
import com.stericsson.RefMan.USDoc.FindTocLocationsFromFile;
import com.stericsson.RefMan.USDoc.FindTocLocationsFromRepo;
import com.stericsson.RefMan.USDoc.UserSpaceDocumentation;
import com.stericsson.RefMan.UmlExportFilter.UmlExport;
import com.stericsson.RefMan.kernelDoc.KernelDoc;

/**
 * This is the main class for generating the API reference manual
 *
 * @author Martin Trulsson <a href=
 *         "mailto:martin.trulsson@stericsson.com?subject=com.stericsson.RefMan.Toc.Main"
 *         >Email</a>
 *
 */
public final class Main {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(Main.class);

    /** The path to the produced index file for the <code>HTML</code> version */
    final static String html_index = "eclipseplugin/html/index.html";

    /** The name of the xml output file. */
    final static String xml_output_file = "API.xml";

    /** The namespace of the exported uml file. */
    final static String umlExportFileNameSpace = "http://www.stericsson.com/refman/ste-uml-export.xsd";

    /** The path to the main toc file */
    private String main_toc = "resources/toc.xml";

    /** The exported uml file */
    private File uml_export_file = null;

    /**
     * True to ignore filtering
     */
    boolean no_uml_filtering = false;

    /** command line execution parameters */
    final static String command_line_exec = "Usage: -p PLATFORM -o OUTDIR -k KERNELDIR -s TOPLEVELDIR -r PROJECTROOTDIR -t TEMP_OUTDIR [options]\nOptions:\n  --verbose\tProvide verbose outputs suitable for debug\n  --use-repo\tAssume repo is used (default)\n  --recurse ADDITIONALDIR\tUse recursion to locate toc-locations.xml in the desired directory";

    /**
     * The path to the folder where the documentation will be copied. And the
     * merged Toc file.
     */
    private File outDir = null;

    /**
     * The path to the folder where the eclipseplugin will be placed. This file
     * is located in <code>outDir</code>.
     */
    private File eclipsePlugin;

    /**
     * The path to the folder where the HTML documentation will be copied.
     */
    private File htmlDir;

    /** The path to the folder for the Linux Kernel. */
    private File kernelDir = null;

    /**
     * The path to the folder for project root.
     */
    private File projectRootDir;

    /**
     * The path to the product/common folder.
     */
    private File topLevelDir;

    /**
     * The path to RefMans temporary folder where it can save files if needed.
     * It is also in this folder the toc-locations.xml files reside.
     */
    public static File tempDir = null;

    /**
     * Tells whether repo manifest should be used to locate gits
     */
    private boolean useRepo = false;

    /**
     * List of paths to the additional directory that should be traversed in
     * search for toc-locations.xml files
     */
    private final List<File> directoriesToRecurse = new Vector<File>();

    /**
     * The name of the platform. Should be used in the documentation.
     */
    public String platformName = null;

    /**
     * The version of the platform. Should be used in the documentation.
     */
    public String platformVersion = null;

    /**
     * The <code>String</code> that will be used as header in reference manual.
     */
    private String manual_header = "STE Linux Platform , ST-Ericsson platforms, Reference Manual";

    /**
     * Not used at the moment
     */
    public boolean ignoreErrors = false;

    /**
     * True, if Androids documentation should be included.
     */
    private boolean includeAndroidDoc = false;

    /**
     * The file which lists all toc files .
     * <p>
     * Will look something like this (paths are absolute not relative).
     * <p>
     * path1/toc-locations.xml
     * <p>
     * path2/toc-locations.xml
     * <p>
     * path3/toc-locations.xml
     * <p>
     * path4/toc-locations.xml
     * <p>
     */
    private static File tocsList = null;

    /**
     * The program reads an XML file representing the UML model. Only interfaces
     * specified in the file will be documented.
     * <p>
     * Copies all the documentation that have kernel space toc files from the
     * linux/Documentation/DocBook directory to the <code>htmlDir</code>
     * directory.
     * <p>
     * Searches for user space toc-location files alternatively reads the
     * locations from a file. For each STE user space toc-location file
     * <code>doxygen</code> is run with a dedicated <code>doxygen</code>
     * configuration file. For each <code>doxygen</code> run a new process is
     * invoked, this is done to maximize performance. If the toc-location file
     * is a standard API, the file is copied to <code>tempDir</code> and the
     * documentation refered to in copied to <code>htmlDir</code>.
     * <p>
     * Takes all the toc-location.xml files in the <code>tempDir</code>
     * directory and merges them to one toc and saves that toc in the
     * <code>outDir</code> directory.
     * <p>
     * Then the method generates an index <code>HTML</code> file representing
     * the merged toc file.
     *
     * @param args
     *            Command-line arguments provided by the user
     */
    public static void main(String[] args) {
        new Main(args);
    }

    public Main(String[] args) {

        config(args);

        generateUserSpaceDocumentation();

        filterKernelDocbooks();

        concatenateTocFiles();

        // Copy kernel doc
        KernelDoc.copyHtml(kernelDir, htmlDir, tempDir);

        File androidDoc = new File(eclipsePlugin, "android_doc/offline-sdk");
        androidDoc.mkdirs();

        if (includeAndroidDoc) {
            try {

                IO.copyDirectory(new File(projectRootDir,
                        "out/target/common/docs/offline-sdk"), androidDoc,
                        false, false);
            } catch (IOException e) {
                logger
                        .error(
                                "Failed to copy Andoids documentation from {} to output folder, {}",
                                new File(projectRootDir,
                                        "out/target/common/docs/offline-sdk"),
                                androidDoc);
            }
        }

        CreateEclipsePlugin createEclipsePlugin = new CreateEclipsePlugin(
                eclipsePlugin, htmlDir, platformName, manual_header,
                platformVersion);
        createEclipsePlugin.createPlugin(main_toc);

        CreateHtmlManual createHtmlManual = new CreateHtmlManual(eclipsePlugin,
                htmlDir, platformName, manual_header, platformVersion);
        createHtmlManual.createManual(main_toc);

        copyCssToOutputFolder();
        copyImagesToOutputFolder();
        copyFormalFiles();
    }

    /**
     * A help method which makes all the config steps needed by the tool.
     */
    private void config(String[] args) {
        // Parse the input parameters
        try {
            parseInput(args);
        } catch (Exception e) {
            logger
                    .error(
                            "Input parameters incorrect."
                                    + " RefMan considers this fatal and will now throw a RuntimeException.",
                            e);
            throw new RuntimeException();
        }

        /* set derived parameters */
        tocsList = new File(tempDir, "tocs.txt");

        // If API.xml already exists, remove it
        File outputFile = new File(tempDir, xml_output_file);
        if (outputFile.exists()) {
            outputFile.delete();
        }

        // Get the platform version tag
        getVersionTag();

        // Insert the platform name into the manual header
        int index = manual_header.indexOf(",");
        String firstPart = manual_header.substring(0, index - 1);
        String secondPart = manual_header.substring(index, manual_header
                .length());
        manual_header = firstPart + " " + platformName + secondPart;

        long timestamp = System.currentTimeMillis();

        eclipsePlugin = new File(outDir, platformName + "_1.0.0." + timestamp);

        File[] files = outDir.listFiles();
        for (File file : files) {
            if (file.getName().startsWith(platformName)) {
                file.renameTo(eclipsePlugin);
            }
        }

        htmlDir = new File(eclipsePlugin, "html");
        htmlDir.mkdirs();
    }

    /**
     * Method that remove kernel documentation that is not relevant for a
     * specific product. The filtering is based on the file exported from the
     * UML model.
     */
    @SuppressWarnings("unchecked")
    private void filterKernelDocbooks() {
        File[] tocFiles = tempDir.listFiles();
        SAXBuilder builder = new SAXBuilder();
        HashMap<String, String> map = new HashMap<String, String>();
        try {
            /*
             * Get the drivers that is specific for a platform from the exported
             * uml file.
             */
            Document document = builder.build(uml_export_file);
            org.jdom.Element root = document.getRootElement();
            org.jdom.Element drivers = root.getChild("drivers", Namespace
                            .getNamespace(umlExportFileNameSpace));
            if (drivers != null) {
                List<org.jdom.Element> driversList = drivers.getChildren();
                String supportedPlatform = "";
                String fileName = "";
                for (org.jdom.Element driver : driversList) {
                    if(!driver.getAttributeValue("kerneldocbookfilename").equals("")) {
                        org.jdom.Element platforms = (org.jdom.Element) driver.getChild("platforms",
                            Namespace.getNamespace(umlExportFileNameSpace));
                        List<org.jdom.Element> platformsList = platforms.getChildren();
                        supportedPlatform = "";
                        for (org.jdom.Element platform : platformsList) {
                            if(platform.getAttributeValue("part_of").equals("1")) {
                                supportedPlatform += platform.getAttributeValue("name") + ",";
                            }
                        }
                        fileName = driver.getAttributeValue("kerneldocbookfilename");
                        map.put(fileName.substring(0, fileName.indexOf(".tmpl")), supportedPlatform);
                    }
                }
            }

            /*
             * Iterate the toc files and remove those that are specific for
             * other platforms.
             */
            boolean remove = true;
            String fileName;
            String driverName;
            String includedInPlatforms;
            String[] platformNames;
            for (int i = 0; i < tocFiles.length; i++) {
                remove = true;
                fileName = tocFiles[i].getName();
                if (fileName.endsWith("-toc-locations.xml")) {
                    driverName = fileName.substring(0, fileName
                            .indexOf("-toc-locations.xml"));
                    includedInPlatforms = map.get(driverName);
                    if (includedInPlatforms != null) {
                        platformNames = includedInPlatforms.split(",");
                        for (int k = 0; k < platformNames.length; k++) {
                            if (platformName.endsWith(platformNames[k])) {
                                remove = false;
                                break;
                            }
                        }
                        if (remove) {
                            tocFiles[i].delete();
                        }
                    }
                }
            }
        } catch (JDOMException e) {
            logger.error("Failed to build JDom document for '"
                    + uml_export_file.getName() + "'");
        } catch (IOException e) {
            logger.error("Failed to build JDom document for '"
                    + uml_export_file.getName() + "'");
        }
    }

    /**
     * Method which concatenates the toc-location files in <code>tempDir</code>
     * into one file.
     *
     * Optionally, it also removes interfaces that aren't supposed to be shown.
     */
    private void concatenateTocFiles() {
        Document xml = null;
        com.stericsson.RefMan.Toc.TocElement API = null;
        try {
            API = ElementFactory.getAssembledElement(tempDir);
            API = filterInterfaces(API, uml_export_file, no_uml_filtering);
            xml = new DocumentFactory().getXmlToc(API, platformName);
        } catch (Exception e) {
            logger.error(
                    "Couldn't generate and return an API toc Document from the directory "
                            + tempDir + " of module tocs.", e);
        }

        XMLOutputter outputter = new XMLOutputter();
        outputter.setFormat(Format.getPrettyFormat());
        try {
            outputter.output(xml, new FileWriter(tempDir + File.separator
                    + xml_output_file));
        } catch (IOException e) {
            logger.error("Failed to write the JDom document to " + tempDir
                    + File.separator + "API.xml", e);
        }
    }

    /**
     * Checks the API Reference Manual against the content of the UML Export
     * file.
     *
     * Logs ERRORs, if the API Reference Manual contains interfaces that are not
     * mentioned in the UML Export file, or if the UML Export contains public
     * interfaces that aren't documented in the API Reference Manual.
     *
     * Optionally, modifies the underlying TocElement API to match the content
     * of the UML Export file, by removing all Elements that are not supposed to
     * be shown in this manual.
     *
     * @param API
     *            The TocElement representing the API Reference Manual.
     * @param umlExportFile
     *            The file representing the UML Export
     * @param disableFiltering
     *            If true, don't modify the TocElement.
     * @return The modified TocElement.
     */
    private TocElement filterInterfaces(
            com.stericsson.RefMan.Toc.TocElement API, File umlExportFile,
            boolean disableFiltering) {
        com.stericsson.RefMan.Toc.TocElement filteredAPI = API;
        if (umlExportFile.isFile()) {
            try {
                UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                        .getElement(umlExportFile, API, platformName);
                int faults = umlExport.checkInterfaces();
                if (faults != 0) {
                    logger
                            .warn(
                                    "Number of mismatches between UML model and Reference Manual detected: {}. Please update the UML model/Reference Manual.",
                                    faults);
                }
                if (!disableFiltering) {
                    filteredAPI = umlExport.removeInterfaces();
                    List<Element> filteredTopics = filteredAPI.getTopics();
                    if (filteredTopics.size() == 0) {
                        logger
                                .error("There is a severe mismatch between the UML model and Reference Manual detected. Unable to generate the Reference Manual. Please update model/reference manual.");
                    }
                }
            } catch (SAXException e1) {
                logger.error(uml_export_file.getAbsolutePath()
                        + ": Unable to parse, see errors above for reason", e1);
            }

        } else {
            logger.warn("No UML export file found in: "
                    + uml_export_file.getAbsolutePath()
                    + ". No filtering from the UML model applied.");
        }

        return filteredAPI;
    }

    /**
     * Method which generates the documentation for User space.
     */
    private void generateUserSpaceDocumentation() {
        UserSpaceDocumentation userSpaceDocumentation = new UserSpaceDocumentation(
                projectRootDir, htmlDir, tempDir, tocsList);
        List<FindTocLocationsFiles> findMethods = new ArrayList<FindTocLocationsFiles>();
        if (tocsList != null && tocsList.exists()) {
            FindTocLocationsFromFile findTocLocationsFromFile = new FindTocLocationsFromFile(
                    tocsList);
            findMethods.add(findTocLocationsFromFile);
        } else {
            if (useRepo) {
                FindTocLocationsFromRepo findTocLocationsFromRepo = new FindTocLocationsFromRepo(
                        projectRootDir);
                findMethods.add(findTocLocationsFromRepo);
            } else {
                FindTocLocationRecursively findTocLocationsRecursively = new FindTocLocationRecursively(
                        projectRootDir);
                findMethods.add(findTocLocationsRecursively);
            }

            for (File recurseDir : directoriesToRecurse) {
                FindTocLocationRecursively findTocLocationsRecursively = new FindTocLocationRecursively(
                        recurseDir);
                findMethods.add(findTocLocationsRecursively);
            }
        }
        userSpaceDocumentation.generateUserSpaceDocumentation(findMethods);
    }

    /**
     * Method retrieves a value of a parameter located at a given position in
     * provided array of strings (input command line parameters). If string not
     * present an exception is thrown.
     *
     * @param switchVal
     *            name of the corresponding switch (for error printout)
     * @param args
     *            array of input parameters
     *
     * @param pos
     *            position of parameter in the array
     *
     * @return returns a requested input parameter
     * @throws Exception
     *             if parameter value is not defined.
     */
    private String fetchString(String switchVal, String[] args, int pos)
            throws Exception {
        if (pos < args.length) {
            return args[pos];
        } else {
            logger.error(command_line_exec);
            throw new Exception("Incorrect input parameters: " + switchVal
                    + " value missing");
        }
    }

    /**
     * Method retrieves a directory with name defined at a given position in
     * provided array of strings (input command line parameters). If directory
     * not present an exception is thrown.
     *
     * @param switchVal
     *            name of the corresponding switch (for error printout)
     * @param args
     *            array of input parameters
     *
     * @param pos
     *            position of parameter in the array
     *
     * @return returns an File object pointing to the requested directory
     * @throws Exception
     *             if directory name not defined or directory not present
     */
    private File fetchDirectory(String switchVal, String[] args, int pos)
            throws Exception {
        String dir = fetchString(switchVal, args, pos);
        File outDir = new File(dir);
        if (!outDir.isDirectory()) {
            throw new DirectoryNotFoundException("Incorrect input parameters: "
                    + switchVal + " value (" + dir + ") is not a directory");
        }
        logger.info(switchVal + " is {}", outDir.getAbsolutePath());
        return outDir;
    }

    /**
     * Method retrieves a file with name defined at a given position in provided
     * array of strings (input command line parameters). If file not present an
     * exception is thrown.
     *
     * @param switchVal
     *            name of the corresponding switch (for error printout)
     * @param args
     *            array of input parameters
     *
     * @param pos
     *            position of parameter in the array
     *
     * @return returns an File object pointing to the requested directory
     * @throws Exception
     *             if file name not defined or file not present
     */
    private File fetchFile(String switchVal, String[] args, int pos)
            throws Exception {
        String dir = fetchString(switchVal, args, pos);
        File outDir = new File(dir);
        if (!outDir.isFile()) {
            throw new DirectoryNotFoundException("Incorrect input parameters: "
                    + switchVal + " value (" + dir + ") is not a file");
        }
        logger.info(switchVal + " is {}", outDir.getAbsolutePath());
        return outDir;
    }

    /**
     * Method checks if an individual parameter is defined, if not an exception
     * is thrown
     *
     * @param param
     *            parameter to check
     * @param name
     *            corresponding parameter name (for error printout)
     *
     * @throws Exception
     *             if parameter in question is not defined.
     */
    private void checkParameter(Object param, String name) throws Exception {
        if (param == null) {
            logger.error(command_line_exec);
            throw new Exception("Incorrect input parameters: " + name
                    + " not specified");
        }
    }

    /** Method checks if all mandatory variables are defined */
    private void checkMandatoryParameters() throws Exception {
        checkParameter(outDir, "OUTDIR");
        checkParameter(kernelDir, "KERNELDIR");
        checkParameter(projectRootDir, "PROJECTROOTDIR");
        checkParameter(topLevelDir, "TOPLEVELDIR");
        checkParameter(tempDir, "TEMP_OUTDIR");
        checkParameter(platformName, "PLATFORM");
        checkParameter(uml_export_file, "UML_EXPORT_FILE");

    }

    /**
     * Method to parse the input arguments and update the needed fields.
     * <p>
     * Also responsible for error handling of the input arguments. For example
     * if the user gives arguments that are not allowed the method should
     * provide output on the usage.
     *
     * @param args
     *            Command-line arguments provided by the user
     *
     * @throws Exception
     *             If input parameters are incorrect.
     */
    private void parseInput(String[] args) throws Exception {

        for (int i = 0; i < args.length; i++) {
            logger.debug(args[i]);
        }

        int position = 0;

        while (position < args.length) {
            /**
             * go through all supported switches. If unknown switch throw
             * exception
             */
            if (args[position].equals("-p")) {
                platformName = fetchString("-p", args, position + 1)
                        .toUpperCase();
                position = position + 2;
            } else if (args[position].equals("-o")) {
                outDir = fetchDirectory("-o", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("-k")) {
                kernelDir = fetchDirectory("-k", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("-r")) {
                projectRootDir = fetchDirectory("-r", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("-s")) {
                topLevelDir = fetchDirectory("-s", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("-t")) {
                tempDir = fetchDirectory("-t", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("-u")) {
                uml_export_file = fetchFile("-u", args, position + 1);
                position = position + 2;
            } else if (args[position].equals("--include_android_doc")) {
                main_toc = "resources" + File.separator
                        + "toc_with_Android_doc.xml";
                includeAndroidDoc = true;
                position = position + 1;
            } else if (args[position].equals("--verbose")) {
                /**
                 * change the default logging level (warning) to detailed
                 * (debug)
                 */
                org.apache.log4j.Logger.getRootLogger().setLevel(
                        org.apache.log4j.Level.DEBUG);
                position = position + 1;
            } else if (args[position].equals("--ignore-errors")) {
                ignoreErrors = true;
                position = position + 1;
            } else if (args[position].equals("--use-repo")) {
                useRepo = true;
                position = position + 1;
            } else if (args[position].equals("--no-uml-filtering")) {
                no_uml_filtering = true;
                position = position + 1;
            } else if (args[position].equals("--recurse")) {
                File recurseDir = fetchDirectory("--recurse", args,
                        position + 1);
                directoriesToRecurse.add(recurseDir);
                position = position + 2;
            }
            /** add additional switches before this else clause */
            else {
                /** handling of unknown parameters, this must always come last */
                logger.error(command_line_exec);
                throw new Exception("Unsupported switch: " + args[position]);
            }
        }

        checkMandatoryParameters();

    }

    private void getVersionTag() {
        File versionFile = new File(tempDir, "versionNumber.txt");
        boolean foundTag = false;
        try {
            Scanner scan = new Scanner(versionFile);
            while (scan.hasNextLine()) {
                String versionTag = scan.nextLine();
                // Find the first tag
                if (versionTag.equals("tag:")) {
                    foundTag = true;
                }
                // If the tag is not empty, use it as version.
                if (!versionTag.equals("tag:") && foundTag) {
                    platformVersion = versionTag.substring(versionTag
                            .lastIndexOf("/") + 1);
                    break;
                }
            }
        } catch (FileNotFoundException e) {
            logger.error("Could not find file containing a version tag,"
                    + " an empty string is used instead. {}", e.getMessage());
            platformVersion = "";
        }
    }

    /**
     * Mehtod which copies html css file to the html folder.
     */
    private void copyCssToOutputFolder() {
        File sourceCssFile = new File("resources" + File.separator
                + "style.css");
        File destCssFile = new File(htmlDir, "style.css");
        try {
            IO.copyFile(sourceCssFile, destCssFile);
        } catch (IOException e) {
            logger.info("Failed to copy html css file", e);
        }
    }

    /**
     * Method which copies the images from the resources folder to the
     * eclipsePlugin folder.
     */
    private void copyImagesToOutputFolder() {
        File sourceFile = new File("resources" + File.separator + "images");
        File destFile = new File(eclipsePlugin, "images");
        try {
            IO.copyDirectory(sourceFile, destFile, false, false);
        } catch (IOException e) {
            logger.info("Failed to copy images to eclipsePlugin folder", e);
        }
    }

    /**
     * Method which copies the formal files to the htmlDir.
     */
    private void copyFormalFiles() {
        File sourceFiles = new File("resources" + File.separator
                + "formal-files");
        try {
            for (File file : sourceFiles.listFiles()) {
                IO.copyFile(file, new File(htmlDir, file.getName()));
            }
        } catch (IOException e) {
            logger.info("Failed to copy formal files to html folder", e);
        }
    }
}
