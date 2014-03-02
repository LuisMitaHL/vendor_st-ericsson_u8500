package com.stericsson.sdk.ftp.communication.test.internal;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.port.IFTPPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.ftp.communication.FTPCommunicationService;
import com.stericsson.sdk.ftp.communication.IFTPCommunicatorListener;

/**
 * @author esrimpa
 * 
 */
public class FTPCommunicationServiceTest extends TestCase {

    /** FAILURE */
    private static final int ERROR = 1;

    /** Success */
    private static final int SUCCESS = 0;

    private static final String USER = "root";

    private static final String PASSWD = "root";

    private FTPCommunicationService testFTPCommunicationService;

    private IPort mockPort;

    private IFTPPort mockFTPPort;

    private IFTPCommunicatorListener mockFTPListener;

    /**
     * Init
     */
    public FTPCommunicationServiceTest() {
        super();
        testFTPCommunicationService = new FTPCommunicationService();
        mockPort = EasyMock.createMock(IPort.class);
        mockFTPPort = EasyMock.createMock(IFTPPort.class);
        mockFTPListener = EasyMock.createMock(IFTPCommunicatorListener.class);
    }

    /**
     * TestCase
     */
    @Test
    public void testListFileConnectFailed() {

        List<String> fileList = new ArrayList<String>();

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(false);

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        fileList = testFTPCommunicationService.listFile(mockFTPPort);

        assertNull(fileList);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testListFileNoFileToDownload() {

        List<String> fileList = new ArrayList<String>();

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);
        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);
        fileList = testFTPCommunicationService.listFile(mockFTPPort);
        if (fileList != null) {
            assertTrue(fileList.isEmpty());
        } else {
            fail("fileList shouldnt be null");
        }
        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testListFileSuccess() {

        List<String> fileList = new ArrayList<String>();

        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);
        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        fileList = testFTPCommunicationService.listFile(mockFTPPort);

        assertNotNull(fileList);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpAutoDeleteFalse() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.uploadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(true);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal =
            testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", false, mockFTPListener);

        assertEquals(SUCCESS, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpAutoDeleteSuccess() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.uploadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(true);
        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(true);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", true, mockFTPListener);

        assertEquals(SUCCESS, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpAutoDeleteFailure() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);
        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);
        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);
        EasyMock.expect(mockFTPPort.uploadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(true);
        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(false);
        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);
        int retVal = testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", true, mockFTPListener);

        assertEquals(ERROR, retVal);
        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    public void testDownloadAllDumpFiles() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.uploadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(true);
        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.downloadDump(mockFTPPort, "all", "Test", false, mockFTPListener);

        assertEquals(SUCCESS, retVal);
        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    public void testDownloadAllDumpFilesFailed() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            false);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.downloadDump(mockFTPPort, "all", "Test", false, mockFTPListener);

        assertEquals(ERROR, retVal);
        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    public void testDownloadAllDumpFilesNoFiles() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(null);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.downloadDump(mockFTPPort, "all", "Test", false, mockFTPListener);

        assertEquals(ERROR, retVal);
        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpFailedConnect() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(false);

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal =
            testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", false, mockFTPListener);

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpFailedDownloadCoreFile() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            false);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal =
            testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", false, mockFTPListener);

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDownloadDumpFailedUploadCoreInfoFile() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        fileList.add("Coredump_2");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.downloadFile(EasyMock.isA(String.class), EasyMock.isA(String.class))).andReturn(
            true);

        EasyMock.expect(mockFTPPort.uploadFile(EasyMock.isA(String.class), EasyMock.isA(String.class)))
            .andReturn(false);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().times(2);

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal =
            testFTPCommunicationService.downloadDump(mockFTPPort, "Coredump_1", "Test", false, mockFTPListener);

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteDumpSuccess() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(true);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "Coredump_1");

        assertEquals(SUCCESS, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteAllDumpsSuccess() {

        List<String> fileList = new ArrayList<String>();

        fileList.add("Coredump_1");

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);

        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);

        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(true);

        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(true);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "all");

        assertEquals(SUCCESS, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteAllDumpDeleteFileFalied() {

        List<String> fileList = new ArrayList<String>();
        fileList.add("Coredump_1");
        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);
        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(fileList);
        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(false);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "all");

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteAllDumpDeleteFileNoFiles() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);
        EasyMock.expect(mockFTPPort.listAllFiles()).andReturn(null);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "all");

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteDumpConnectFailed() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(false);
        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "Coredump_1");

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testDeleteDumpDeleteFileFalied() {

        EasyMock.expect(mockFTPPort.connect(EasyMock.eq(USER), EasyMock.eq(PASSWD))).andReturn(true);
        EasyMock.expect(mockFTPPort.deleteFile(EasyMock.isA(String.class))).andReturn(false);

        mockFTPPort.disconnect();
        EasyMock.expectLastCall().once();

        EasyMock.replay(mockPort, mockFTPPort, mockFTPListener);

        int retVal = testFTPCommunicationService.deleteDump(mockFTPPort, "Coredump_1");

        assertEquals(ERROR, retVal);

        EasyMock.verify(mockPort, mockFTPPort, mockFTPListener);
    }

    /**
     * TestCase
     */
    @Test
    public void testShutdown() {
        assertEquals(SUCCESS, testFTPCommunicationService.shutdown(mockFTPPort));

    }

}
