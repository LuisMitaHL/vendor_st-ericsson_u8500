package com.stericsson.sdk.equipment.io.test;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPFile;

/**
 * 
 * For Unit test FTP client is stubbed
 * 
 * 
 * @author esrimpa
 * 
 */
public class FTPClientStub {

    private int resultcode;

    private boolean resultLogin;

    private boolean resultConenctException = false;

    private boolean resultChdirException = false;

    private boolean resultDisconenctException = false;

    private int resultListFileException = 1;

    private boolean resultRetrieveFileException = false;

    private boolean resultStoreFileException = false;

    private boolean resultDeleteFileException = false;

    private boolean isConnect = true;

    private FTPClient ftpClient = new FTPClient() {

        @Override
        public void connect(String server) throws IOException {
            if (!resultConenctException) {
                return;
            } else {
                throw new IOException("Connect Exception");
            }

        }

        @Override
        public boolean login(String user, String pass) {
            return resultLogin;
        }

        @Override
        public int getReplyCode() {
            return resultcode;

        }

        @Override
        public boolean isConnected() {
            if (isConnect) {
                return true;
            } else {
                // Reset before leaving
                isConnect = true;
                return false;
            }
        }

        @Override
        public boolean changeWorkingDirectory(String path) throws IOException {
            if (!resultChdirException) {
                return true;
            } else {
                throw new IOException("Change Directory Exception");
            }

        }

        @Override
        public void disconnect() throws IOException {

            if (!resultDisconenctException) {
                return;
            } else {
                throw new IOException("Disconnect Exception");
            }

        }

        public FTPFile[] listFiles(String dir) throws IOException {
            if (resultListFileException == 1) {
                FTPFile[] testFile = {
                    new FTPFile()};
                testFile[0].setName("TestFile.txt");
                testFile[0].setType(FTPFile.FILE_TYPE);
                testFile[0].setSize(20);
                return testFile;
            } else if (resultListFileException == 2) {
                return null;

            } else {
                throw new IOException("List file Exception");
            }
        }

        public boolean retrieveFile(String remote, OutputStream local) throws IOException {
            if (resultcode == 0) {
                String output = "/testfiles/";
                local.write(output.getBytes("UTF-8"));
                return true;
            } else if (!resultRetrieveFileException) {
                return false;
            } else {
                throw new IOException("Retrieve file Exception");
            }

        }

        public InputStream retrieveFileStream(String local) throws IOException {

            if (resultcode == 0) {

                InputStream abc = new InputStream() {
                    @Override
                    public int read() throws IOException {
                        return -1;
                    }
                };
                return abc;
            } else if (!resultRetrieveFileException) {
                return null;
            } else {
                throw new IOException("Retrieve file Stream Exception");
            }

        }

        public boolean completePendingCommand() throws IOException {
            return true;
        }

        public boolean storeFile(String remote, InputStream local) throws IOException {

            if (resultcode == 0) {
                return true;
            } else if (!resultStoreFileException) {
                return false;
            } else {
                throw new IOException("Store file Exception");
            }
        }

        public boolean deleteFile(String remote) throws IOException {

            if (!resultDeleteFileException) {
                return true;
            } else {
                throw new IOException("Store file Exception");
            }

        }

    };

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setResult(int result) {
        this.resultcode = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setIsConnect(Boolean result) {
        this.isConnect = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setResultLogin(boolean result) {
        this.resultLogin = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setConenctException(boolean result) {
        this.resultConenctException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setDisconenctException(boolean result) {
        this.resultDisconenctException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setListFileException(int result) {
        this.resultListFileException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setRetrieveFileException(boolean result) {
        this.resultRetrieveFileException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setStoreFileException(boolean result) {
        this.resultStoreFileException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setDeleteFileException(boolean result) {
        this.resultDeleteFileException = result;
    }

    /**
     * stubbed method for unit test
     * 
     * @param result
     *            True/false
     */
    public void setChdirException(boolean result) {
        this.resultChdirException = result;
    }

    /**
     * unit test case to get the FTPClient stub
     * 
     * @return FTPClient
     */
    public FTPClient getFTPClient() {
        return ftpClient;
    }

}
