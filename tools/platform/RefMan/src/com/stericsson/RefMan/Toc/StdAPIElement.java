/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Collection;
import java.util.List;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.IO;
import com.stericsson.RefMan.Exception.IncludeDocNotSpecifiedException;

public class StdAPIElement extends Element {
    /** The logger */
    final Logger logger = LoggerFactory.getLogger(StdAPIElement.class);

    /** A <code>String</code> saying that this is a <code>StdAPIElement</code> */
    private static final String type = "StdAPI";

    /** The name of the module */
    private String moduleName;

    /** The path to the module */
    private String modulePath;

    /** The path to the implementation */
    private String implementationpath;

    /** A <code>Vector</code> with the includeDirectories of the module */
    private Vector<IncludeDoc> includeDirs;

    /** A <code>Vector</code> with the includeFiles of the module */
    private Vector<IncludeDoc> includeFiles;

    /** A <code>Vector</code> containing scripts to run */
    private Vector<Script> externalScripts;

    /**
     * A <code>Vector</code> of the directories in which to search for
     * includeFiles and includeDirs. Is null when the directories are
     * incorrectly configured.
     */
    private Vector<File> documentationSourcePath;

    /**
     * The directory in which to open the DocBook file. Is null when the
     * directories are incorrectly configured.
     */
    private File docbookSourcePath;

    /**
     * An object of a help class which holds the names of the files that already
     * has been copied. To handle files with the same name.
     */
    private CopiedFilesHelper copiedFilesHelper;

    /**
     * Constructor for the <code>StdAPIElement</code>.
     *
     * @param title
     *            The name of a module. Will be showed in the Reference Manual.
     * @param documentation
     *            The name of the documentation file. Will be used as link path
     *            in the Reference Manual.
     * @param moduleName
     *            The name of a module.
     * @param modulePath
     *            The path to the documentation of the implementation module,
     *            relative to the CM module containing it.
     * @param implementationpath
     *            The path to the CM module containing the implementation of the
     *            implementation module, or null if the documentation is
     *            contained within the same CM module as the implementation.
     * @param includeDir
     *            A representation of the directory containing the documentation
     *            of a module.
     * @param includeFile
     *            A representation of the file containing the documentation of a
     *            module.
     * @param externalScripts
     *            A <code>Vector</code> containing external scripts that should
     *            be started.
     */
    public StdAPIElement(String title, String documentation, String moduleName,
            String modulePath, String implementationpath,
            Vector<IncludeDoc> includeDir, Vector<IncludeDoc> includeFile,
            Vector<Script> externalScripts) {
        super(title, documentation, new Vector<Element>());
        this.moduleName = moduleName;
        this.modulePath = modulePath;
        this.implementationpath = implementationpath;
        this.includeDirs = includeDir;
        this.includeFiles = includeFile;
        this.externalScripts = externalScripts;
        this.copiedFilesHelper = CopiedFilesHelper.getCopiedFilesHelper();
    }

    /**
     * Constructor for the <code>StdAPIElement</code>.
     */
    public StdAPIElement() {
        super();
        moduleName = "";
        modulePath = "";
        implementationpath = "";
        includeDirs = new Vector<IncludeDoc>();
        includeFiles = new Vector<IncludeDoc>();
        externalScripts = new Vector<Script>();
    }

