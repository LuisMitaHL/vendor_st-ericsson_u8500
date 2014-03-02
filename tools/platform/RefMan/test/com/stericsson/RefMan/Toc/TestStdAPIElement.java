/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;
import java.util.Vector;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Exception.IncludeDocNotSpecifiedException;

public class TestStdAPIElement {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestStdAPIElement.class);

    private StdAPIElement elmCorrect1;
    private StdAPIElement elmCorrect2;
    private StdAPIElement elmIncorrect;
    private StdAPIElement elmNoTopics;

    // Constants used by testCalculateCopyDirPath tests

    private static final String IMPLEMENTATION_CM_MODULE = "implementation_cm_module";
    private static final String MODULE_PATH_IN_CM = "mymodule";
    private static final String MYMODULE_NAME_IGNORED_BY_TEST = "mymodule-ignored-by-test";
    private static final String MYMODULE_XML_IGNORED_BY_TEST = "mymodule-ignored-by-test.xml";
    private static final String MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST = "My Module-ignored-by-test";
    private static final String STELP_DIRECTORY_PATH = "test/com/stericsson/RefMan/Toc/StdAPIElement/stelp/";
    private static final String INCORRECT_MODULE_PATH_IN_IM = "incorrect_module_path_in_im";
    private static final String INCORRECT_MODULE_PATH_IN_DM = "incorrect_module_path_in_dm";
    private static final String DUMMY_DM_XML = "docbook_dm.xml";
    private final File tocLocationsFile = new File(STELP_DIRECTORY_PATH,
            "documentation_cm_module/toc-locations.xml");
    private final File dummyImplementationFile = new File(STELP_DIRECTORY_PATH,
            "implementation_cm_module/mymodule/dummy_im.h");
    private final File dummyDocumentationFile = new File(STELP_DIRECTORY_PATH,
            "documentation_cm_module/mymodule/dummy_dm.h");
    private final File tempCopyDir = new File(STELP_DIRECTORY_PATH, "tempCopyDir");

    private static final String STD_API_TOC_FILE = "test/com/stericsson/RefMan/Toc/StdApiElement/stelp/stdapi-copy-files.xml";

    @Before
    public void setUp() throws Exception {
        if (!tempCopyDir.exists()) {
            tempCopyDir.mkdir();
        }
        elmCorrect1 = new StdAPIElement();
        elmCorrect1.setLabel("title_correct");
        elmCorrect1.setHref("doc_correct");
        elmCorrect1.setModuleName("name_correct");
        elmCorrect1.setModulePath("path_correct");
        Vector<IncludeDoc> vDirs1 = new Vector<IncludeDoc>();
        vDirs1.add(new IncludeDoc("Directory", "destination", true));
        Vector<IncludeDoc> vFiles1 = new Vector<IncludeDoc>();
        vFiles1.add(new IncludeDoc("file", "destination", false));
        elmCorrect1.setIncludeDir(vDirs1);
        elmCorrect1.setIncludeFiles(vFiles1);
        elmCorrect2 = new StdAPIElement();
        elmCorrect2.setLabel("title_correct");
        elmCorrect2.setHref("doc_correct");
        elmCorrect2.setModuleName("name_correct");
        elmCorrect2.setModulePath("path_correct");
        Vector<IncludeDoc> vDirs2 = new Vector<IncludeDoc>();
        vDirs2.add(new IncludeDoc("Directory", "destination", true));
        Vector<IncludeDoc> vFiles2 = new Vector<IncludeDoc>();
        vFiles2.add(new IncludeDoc("file", "destination", false));
        elmCorrect2.setIncludeDir(vDirs2);
        elmCorrect2.setIncludeFiles(vFiles2);
        elmIncorrect = new StdAPIElement();
        elmIncorrect.setLabel("title_correct");
    }

    @After
    public void tearDown() throws Exception {
        deleteDirs(tempCopyDir);
    }

    /**
     * Help method to remove temporary files and folders
     * */
    private void deleteDirs(File file) {
        if (file.isDirectory()) {
            File[] children = file.listFiles();
            for (int i = 0; i < children.length; i++) {
                deleteDirs(children[i]);
            }
        }
        file.delete();
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.TopicElement#equals(Object)}.
     */
    @Test
    public void testEquals() {
        assertTrue(elmCorrect1.equals(elmCorrect2));
        assertTrue(!elmCorrect1.equals(elmIncorrect));
        assertTrue(!elmCorrect1.equals(elmNoTopics));

    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#hashCode()}
     * .
     */
    @Test
    public void testHashcode() {
        assertTrue(elmCorrect1.hashCode() == elmCorrect2.hashCode());
        assertTrue(elmCorrect1.hashCode() != elmIncorrect.hashCode());
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#toString()}
     * .
     */
    @Test
    public void testToString() {
        String correct = "type: StdAPI; title: title_correct; documentation: doc_correct;"
                + " moduleName: name_correct; modulePath: path_correct";
        String string = elmCorrect1.toString();

        assertTrue(string.compareTo(correct) == 0);
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#getType()}.
     *
     * Test getType() to return 'TOPIC'.
     */
    @Test
    public void testGetType() {
        assertTrue(elmCorrect1.getType().compareTo("StdAPI") == 0);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     *
     */
    @Test
    public void testCalculateCopyDirPathTestSetup() {
        assertTrue(tocLocationsFile.exists());
        assertTrue(dummyImplementationFile.exists());
        assertTrue(dummyDocumentationFile.exists());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     * .
     *
     * @throws IOException
     */
    @Test
    public void testCalculateCopyDirPathimplPathIsNull() throws IOException {

        StdAPIElement elem = new StdAPIElement(
                MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST,
                MYMODULE_XML_IGNORED_BY_TEST, MYMODULE_NAME_IGNORED_BY_TEST,
                MODULE_PATH_IN_CM, null, null, null, null);
        elem.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        Vector<File> path = elem.getCopyDirPath();
        assertNotNull(path);
        assertTrue(path.size() == 1);
        assertTrue(fileExistsInPath(dummyDocumentationFile, path));
        assertFalse(fileExistsInPath(dummyImplementationFile, path));

        File docbookPath = elem.getDocBookDirectory();
        assertTrue(new File(docbookPath, DUMMY_DM_XML).exists());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     *
     */
    @Test
    public void testCalculateCopyDirPathimplPathIsNullButModuleIncorrect() {

        StdAPIElement elem = new StdAPIElement(
                MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST,
                MYMODULE_XML_IGNORED_BY_TEST, MYMODULE_NAME_IGNORED_BY_TEST,
                "incorrect_module_path", null, null, null, null);
        try {
            elem.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
            fail("Expected FileNotFoundException here");
        } catch (IOException e) {
            // OK!
        }

        Vector<File> path = elem.getCopyDirPath();
        assertNull(path);
        assertNull(elem.getDocBookDirectory());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     *
     */
    @Test
    public void testCalculateCopyDirPathimplPathIsSetButModuleIncorrectInIMPath() {

        StdAPIElement elem = new StdAPIElement(
                MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST,
                MYMODULE_XML_IGNORED_BY_TEST, MYMODULE_NAME_IGNORED_BY_TEST,
                INCORRECT_MODULE_PATH_IN_IM, IMPLEMENTATION_CM_MODULE, null,
                null, null);
        try {
            elem.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
            fail("Expected FileNotFoundException here");
        } catch (IOException e) {
            // OK
        }

        Vector<File> path = elem.getCopyDirPath();
        assertNull(path);
        assertNull(elem.getDocBookDirectory());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     * .
     *
     * @throws IOException
     */
    @Test
    public void testCalculateCopyDirPathimplPathIsSetButModuleIncorrectInDMPath()
            throws IOException {

        StdAPIElement elem = new StdAPIElement(
                MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST,
                MYMODULE_XML_IGNORED_BY_TEST, MYMODULE_NAME_IGNORED_BY_TEST,
                INCORRECT_MODULE_PATH_IN_DM, IMPLEMENTATION_CM_MODULE, null,
                null, null);
        elem.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        Vector<File> path = elem.getCopyDirPath();
        assertNotNull(path);
        assertTrue(path.size() == 1);
        assertTrue(fileExistsInPath(dummyImplementationFile, path));

        File docbookPath = elem.getDocBookDirectory();
        assertTrue(new File(docbookPath, DUMMY_DM_XML).exists());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.StdAPIElement#calculatePaths(File, String)}
     * .
     *
     * @throws IOException
     */
    @Test
    public void testCalculateCopyDirPathimplPathIsSet() throws IOException {

        StdAPIElement elem = new StdAPIElement(
                MY_MODULE_DOCUMENATION_NAME_IGNORED_BY_TEST,
                MYMODULE_XML_IGNORED_BY_TEST, MYMODULE_NAME_IGNORED_BY_TEST,
                MODULE_PATH_IN_CM, IMPLEMENTATION_CM_MODULE, null, null, null);
        elem.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        Vector<File> path = elem.getCopyDirPath();
        assertNotNull(path);
        assertTrue(path.size() == 2);
        assertTrue(fileExistsInPath(dummyDocumentationFile, path));
        assertTrue(fileExistsInPath(dummyImplementationFile, path));

        File docbookPath = elem.getDocBookDirectory();
        assertTrue(new File(docbookPath, DUMMY_DM_XML).exists());
    }

    private boolean fileExistsInPath(File fileToLocate, Vector<File> path) {
        String fileName = fileToLocate.getName();
        for (File testPath : path) {
            File testFile = new File(testPath, fileName);
            if (testFile.exists() && testFile.isFile()) {
                return true;
            }
        }
        return false;
    }

    /**
     * Test method for { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFiles(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyIncludeFiles() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includeFile = new IncludeDoc("includeDir" + File.separator
                + "dummy1.h", "temp", false);
        Vector<IncludeDoc> v = new Vector<IncludeDoc>();
        v.add(includeFile);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", null, v, null);
        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertTrue(stdApi.copyIncludeFiles(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp").listFiles();
        assertTrue(files.length == 1);
        assertTrue(files[0].getName().compareTo("dummy1.h.html") == 0);
    }

    /**
     * Test method for { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFiles(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyIncludeFilesWithSameName() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includeFile1 = new IncludeDoc("includeDir" + File.separator
                + "dummy1.h", "temp4", false);
        IncludeDoc includeFile2 = new IncludeDoc("includeDir" + File.separator
                + "dummy1.h", "temp4", false);
        Vector<IncludeDoc> v = new Vector<IncludeDoc>();
        v.add(includeFile1);
        v.add(includeFile2);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", null, null, v, null);
        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        assertFalse(stdApi.copyIncludeFiles(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp4").listFiles();
        if (files == null) {
            logger.info("null");
        }
        assertTrue(files.length == 1);
    }

    /**
     * Test method for { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFilesDirectory(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyIncludeFilesDirectory() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includeDir = new IncludeDoc("includeDir", "temp2", true);
        Vector<IncludeDoc> v = new Vector<IncludeDoc>();
        v.add(includeDir);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", v, null, null);

        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertTrue(stdApi.copyIncludeFilesDirectory(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp2").listFiles();
        assertTrue(files.length == 3);
    }

    /**
     * Test method for { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFilesDirectory(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyIncludeFilesDirectoryWithSameFiles()
            throws IOException, IncludeDocNotSpecifiedException {
        IncludeDoc includeDir1 = new IncludeDoc("includeDir", "temp3", true);
        IncludeDoc includeDir2 = new IncludeDoc("includeDir", "temp3", true);
        Vector<IncludeDoc> v = new Vector<IncludeDoc>();
        v.add(includeDir1);
        v.add(includeDir2);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", v, null, null);
        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertFalse(stdApi.copyIncludeFilesDirectory(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp3").listFiles();
        assertTrue(files.length == 3);
    }

    /**
     * Test method for { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFilesDirectory(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyIncludeFilesDirectoryRecursion() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includeDir1 = new IncludeDoc("includes", "temp6", true);
        Vector<IncludeDoc> v = new Vector<IncludeDoc>();
        v.add(includeDir1);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", v, null, null);
        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertTrue(stdApi.copyIncludeFilesDirectory(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp6").listFiles();
        assertTrue(files.length == 2);
        for (File file : files) {
            if (file.isDirectory()) {
                File[] f = file.listFiles();
                assertTrue(f.length == 1);
            }
        }
    }

    /**
     * Test to copy several files with { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFilesDirectory(File)}
     * and { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFiles(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopySeveralDifferentFiles() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includeDir = new IncludeDoc("includeDir", "temp5", true);
        Vector<IncludeDoc> vDir = new Vector<IncludeDoc>();
        vDir.add(includeDir);
        StdAPIElement stdApi = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", vDir, null, null);
        IncludeDoc includeDir2 = new IncludeDoc("includeDir", "temp5", true);
        Vector<IncludeDoc> v2 = new Vector<IncludeDoc>();
        v2.add(includeDir2);
        StdAPIElement stdApi2 = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", vDir, null, null);
        IncludeDoc includeFile3 = new IncludeDoc("dummy2.h", "temp5", false);
        Vector<IncludeDoc> vFiles2 = new Vector<IncludeDoc>();
        vFiles2.add(includeFile3);
        StdAPIElement stdApi3 = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule" + File.separator + "includeDir", "",
                null, vFiles2, null);
        stdApi.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        stdApi2.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        stdApi3.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertTrue(stdApi3.copyIncludeFiles(tempCopyDir));
        assertFalse(stdApi.copyIncludeFilesDirectory(tempCopyDir));
        assertFalse(stdApi2.copyIncludeFilesDirectory(tempCopyDir));

        File[] files = new File(tempCopyDir, "moduleName\\temp5").listFiles();
        assertTrue(files.length == 3);
    }

    /**
     * Test to copy several files with { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFilesDirectory(File)}
     * and { @link
     * com.stericsson.RefMan.Toc.StdAPIElement#copyIncludeFiles(File)}
     *
     * @throws IOException
     * @throws IncludeDocNotSpecifiedException
     */
    @Test
    public void testCopyingFiles() throws IOException,
            IncludeDocNotSpecifiedException {
        IncludeDoc includedir_a = new IncludeDoc("includedir_a",
                "includedir_a", true);
        Vector<IncludeDoc> vDir_a = new Vector<IncludeDoc>();
        vDir_a.add(includedir_a);
        StdAPIElement stdApi_DirA = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", vDir_a, null, null);
        IncludeDoc includedir_b = new IncludeDoc("includedir_b",
                "includedir_b", true);
        Vector<IncludeDoc> vDir_b = new Vector<IncludeDoc>();
        vDir_b.add(includedir_b);
        StdAPIElement stdApi_DirB = new StdAPIElement("title", "documentation",
                "moduleName", "mymodule", "", vDir_b, null, null);

        IncludeDoc includeFile2 = new IncludeDoc("source_2" + File.separator
                + "dummy2.h", "", false);
        Vector<IncludeDoc> vFiles2 = new Vector<IncludeDoc>();
        vFiles2.add(includeFile2);
        StdAPIElement stdApi_File2 = new StdAPIElement("title",
                "documentation", "moduleName", "mymodule", "", null, vFiles2,
                null);
        IncludeDoc includeFile1 = new IncludeDoc("source_1" + File.separator
                + "dummy2.h", "includedir_b", false);
        Vector<IncludeDoc> vFiles1 = new Vector<IncludeDoc>();
        vFiles1.add(includeFile1);
        StdAPIElement stdApi_File1 = new StdAPIElement("title",
                "documentation", "moduleName", "mymodule", "", null, vFiles1,
                null);

        stdApi_DirA.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        stdApi_DirB.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        stdApi_File2.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);
        stdApi_File1.calculatePaths(tocLocationsFile, STELP_DIRECTORY_PATH);

        assertTrue(stdApi_DirA.copyIncludeFilesDirectory(tempCopyDir));
        assertTrue(stdApi_DirB.copyIncludeFilesDirectory(tempCopyDir));
        assertTrue(stdApi_File2.copyIncludeFiles(tempCopyDir));
        assertFalse(stdApi_File2.copyIncludeFiles(tempCopyDir));
    }

    @Test
    public void testWithStdApiTocFile() {
        File file = new File(STD_API_TOC_FILE);
        Element tocElm = ElementFactory.getElement(file, 0);

        try {
            StdAPIElement elm = (StdAPIElement) tocElm.getTopics().get(0)
                    .getTopics().get(0).getTopics().get(0);
            elm.calculatePaths(file, STELP_DIRECTORY_PATH);
            if (elm.getIncludeDirs() != null) {
                elm.copyIncludeFilesDirectory(tempCopyDir);
            }
            if (elm.getIncludeFiles() != null) {
                elm.copyIncludeFiles(tempCopyDir);
            }
        } catch (IOException e) {
            logger.info("Failed to process {}", file.getName(), e.getMessage());
        } catch (IncludeDocNotSpecifiedException e) {
            logger.info("Failed to copy include files for {}", file.getName());
        } catch (ClassCastException e) {
            logger.info("The file is not a standard api toc file."
                    + " {} will be skiped", file.getName(), e);
        }

        File[] files = new File(tempCopyDir, "test").listFiles();
        assertTrue(files.length == 2);
        for (File folder : files) {
            if (folder.isDirectory()) {
                File[] f = folder.listFiles();
                assertTrue(f.length == 1);
            }
        }
    }
}
