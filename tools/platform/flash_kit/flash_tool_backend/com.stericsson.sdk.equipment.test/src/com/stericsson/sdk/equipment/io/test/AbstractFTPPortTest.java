package com.stericsson.sdk.equipment.io.test;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.List;

import junit.framework.TestCase;

import org.eclipse.core.runtime.FileLocator;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.AbstractFTPPort;

/**
 * Unit test for Abstract port
 * 
 * @author esrimpa
 * 
 */
public class AbstractFTPPortTest extends TestCase {

    private static final String TEST_FTP_PORT_NAME = "TESTPORT";

    private AbstractFTPPort testAbstractFTPPort;

    private FTPClientStub ftpClientStub;

    /***/
    public static final String TEST_FILE = "/test_files/TestFile.txt";

    /**
     * setUp() method that initializes common objects
     * 
     * @throws Exception
     *             Thows exception
     */
    @Before
    public void setUp() throws Exception {
        super.setUp();

        testAbstractFTPPort = new AbstractFTPPort(TEST_FTP_PORT_NAME) {
        };
        ftpClientStub = new FTPClientStub();
    }

    /**
     * * tearDown() method that cleanup the common objects
     * 
     * @throws Exception
     *             Thows exception
     */
    @After
    public void tearDown() throws Exception {
        super.tearDown();

        testAbstractFTPPort = null;
    }

    /**
     * * Constructor for AbstractFTPPortTest. *
     * */
    public AbstractFTPPortTest() {
        super();
    }

    /**
     * Unit test case
     */
    @Test
    public void testgetIdentifier() {

        assertEquals(testAbstractFTPPort.getPortName(), TEST_FTP_PORT_NAME);

    }

    /**
     * Unit test case
     */
    @Test
    public void testOpen() {

        try {
            testAbstractFTPPort.open();
            assertEquals(testAbstractFTPPort.isOpen(), true);
            testAbstractFTPPort.close();
        } catch (Exception e) {
            System.err.println("Error in open");
        }

    }

