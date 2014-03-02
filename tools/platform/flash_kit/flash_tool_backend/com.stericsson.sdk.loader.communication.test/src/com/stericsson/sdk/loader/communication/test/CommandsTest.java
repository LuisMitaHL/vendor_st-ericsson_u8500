package com.stericsson.sdk.loader.communication.test;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.internal.LCTimeouts;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationCommandHandler;
import com.stericsson.sdk.loader.communication.types.LoaderCommunicationType;

/**
 * @author xdancho
 * @return TBD
 */
public class CommandsTest extends TestCase implements IAuthenticate {

    static LoaderCommunicationCommandHandler service = null;

    static long instancePointer = 1;

    static final int TIMEOUT = 10000;

    static final int NUMBER_OF_TIMES_TO_RUN = 10;

    List<String> resultList = new ArrayList<String>();

    Object[] executeSoftwareObject = new Object[] {
        new File("resources/TestTxtFile.txt").getAbsolutePath(), 100L, true, null};

    Object[] processFileObject = new Object[] {
        100L, "x-empflash/flasharchive", new File("resources/testFiles/TestTxtFile.txt").getAbsolutePath(), true, null};

    Object[] emptyObject = new Object[0];

    Object[] dumpAreaObject = new Object[] {
        "/flash0", 0, 100, new File("resources/TestTxtFile.txt").getAbsolutePath(), true, null};

    Object[] eraseAreaObject = new Object[] {
        "/flash0", 0, 268435456};

    Object[] setEnhancedAreaObject = new Object[] {
        "/flash0", 0, 268435456};

    Object[] volumePropertiesObject = new Object[] {
        ":/sys/"};

    Object[] formatVolumeObject = new Object[] {
        ":/sys/"};

    Object[] listDirectoryObject = new Object[] {
        ":/sys"};

    Object[] copyFileObject = new Object[] {
        new File("resources/TestTxtFile.txt").getAbsolutePath(), ":/sys/TestTxtFile.txt", true, false, null};

    Object[] deleteFileObject = new Object[] {
        ":/sys/TestTxtFile.txt"};

    Object[] moveFileObject = new Object[] {
        ":/sys/TestTxtFile.txt", ":/sys/TestTxtFile2.txt"};

    Object[] createDirectoryObject = new Object[] {
        ":/sys/directory"};

    Object[] propertiesObject = new Object[] {
        ":/sys"};

    Object[] changeAccessObject = new Object[] {
        511, "/sys"};

    Object[] readLoadModuleManifestObject = new Object[] {
        new File("resources/testReadModule.bin").getAbsolutePath(), ":/boot/"};

    Object[] readBitsObject = new Object[] {
        9, 0, 8};

    Object[] setBitsObject = new Object[] {
        9, 0, 32, new byte[4]};

    Object[] writeAndLockObject = new Object[] {
        9};

    Object[] readGlobalDataUnitObject = new Object[] {
        "/flash0", 1};

    Object[] writeGlobalDataUnitObject = new Object[] {
        "/flash0", 1, new byte[] {
            0x0F}};

    Object[] readGlobalDataSetObject = new Object[] {
        "/flash0", new File(Activator.getResourcesPath() + "/gdfsRead.gdf").getAbsolutePath(), true, null};

    Object[] writeGlobalDataSetObject =
        new Object[] {
            "/flash0", new File(Activator.getResourcesPath() + "/gdfsRead.gdf").length(),
            new File(Activator.getResourcesPath() + "/gdfsRead.gdf").getAbsolutePath(), true, null};

    Object[] eraseGlobalDataSet = new Object[] {
        "/flash0"};

    Object[] setDomainObject = new Object[] {
        LCDefinitions.DOMAIN_FACTORY_DOMAIN};

    Object[] getPropertiesObject = new Object[] {
        0x10};

    Object[] setPropertiesObject = new Object[] {
        0x10, 192, new byte[192]};

    Object[] updateRequest = new Object[] {
        new byte[1]};

    Object[] changeBaudrate = new Object[] {
        1};

    Object[] writeAndLock = new Object[] {
        1};

    Object[] storeSecureObject = new Object[] {
        new File("resources/gdfsRead.gdf").getAbsolutePath(), LCDefinitions.GLOBAL_DATA_SET_STORAGE_ID_GDFS, true};

    Object[] timeouts = new Object[] {
        1, 1, 1, 1, 1, 1};

    Object[] loaderStartupStatus = new Object[0];

    Object[] reboot = new Object[] {
        1};

    Object[] authenticateControlKeys = new Object[] {
        LCDefinitions.AUTHENTICATE_CONTROL_KEYS, this};

    Object[] authenticateCerts = new Object[] {
        LCDefinitions.AUTHENTICATE_CERTIFICATE, this};