    /**
     * See set description.
     *
     * @param moduleName
     *            The <code>String</code> representing the name of a module.
     */
    public void setModuleName(String moduleName) {
        this.moduleName = moduleName;
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> representing the name of a module.
     */
    public String getModuleName() {
        return moduleName;
    }

    /**
     * See set description.
     *
     * @param modulePath
     *            The <code>String</code> representing the path of the module.
     */
    public void setModulePath(String modulePath) {
        this.modulePath = modulePath;
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> representing the path of the module.
     */
    public String getModulePath() {
        return modulePath;
    }

    /**
     * See set description.
     *
     * @param implementationpath
     *            The <code>String</code> representing the path to the
     *            implementation.
     */
    public void setImplementationpath(String implementationpath) {
        this.implementationpath = implementationpath;
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> representing the path to the
     *         implementation.
     */
    public String getImplementationpath() {
        return implementationpath;
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>IncludeDocs</code>
     *         representing the <code>StdAPIElement's</code> includeDirectories.
     */
    public List<IncludeDoc> getIncludeDirs() {
        return includeDirs;
    }

    /**
     * See set description.
     *
     * @param includeDirs
     *            Sets the <code>StdAPIElement's</code> includeDirectories to the ones
     *            provided by the <code>Collection</code>.
     */
    public void setIncludeDir(Collection<IncludeDoc> includeDirs) {
        this.includeDirs = new Vector<IncludeDoc>(includeDirs);
    }

    /**
     * Adds an includeDir to the <code>StdAPIElement's</code> includeDirectories.
     *
     * @param includeDir
     *            <code>IncludeDoc</code> to be appended to this <code>StdAPIElement's</code>
     *            includeDirectories.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addIncludeDir(IncludeDoc includeDir) {
        return this.includeDirs.add(includeDir);
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>IncludeDocs</code>
     *         representing the <code>StdAPIElement's</code> includeFiles.
     */
    public List<IncludeDoc> getIncludeFiles() {
        return includeFiles;
    }

    /**
     * See set description.
     *
     * @param includeFiles
     *            Sets the <code>StdAPIElement's</code> includeFiles to the ones provided
     *            by the <code>Collection</code>.
     */
    public void setIncludeFiles(Collection<IncludeDoc> includeFiles) {
        this.includeFiles = new Vector<IncludeDoc>(includeFiles);
    }

    /**
     * Adds an includeFile to the <code>StdAPIElement's</code> includeFiles.
     *
     * @param includeFile
     *            <code>IncludeDoc</code> to be appended to this <code>StdAPIElement's</code>
     *            includeFiles.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addIncludeFile(IncludeDoc includeFile) {
        return this.includeFiles.add(includeFile);
    }

    /**
     * See set description
     *
     * @param scripts
     *            Sets the external scripts that should be started to the ones
     *            provided by the <code>Collection</code>.
     */
    public void setExternalScripts(Collection<Script> scripts) {
        this.externalScripts = new Vector<Script>(scripts);
    }

    /**
     * Adds a script to the <code>StdAPIElement</code>'s external scripts.
     *
     * @param script
     *            The <code>Script</code> to add to the external scripts.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addExternalScripts(Script script) {
        return this.externalScripts.add(script);
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>Script</code>'s
     *         representing the <code>StdAPIElement</code>'s external scripts.
     */
    public List<Script> getExternalScripts() {
        return externalScripts;
    }

    /**
     * Copy the include files placed in the included Directory to the folder
     * represented by <code>copyTo</code>. If a file with the same name already
     * has been copied to the target folder, the file is not copied.
     *
     * The files are converted to HTML, if required.
     *
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     * must have been called before this method is invoked.
     *
     * @param copyTo
     *            The folder which the include files should be copied to.
     * @return {@code true} if the files where succesfully copied, otherwise
     *         <code>False</code>
     * @throws IncludeDocNotSpecifiedException
     *             Thrown if no include directory is specified.
     */
    public boolean copyIncludeFilesDirectory(File copyTo)
            throws IncludeDocNotSpecifiedException {
        boolean success = true;
        if (includeDirs == null) {
            throw new IncludeDocNotSpecifiedException();
        }
        for (final IncludeDoc doc : includeDirs) {
            File docFile = null;
            // Find the folder where the file to copy is placed.
            for (File file : documentationSourcePath) {
                docFile = new File(file, doc.getName());
                if (docFile.exists() && docFile.isDirectory()) {
                    break;
                }
            }
            if (docFile == null) {
                logger.error("could not find {}", doc.getName());
                return false;
            } else {
                File includeDir = docFile;
                try {
                    File[] files = includeDir.listFiles();
                    for (int i = 0; i < files.length; i++) {
                        // Copy the files to
                        // copyTo/moduleName/doc.getDestination
                        // for example:
                        // eclipseplugin/html/opengl/includes
                        File destinationDirectory = new File(copyTo, moduleName
                                + File.separator + doc.getDestination());
                        destinationDirectory.mkdirs();
                        success &= copyFiles(files[i], new File(
                                destinationDirectory, files[i].getName()));
                    }
                } catch (Exception e) {
                    logger.error("Failed to copy the content of '{}'",
                            includeDir.getAbsolutePath());
                    logger.error(e.getMessage());
                    success = false;
                }
            }
        }
        return success;
    }

    /**
     * Private help method that copies files recursively.
     *
     * @param srcFile
     *            The <code>File</code> to copy.
     * @param destFile
     *            The <code>File</code> to copy to.
     * @return {@code true} if the files where succesfully copied, otherwise
     *         <code>False</code>
     * @throws IOException
     *             If the copy failed.
     */
    private boolean copyFiles(File srcFile, File destFile) throws IOException {
        boolean success = true;
        if (srcFile.isDirectory()) {
            for (File file : srcFile.listFiles()) {
                destFile.mkdirs();
                success &= copyFiles(file, new File(destFile, file.getName()));
            }
        } else {
            // If the file has not yet been copied, copy the file. If the file
            // has already has been copied, do not overwrite it but give the
            // user an error.
            if (!copiedFilesHelper
                    .fileAlreadyCopied(destFile.getAbsolutePath())) {
                IO.copyFileAndOptionallyConvert(srcFile, destFile, false);
                copiedFilesHelper.addFileName(destFile.getAbsolutePath());
            } else {
                logger.error("Failed to copy '{}', a file"
                        + " named '{}' already exists.", srcFile
                        .getAbsolutePath(), destFile.getAbsoluteFile());
                success = false;
            }
        }
        return success;
    }

    /**
     * Copy the include files to the folder represented by <code>copyTo</code>.
     * If a file with the same name already has been copied to the target
     * folder, the file is not copied.
     *
     * The files are converted to HTML, if required.
     *
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     * must have been called before this method is invoked.
     *
     * @param copyTo
     *            The folder which the include files should be copied to.
     * @return {@code true} if the files where succesfully copied, otherwise
     *         <code>False</code>
     * @throws IncludeDocNotSpecifiedException
     *             Thrown if no include files is specified.
     */
    public boolean copyIncludeFiles(File copyTo)
            throws IncludeDocNotSpecifiedException {

        boolean success = true;
        if (includeFiles == null) {
            throw new IncludeDocNotSpecifiedException();
        }
        for (IncludeDoc doc : includeFiles) {
            final String docFileName = doc.getName();
            File docFile = null;
            // Find the folder where the file to copy is placed.
            for (File file : documentationSourcePath) {
                docFile = new File(file, docFileName);
                if (docFile.exists() && docFile.isFile()) {
                    break;
                }
            }
            if (docFile == null) {
                logger.error("Could not find '{}'", docFileName);
                return false;
            } else {
                // If the file exists, copy the file.
                String filePath = docFile.getAbsolutePath();
                File destinationDirectory = null;
                try {
                    // Copy the files to copyTo/moduleName/doc.getDestination
                    // for example: eclipseplugin/html/opengl/includes
                    destinationDirectory = new File(copyTo, moduleName
                            + File.separator + doc.getDestination());
                    destinationDirectory.mkdirs();
                    String fileName = docFile.getName();
                    File destFile = new File(destinationDirectory, fileName);
                    // If the file has not yet been copied, copy the file. If
                    // the file already has been copied, do not overwrite it
                    // but give the user an error.
                    if (copiedFilesHelper.fileAlreadyCopied(destFile
                            .getAbsolutePath())) {
                        logger.error("Failed to copy '{}', a file"
                                + " named '{}' already exists.", destFile
                                .getAbsolutePath());
                        success = false;
                    } else {
                        IO.copyFileAndOptionallyConvert(new File(filePath),
                                destFile, false);
                        copiedFilesHelper.addFileName(destFile
                                .getAbsolutePath());
                    }
                } catch (IOException e) {
                    logger.error("Failed to copy '{}'.", filePath, e);
                    return false;
                }
            }
        }
        return success;
    }

    /**
     * Compares an <code>Object</code> to this <code>StdAPIElement</code> to see
     * if they are equal.
     *
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>StdAPIElement</code>
     * @return {@code true} if the <code>Object</code> and
     *         <code>StdAPIElement</code> are equal, otherwise
     *         <code>false</code>.
     */
    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (o instanceof StdAPIElement) {
            StdAPIElement e = (StdAPIElement) o;
            if (href == null) {
                if (label.compareTo(e.getLabel()) == 0 && href == e.getHref()
                        && moduleName.compareTo(e.getModuleName()) == 0
                        && modulePath.compareTo(e.getModulePath()) == 0
                        && includeDirs.equals(e.getIncludeDirs())
                        && includeFiles.equals(e.getIncludeFiles())
                        && topics.equals(e.getTopics())) {
                    return true;
                }
            } else {
                if (label.compareTo(e.getLabel()) == 0
                        && href.compareTo(e.getHref()) == 0
                        && moduleName.compareTo(e.getModuleName()) == 0
                        && modulePath.compareTo(e.getModulePath()) == 0
                        && includeDirs.equals(e.getIncludeDirs())
                        && includeFiles.equals(e.getIncludeFiles())
                        && topics.equals(e.getTopics())) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> representing the type of the
     *         <code>StdAPIElement</code>.
     */
    @Override
    public String getType() {
        return type;
    }

    /**
     * Calculates the hashCode of this <code>StdAPIElement</code>.
     *
     * @return A hashCode value for this <code>StdAPIElement</code>.
     */
    @Override
    public int hashCode() {
        return toString().hashCode();
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>StdAPIElement</code>.
     */
    @Override
    public String toString() {
        return "type: StdAPI; title: " + label + "; documentation: " + href
                + "; moduleName: " + moduleName + "; modulePath: " + modulePath;
    }

    /**
     * Gets the the source directories to copy from, depending on the presence
     * of an <code>&lt;implementationpath&gt</code> element.
     *
     * All source names in the includeFiles and includeDirs shall be located
     * relative to this directory.
     *
     * @return List of directory from which to copy. Returns null if it could
     *         not be resolved.
     */
    public Vector<File> getCopyDirPath() {
        return documentationSourcePath;

    }

    /**
     * Gets the the source directory to obtain the DocBook from.
     *
     * @return Directory from which to read the DocBook. Returns null if it
     *         could not be resolved.
     */
    public File getDocBookDirectory() {
        return docbookSourcePath;

    }

    /**
     * Calculates the the source directories to copy from, depending on the
     * presence of an <code>implementationpath</code> attribute. Sets the
     * <code>documentationSourcePath</code> and <code>docbookSourcePath</code>
     * fields. Sets the <code>documentationSourcePath</code> and
     * <code>docbookSourcePath</code> to null, logs a descriptive ERROR and
     * throws an exception in case of incorrect or missing directories.
     * <code>modulePath</code> and optionally also
     * <code>implementationpath</code> must be set before calling this method.
     *
     * @param tocLocationsSourceFile
     *            The File representing this toc-locations.xml
     * @param projectRootDirectoryPath
     *            The project root directory, as specified by user
     * @throws IOException
     *             In case of incorrect or missing directories.
     */
    public void calculatePaths(File tocLocationsSourceFile,
            String projectRootDirectoryPath) throws IOException {
        documentationSourcePath = new Vector<File>();

        if (modulePath == null || modulePath.length() == 0) {
            throw new NullPointerException(
                    "modulePath must be defined prior to calling this method");
        }

        if (modulePath.equals(".")) {
            modulePath = "";
        }

        File documentationCmModulePath;
        try {
            documentationCmModulePath = tocLocationsSourceFile
                    .getCanonicalFile().getParentFile();
        } catch (IOException e) {
            logger.error(e.toString());
            documentationSourcePath = null;
            docbookSourcePath = null;
            throw e;
        }
        docbookSourcePath = documentationCmModulePath;

        boolean noModulePathInDocumentation = false;
        File documentationModulePath = new File(documentationCmModulePath,
                modulePath);
        if (!documentationModulePath.exists()
                || !documentationModulePath.isDirectory()) {
            noModulePathInDocumentation = true;
        } else {
            documentationSourcePath.add(documentationModulePath);
        }

        if (implementationpath != null && implementationpath.length() > 0) {
            File stelpDirectory = new File(projectRootDirectoryPath);
            if (!stelpDirectory.exists() || !stelpDirectory.isDirectory()) {
                String stelpCanonicalPath;
                try {
                    stelpCanonicalPath = stelpDirectory.getCanonicalPath();
                    logger
                            .error(
                                    "STELP specified as '{}' but does not exist or is not a directory in '{}'",
                                    projectRootDirectoryPath,
                                    stelpCanonicalPath);
                } catch (IOException e) {
                    stelpCanonicalPath = projectRootDirectoryPath;
                    logger
                            .error(
                                    "STELP specified as '{}' but does not exist or is not a directory",
                                    projectRootDirectoryPath);
                }
                documentationSourcePath = null;
                docbookSourcePath = null;
                throw new FileNotFoundException("STELP directory not found");
            }

            // ok
            File implementationPathDirectory = new File(stelpDirectory,
                    implementationpath);
            if (!implementationPathDirectory.exists()
                    || !implementationPathDirectory.isDirectory()) {
                try {
                    String implementationPathDirectoryCanonicalPath = implementationPathDirectory
                            .getCanonicalPath();
                    logger
                            .error(
                                    "'implementationpath' specified as '{}' but does not exist or is not a directory in '{}'",
                                    implementationpath,
                                    implementationPathDirectoryCanonicalPath);
                } catch (IOException e) {
                    logger
                            .error(
                                    "'implementationpath' specified as '{}' but does not exist or is not a directory in STELP '{}'",
                                    implementationpath,
                                    projectRootDirectoryPath);
                }
                documentationSourcePath = null;
                docbookSourcePath = null;
                throw new FileNotFoundException("Invalid 'implementationpath'");
            }

            File implementationModulePath = new File(
                    implementationPathDirectory, modulePath);
            if (!implementationModulePath.exists()
                    || !implementationModulePath.isDirectory()) {
                try {
                    String implementationModulePathCanonicalPath = implementationModulePath
                            .getCanonicalPath();
                    logger
                            .error(
                                    "'implementationpath' specified as '{}' and <modulepath> specified as '{}' but expected source directory '{}' does not exist or is not a directory in '"
                                            + implementationModulePathCanonicalPath
                                            + "'", implementationpath,
                                    modulePath);
                } catch (IOException e) {
                    logger
                            .error(
                                    "'implementationpath' specified as '{}' and <modulepath> specified as '{}' but expected source directory '{}' does not exist or is not a directory",
                                    implementationpath, modulePath);
                }
                documentationSourcePath = null;
                docbookSourcePath = null;
                throw new FileNotFoundException("Invalid 'modulepath'");
            }

            // ok
            documentationSourcePath.add(implementationModulePath);
            if (noModulePathInDocumentation) {
                // This is ok if the module is in implementation
                try {
                    String documentationModuleCanonicalPath = documentationModulePath
                            .getCanonicalPath();
                    logger
                            .warn(
                                    "<modulepath> specified as '{}' but does not exist or is not a directory as '{}'",
                                    modulePath,
                                    documentationModuleCanonicalPath);
                } catch (IOException e) {
                    logger
                            .warn(
                                    "<modulepath> specified as '{}' but does not exist or is not a directory in '{}'",
                                    modulePath, documentationCmModulePath);
                }
            }
        } else {
            // NO implementation path
            if (noModulePathInDocumentation) {
                // We had no module path either - this is an error,
                // but could possibly be acceptable if there is nothing to copy.
                try {
                    String documentationModuleCanonicalPath = documentationModulePath
                            .getCanonicalPath();
                    logger
                            .error(
                                    "<modulepath> specified as '{}' but does not exist or is not a directory as '{}'",
                                    modulePath,
                                    documentationModuleCanonicalPath);
                } catch (IOException e) {
                    logger
                            .error(
                                    "<modulepath> specified as '{}' but does not exist or is not a directory in '{}'",
                                    modulePath, documentationCmModulePath);
                }
                documentationSourcePath = null;
                docbookSourcePath = null;
                throw new FileNotFoundException("Invalid 'modulepath'");
            }
        }
    }
}