    /**
     * Unit test case
     */
    @Test
    public void testAlreadyConnected() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setResult(220);
        ftpClientStub.setIsConnect(true);
        boolean retVal = testAbstractFTPPort.connect();
        assertEquals(retVal, true);
    }

    /**
     * Unit test case
     */
    @Test
    public void testConnectNoUser() {

        FTPClientStub ftpClient = new FTPClientStub();
        testAbstractFTPPort.setFTPClient(ftpClient.getFTPClient());
        ftpClient.setResult(220);
        ftpClient.setIsConnect(false);
        boolean retVal = testAbstractFTPPort.connect();
        assertEquals(retVal, true);

    }

    /**
     * Unit test case
     */
    @Test
    public void testConnectNoUserReplyNegative() {

        FTPClientStub ftpClient = new FTPClientStub();
        testAbstractFTPPort.setFTPClient(ftpClient.getFTPClient());
        ftpClient.setResult(-1);
        ftpClient.setIsConnect(false);
        boolean retVal = testAbstractFTPPort.connect();
        testAbstractFTPPort.disconnect();
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testConnectUserFailed() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setResult(220);
        ftpClientStub.setResultLogin(false);
        ftpClientStub.setIsConnect(false);
        boolean retVal = testAbstractFTPPort.connect("1", "2");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testConnectExpectIOException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setResult(220);
        ftpClientStub.setIsConnect(false);
        ftpClientStub.setConenctException(true);
        ftpClientStub.setDisconenctException(false);
        boolean retVal = testAbstractFTPPort.connect();

        assertEquals(retVal, false);
    }

    /**
     * Unit test case
     */
    @Test
    public void testConnectDisConnectExpectIOException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setResult(220);
        ftpClientStub.setConenctException(true);
        ftpClientStub.setDisconenctException(true);
        ftpClientStub.setIsConnect(false);
        boolean retVal = testAbstractFTPPort.connect();
        testAbstractFTPPort.disconnect();
        assertEquals(retVal, false);
    }

    /**
     * Unit test case
     */
    @Test
    public void testListFile() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setListFileException(1);
        List<String> retVal = testAbstractFTPPort.listAllFiles();
        assertEquals(retVal.get(0), "/testfiles/TestFile.txt");
    }

    /**
     * Unit test case
     */
    @Test
    public void testListFileRetunsNull() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setListFileException(2);
        List<String> retVal = testAbstractFTPPort.listAllFiles();
        assertNull(retVal);

    }

    /**
     * Unit test case
     */
    @Test
    public void testListFileRetunsFailedToGetInfoList() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setListFileException(1);
        ftpClientStub.setResult(1);
        List<String> retVal = testAbstractFTPPort.listAllFiles();
        assertEquals(retVal.get(0), "/sdcard/core_dumps/modem/TestFile.txt");

    }

    /**
     * Unit test case
     */
    @Test
    public void testListFileRetunsExceptionGettingInfoList() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setListFileException(1);
        ftpClientStub.setResult(1);
        ftpClientStub.setRetrieveFileException(true);
        List<String> retVal = testAbstractFTPPort.listAllFiles();
        assertEquals(retVal.get(0), "/sdcard/core_dumps/modem/TestFile.txt");
    }

    /**
     * Unit test case
     */
    @Test
    public void testListFileException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setListFileException(0);
        List<String> retVal = testAbstractFTPPort.listAllFiles();
        assertNull(retVal);
    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadretrieveFileStreamSuccess() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(false);
        boolean retVal = testAbstractFTPPort.downloadFile("/Test/TestFile.txt", "test_files");
        assertEquals(retVal, true);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadretrieveFileStreamException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(true);
        ftpClientStub.setResult(1);
        boolean retVal = testAbstractFTPPort.downloadFile("TestFile.txt", "test_files");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadGetFileSizeFailed() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(false);
        ftpClientStub.setResult(1);
        ftpClientStub.setListFileException(2);
        boolean retVal = testAbstractFTPPort.downloadFile("TestFile.txt", "test_files");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadGetFileSizeDisconnectException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(false);
        ftpClientStub.setResult(1);
        ftpClientStub.setListFileException(3);
        ftpClientStub.setDisconenctException(true);
        boolean retVal = testAbstractFTPPort.downloadFile("TestFile.txt", "test_files");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadGetFileSizeException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(false);
        ftpClientStub.setResult(1);
        ftpClientStub.setListFileException(3);
        boolean retVal = testAbstractFTPPort.downloadFile("TestFile.txt", "test_files");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDownloadRetrieveFileException() {

        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setRetrieveFileException(true);
        ftpClientStub.setResult(1);
        boolean retVal = testAbstractFTPPort.downloadFile("TEST", "TESTDIR");
        assertEquals(retVal, false);
    }

    /**
     * Unit test case
     * 
     * @throws Exception
     *             tbd
     */
    @Test
    public void testUploadStoreFile() throws Exception {
        File testFile = getFile();
        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setStoreFileException(false);
        ftpClientStub.setResult(0);
        boolean retVal = testAbstractFTPPort.uploadFile(testFile.getAbsolutePath(), "TESTDIR");
        assertEquals(retVal, true);
    }

    /**
     * Unit test case
     * 
     * @throws Exception
     *             tbd
     */
    @Test
    public void testUploadStoreFileFailed() throws Exception {
        File testFile = getFile();
        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setStoreFileException(false);
        ftpClientStub.setResult(1);
        boolean retVal = testAbstractFTPPort.uploadFile(testFile.getAbsolutePath(), "TESTDIR");
        assertEquals(retVal, false);
    }

    private File getFile() throws IOException {
        URL url = getClass().getResource(TEST_FILE);
        URL find = null;
        find = FileLocator.toFileURL(url);
        File testFile = new File(find.getFile());
        return testFile;
    }

    /**
     * Unit test case
     */
    @Test
    public void testUploadStoreFileException() {

        File testFile = new File(TEST_FILE);
        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setStoreFileException(true);
        ftpClientStub.setResult(1);
        boolean retVal = testAbstractFTPPort.uploadFile(testFile.getAbsolutePath(), "TESTDIR");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDeleteFileSuccess() {
        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setDeleteFileException(false);
        boolean retVal = testAbstractFTPPort.deleteFile("corefile1");
        assertEquals(retVal, true);

    }

    /**
     * Unit test case
     */
    @Test
    public void testDeleteFileException() {
        testAbstractFTPPort.setFTPClient(ftpClientStub.getFTPClient());
        ftpClientStub.setDeleteFileException(true);
        boolean retVal = testAbstractFTPPort.deleteFile("corefile1");
        assertEquals(retVal, false);

    }

    /**
     * Unit test case
     */
    @Test
    public void isDumping() {

        assertEquals(false, testAbstractFTPPort.isDumping());

    }

}