    private static void setCommandHandler(LoaderCommunicationCommandHandler handler) {
        CommandsTest.service = handler;
    }

    /**
     * @return the instancePointer
     */
    private static long getInstancePointer() {
        return instancePointer;
    }

    /**
     * @param ip
     *            the instancePointer to set
     */
    private static void setInstancePointer(long ip) {
        ReaderWriterTest.instancePointer = ip;
    }

    /**
     * 
     */
    public void testAllCommands() {

        ServiceReference sr = null;
        try {
            sr =
                Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                    "(type=normal)")[0];
        } catch (InvalidSyntaxException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        setCommandHandler((LoaderCommunicationCommandHandler) Activator.getBundleContext().getService(sr));

        try {
            setInstancePointer(service.initialize(null));
        } catch (Exception e) {
            e.printStackTrace();
        }

        runSupportedCommands();

        runProcessFile();

        runListDevices();
        runDumpArea();
        runVolumeProperties();
        runFormatVolume();
        runListDirectory();
        runMoveFile();
        runDeleteFile();
        runCopyFile();
        runCreateDirectory();
        runProperties();
        runChangeAccess();
        runReadLoadModuleManifest();
        runReadBits();
        runWriteBits();
        runReadGlobalDataUnit();
        runWriteGlobalDataUnit();
        runWriteGlobalDataSet();
        runReadGlobalDataSet();
        runEraseGlobalDataSet();
        runSetDomain();
        runGetDomain();
        runSetProperties();
        runGetProperties();
        runBindProperties();

        runStoreSecureObject();
        runAuthenticateControlKeys();
        runAuthenticateCert();

        runWriteAndLock();
        runSetBits();
        runEraseArea();
        setEnhancedArea();
        runExecuteSoftware();
        runLoaderStartupStatus();
        runReboot();
        // runChangeBaudrate();
        runShutDown();
        runUnrecognized();
        runCanceledByTimer();

        runCommandWithVerbosePrintouts();

    }

    private Object runAuthenticateCert() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, authenticateCerts);

    }

    private Object runAuthenticateControlKeys() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, authenticateControlKeys);

    }

    private Object runCommandWithVerbosePrintouts() {
        System.setProperty("backend.mode", "normal");
        Object temp = sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_REBOOT, reboot);
        System.setProperty("backend.mode", "test");
        return temp;

    }

    private Object runCanceledByTimer() {
        LCTimeouts.getInstance().setTimeouts(LCDefinitions.COMMAND_GR, 10);
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_REBOOT, reboot);

    }

    private Object runUnrecognized() {
        return sendCommand("unrecognized", emptyObject);

    }

    /**
     * @return TBD
     */
    // public Object runUpdateRequest() {
    // return sendCommand(LCDefinitions.METHOD_NAME_RESET_UPDATE_REQUEST, updateRequest);
    // }

    /**
     * @return TBD
     */
    // public Object runInitRequest() {
    // return sendCommand(LCDefinitions.METHOD_NAME_RESET_INIT_REQUEST, emptyObject);
    // }

    /**
     * @return TBD
     */
    public Object runStoreSecureObject() {
        return sendCommand(LCDefinitions.METHOD_NAME_OTP_STORE_SECURE_OBJECT, storeSecureObject);
    }

    /**
     * @return TBD
     */
    public Object runWriteAndLock() {
        return sendCommand(LCDefinitions.METHOD_NAME_OTP_WRITE_AND_LOCK_BITS, writeAndLock);
    }

    /**
     * @return TBD
     */
    public Object runWriteBits() {
        return sendCommand(LCDefinitions.METHOD_NAME_OTP_WRITE_BITS, setBitsObject);
    }

    /**
     * @return TBD
     */
    public Object runEraseArea() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_ERASE_AREA, eraseAreaObject);
    }

    /**
     * @return TBD
     */
    public Object runShutDown() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_SHUTDOWN, emptyObject);
    }

    /**
     * @return TBD
     */
    public Object runChangeBaudrate() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_CHANGE_BAUDRATE, changeBaudrate);
    }

    /**
     * @return TBD
     */
    public Object runReboot() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_REBOOT, reboot);
    }

    /**
     * @return TBD
     */
    public Object runLoaderStartupStatus() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_LOADER_STARTUP_STATUS, loaderStartupStatus);
    }

    /**
     * @return TBD
     */
    public Object runSupportedCommands() {

        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS, emptyObject);
    }

    /**
     * @return TBD
     */
    public Object runExecuteSoftware() {
        return sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_EXECUTE_SOFTWARE, executeSoftwareObject);
    }

    /**
     * @return TBD
     */
    public Object runProcessFile() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_PROCESS_FILE, processFileObject);
    }

    /**
     * @return TBD
     */
    public Object runListDevices() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, emptyObject);
    }

    /**
     * @return TBD
     */
    public Object runDumpArea() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_DUMP_AREA, dumpAreaObject);
    }

    /**
     * @return TBD
     */
    public Object eraseArea() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_ERASE_AREA, eraseAreaObject);
    }

    /**
     * method sends command set_enhanced_area
     * 
     * @return result
     */
    public Object setEnhancedArea() {
        return sendCommand(LCDefinitions.METHOD_NAME_FLASH_SET_ENHANCED_AREA, setEnhancedAreaObject);
    }

    /**
     * @return TBD
     */
    public Object runVolumeProperties() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_VOLUME_PROPERTIES, volumePropertiesObject);
    }

    /**
     * @return TBD
     */
    public Object runFormatVolume() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_FORMAT_VOLUME, formatVolumeObject);
    }

    /**
     * @return TBD
     */
    public Object runListDirectory() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_LIST_DIRECTORY, listDirectoryObject);
    }

    /**
     * @return TBD
     */
    public Object runMoveFile() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_MOVE_FILE, moveFileObject);
    }

    /**
     * @return TBD
     */
    public Object runDeleteFile() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_DELETE_FILE, deleteFileObject);
    }

    /**
     * @return TBD
     */
    public Object runCopyFile() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_COPY_FILE, copyFileObject);
    }

    /**
     * @return TBD
     */
    public Object runCreateDirectory() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_CREATE_DIRECTORY, createDirectoryObject);
    }

    /**
     * @return TBD
     */
    public Object runProperties() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_PROPERTIES, propertiesObject);
    }

    /**
     * @return TBD
     */
    public Object runChangeAccess() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_CHANGE_ACCESS, changeAccessObject);
    }

    /**
     * @return TBD
     */
    public Object runReadLoadModuleManifest() {
        return sendCommand(LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS, readLoadModuleManifestObject);
    }

    /**
     * @return TBD
     */
    public Object runReadBits() {
        return sendCommand(LCDefinitions.METHOD_NAME_OTP_READ_BITS, readBitsObject);
    }

    /**
     * @return TBD
     */
    public Object runSetBits() {
        return sendCommand(LCDefinitions.METHOD_NAME_OTP_SET_BITS, setBitsObject);

    }

    /**
     * @return TBD
     */
    public Object runReadGlobalDataUnit() {
        return sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_UNIT, readGlobalDataUnitObject);
    }

    /**
     * @return TBD
     */
    public Object runWriteGlobalDataUnit() {
        return sendCommand(LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_UNIT, writeGlobalDataUnitObject);
    }

    /**
     * @return TBD
     */
    public Object runReadGlobalDataSet() {
        return sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_SET, readGlobalDataSetObject);
    }

    /**
     * @return TBD
     */
    public Object runWriteGlobalDataSet() {
        return sendCommand(LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_SET, writeGlobalDataSetObject);
    }

    /**
     * @return TBD
     */
    public Object runEraseGlobalDataSet() {
        return sendCommand(LCDefinitions.METHOD_NAME_PS_ERASE_GLOBAL_DATA_SET, eraseGlobalDataSet);
    }

    /**
     * @return TBD
     */
    public Object runSetDomain() {
        return sendCommand(LCDefinitions.METHOD_NAME_SECURITY_SET_DOMAIN, setDomainObject);
    }

    /**
     * @return TBD
     */
    public Object runGetDomain() {
        return sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_DOMAIN, emptyObject);
    }

    /**
     * @return TBD
     */
    public Object runGetProperties() {
        return sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_PROPERTY, getPropertiesObject);
    }

    /**
     * @return TBD
     */
    public Object runSetProperties() {
        return sendCommand(LCDefinitions.METHOD_NAME_SECURITY_SET_PROPERTY, setPropertiesObject);
    }

    /**
     * @return TBD
     */
    public Object runBindProperties() {
        return sendCommand(LCDefinitions.METHOD_NAME_SECURITY_BIND_PROPERTIES, emptyObject);
    }

    private Object sendCommand(String cmd, Object[] args) {

        Object result = service.sendCommand(getInstancePointer(), cmd, args, null);

        if (result != null) {
            int status = 0;
            if (result instanceof LoaderCommunicationType) {
                status = ((LoaderCommunicationType) result).getStatus();
            } else {
                status = ((Integer) result);
            }
            resultList.add("Command: " + cmd + " result: " + status);
        }
        return result;
    }

    /**
     * {@inheritDoc}
     */
    public byte[] getChallengeResponse(byte[] challenge) {
        // TODO Auto-generated method stub
        return new byte[] {
            (byte) 0xFF};
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getControlKeys() {
        // TODO Auto-generated method stub
        return new Object[] {
            0xFF};
    }
}
