package com.stericsson.sdk.loader.communication.internal;

import static com.stericsson.sdk.common.EnvironmentProperties.getLibraryExtension;
import static com.stericsson.sdk.common.EnvironmentProperties.is64BitVM;
import static com.stericsson.sdk.common.EnvironmentProperties.isMac;
import static com.stericsson.sdk.common.EnvironmentProperties.isUnix;
import static com.stericsson.sdk.common.EnvironmentProperties.isWindows;

import java.io.File;
import java.util.Map;
import java.util.Observer;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.Activator;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.parser.LCGDFSParser;
import com.stericsson.sdk.loader.communication.types.AuthenticationType;
import com.stericsson.sdk.loader.communication.types.LoaderCommunicationType;

/**
 * @author xdancho
 */
public final class LoaderCommunicationInterface {

    // ---------------ERROR-----------------------------------------

    /** Short error description */
    public static final int ERROR_SHORT_DESCRIPTION = 1;

    /** Long error description */
    public static final int ERROR_LONG_DESCRIPTION = 2;

    /** */
    private static final String LIB_NAME = "lc_jni";

    /** */
    private static final String LIB_NAME_X64 = "lc_jni_x64";

    /** */
    private static final String LCD_DLL_NAME = "LCDriver_CNH1606432.dll";

    /** */
    private static final String LCD_DLL_NAME_X64 = "LCDriver_CNH1606432_x64.dll";

    /** */
    private static final String LCD_SO_NAME = "liblcdriver.so";

    /** */
    private static final String LCD_SO_NAME_X64 = "liblcdriver_x64.so";

    /** */
    private static final String LCD_DYLIB_NAME_X64 = "liblcdriver_x64.dylib";// liblcdriver_MacOS

    /** */
    private static final String LCM_DLL_NAME = "LCM.dll";

    /** */
    private static final String LCM_DLL_NAME_X64 = "LCM_x64.dll";

    /** */
    private static final String LCM_SO_NAME = "liblcm.so.1";

    /** */
    private static final String LCM_SO_NAME_X64 = "liblcm_x64.so.1";

    /** */
    private static final String LCM_DYLIB_NAME_X64 = "liblcm_x64.dylib";

    private static LoaderCommunicationInterface interfaceInstance = null;

    static Map<Long, LoaderCommunicationContext> ctxList = new ConcurrentHashMap<Long, LoaderCommunicationContext>();

    private static Logger logger = Logger.getLogger("LoaderCommunicationInterface");

    static String sEPAPath;

    static String sGDFSPath;

    static String separator;

    private static Map<Long, ILoaderCommunicationListener> listeners =
        new ConcurrentHashMap<Long, ILoaderCommunicationListener>();

    private static Map<Long, ReaderThread> instanceReaderThreadMap = new ConcurrentHashMap<Long, ReaderThread>();

    private static Map<Long, WriterThread> instanceWriterThreadMap = new ConcurrentHashMap<Long, WriterThread>();

    // private static boolean copyFilesToEPA = true; // should be false when
    // building PA

    private static boolean useCommunicationPrintouts = true; // true =

    // printout,
    // false = no

    // printouts at all

    // private static boolean disableChunkPrintouts = true; // true = does not

    // print bigger than

    // 600b, false = print everything

    /**
     * the initialize method for an instance
     * 
     * @return an id for the instance
     */
    private native long initialize() throws Exception;

    /**
     * deinitialize an instance
     * 
     * @param instancePointer
     * @return
     * @throws Exception
     */
    private native int deInitialize(long instancePointer) throws Exception;

    /**
     * 
     * @param instancePointer
     * @param cmdAck
     *            command acknowledge
     * @param bulkPacketReceived
     *            bulk command packet received
     * @param bulkSessionEnd
     *            bulk session end
     * @param bulkDataReceived
     *            bulk data packet received
     */
    private native int nSetTimeouts(long instancePointer, int cmdAck, int bulkPacketReceived, int bulkSessionEnd,
        int bulkDataReceived);

    /**
     * @param instancePointer
     * @return timeoutType
     */
    // private native Object nGetTimeouts(long instancePointer);

    /**
     * set the path to the Loader Communication Driver dll
     * 
     * @param path
     */
    private native void setLCDPath(String path);

    /**
     * set the path to the Loader Communication Module
     * 
     * @param path
     */
    private native void setLCMPath(String path);

    /**
     * get the error description from Loader Communication Module
     * 
     * @param error
     *            error description
     */
    private native String getLoaderErrorDescription(int error);

    /**
     * callback to the LCM when reading data
     * 
     * @param data
     * @param length
     * @param dataPointer
     * @param instancePointer
     */
    private static native void readReply(byte[] data, long length, long dataPointer, long instancePointer);

    /**
     * callback to the LCM when writing data
     * 
     * @param length
     * @param dataPointer
     * @param instancePointer
     */
    private static native void writeReply(long length, long dataPointer, long instancePointer);

    /**
     * @param length
     *            tbd
     * @param dataPointer
     *            tbd
     * @param instancePointer
     *            tbd
     */
    protected static void wReply(long length, long dataPointer, long instancePointer) {
        if (ctxList.containsKey(instancePointer)) {
            writeReply(length, dataPointer, instancePointer);
        }
    }

    /**
     * @param data
     *            tbd
     * @param length
     *            tbd
     * @param dataPointer
     *            tbd
     * @param instancePointer
     *            tbd
     */
    protected static void rReply(byte[] data, long length, long dataPointer, long instancePointer) {
        if (ctxList.containsKey(instancePointer)) {
            readReply(data, length, dataPointer, instancePointer);
        }
    }

    /**
     * 
     * @param error
     *            .
     * @return description .
     */
    public String getLoaderErrorDesc(int error) {
        return interfaceInstance.getLoaderErrorDescription(error);
    }

    // /**
    // * cancel operations
    // *
    // * @param instancePointer
    // */
    // private native void cancelReply(long instancePointer);

    // native COMMANDS

    /** System Command Group */

    private native Object nSystemLoaderStartupStatus(long instancePointer);

    // private native int nSystemChangeBaudRate(long instancePointer, int baudrate);

    private native int nSystemReboot(long instancePointer, int mode);

    private native int nSystemShutdown(long instancePointer);

    private native Object nSystemSupportedCommands(long instancePointer);

    private native int nSystemExecuteSoftware(long instancePointer, String sourcePath, boolean useBulk);

    private native Object nSystemAuthenticate(long instancePointer, int type);

    private native int nSystemGetControlKeys(long instancePointer, Object[] data); // TODO:

    private native int nSystemAuthenticationChallenge(long instancePointer, int length, byte[] data); // TODO:

    private native Object nSystemCollectData(long instancePointer, int type);

    private native int nSystemChangeBaudRate(long instancePointer, int baudrate);

    private native int nSystemDeauthenticate(long instancePointer, int permanentFlag);

    /** Flash Command Group */

    private native int nFlashProcessFile(long instancePointer, String path, String type, boolean useBulk);

    private native Object nFlashListDevices(long instancePointer);

    private native int nFlashDumpArea(long instancePointer, String sourcePath, long start, long length,
        String destPath, int extraDumpParameters, boolean useBulk);

    private native int nFlashEraseArea(long instancePonter, String sourcePath, long start, long length);

    private native int nFlashSetEnhancedArea(long instancePonter, String sourcePath, long start, long length);

    private native int nFlashSelectLoaderOptions(long instancePonter, long uiProperty, long uiValue);

    /** File System Command Group */

    // private native int nFSSetReadOnly(long instancePointer);

    private native Object nFSVolumeProperties(long instancePointer, String devicePath);

    private native int nFSFormatVolume(long instancePointer, String devicePath);

    private native Object nFSListDirectory(long instancePointer, String fileSystemPath);

    private native int nFSMoveFile(long instancePointer, String sourcePath, String destPath);

    private native int nFSDeleteFile(long instancePointer, String fileSystemFile);

    private native int nFSCopyFile(long instancePointer, String sourcePath, String destPath, boolean srcUseBulk,
        boolean destUseBulk);

    private native int nFSCreateDirectory(long instancePointer, String targetPath);

    private native Object nFSProperties(long instancePointer, String targetPath);

    private native int nFSChangeAccess(long instancePointer, String targetPath, int access);

    private native int nFSReadLoadModuleManifests(long instancePointer, String targetPath, String sourcePath);

    /** OTP Handling Group */

    private native Object nOTPReadBits(long instancePointer, int id, int start, int length);

    // private native int nOTPWriteBits(long instancePointer, int id, int start, int length, byte[]
    // bits);

    private native int nOTPSetBits(long instancePointer, int id, int start, int length, byte[] bits);

    private native int nOTPWriteAndLockBits(long instancePointer, int id);

    private native int nOTPStoreSecurityObject(long instancePointer, String sourcePath, int destination, boolean useBulk);

    /** Parameter Storage Command Group */

    private native Object nPSReadGlobalDataUnit(long instancePointer, String devicePath, int unitId);

    private native int nPSWriteGlobalDataUnit(long instancePoiner, String devicePath, int unitId, byte[] data,
        int length);

    private native int nPSReadGlobalDataSet(long instancePointer, String devicePath, String sourcePath, boolean useBulk);

    private native int nPSWriteGlobalDataSet(long instancePointer, String devicePath, long dataLength,
        String sourcePath, boolean useBulk);

    private native int nPSEraseGlobalDataSet(long instancePointer, String devicePath);

    /** Security Settings Command Group */

    private native int nSecuritySetDomain(long instancePointer, int domain);

    private native Object nSecurityGetDomain(long instancePointer);

    private native Object nSecurityGetProperties(long instancePointer, int propertyId);

    private native int nSecuritySetProperties(long instancePointer, int propertyId, int dataLenght, byte[] buffer);

    private native int nSecurityBindProperties(long instancePointer);

    private native int nSecurityWriteRpmbKey(long instancePointer, int deviceId, boolean commercial);

    private native int nSecurityInitArbTable(long instancePointer, int iType, int piLength, byte[] puchData);

    // ------------------SYSTEM---------------------------------------------

    /**
     * @param instancePointer
     *            instance id
     * @return result
     */
    protected int systemLoaderStartupStatus(long instancePointer) {

        LoaderCommunicationType lst = (LoaderCommunicationType) nSystemLoaderStartupStatus(instancePointer);
        return lst.getStatus();
    }

    /**
     * @param instancePointer
     *            instance id
     * @param baudrate
     *            baudrate
     * @return result
     */
    protected int systemChangeBaudRate(long instancePointer, int baudrate) {
        return nSystemChangeBaudRate(instancePointer, baudrate);
    }

    /**
     * 
     * @param instancePointer
     *            instance id
     * @param permanentFlag
     *            permanent Flag
     * @return result
     */
    protected int systemDeauthenticate(long instancePointer, int permanentFlag) {
        return nSystemDeauthenticate(instancePointer, permanentFlag);
    };

    /**
     * @param instancePointer
     *            instance id
     * @param mode
     *            mode of reboot
     * @return result
     */
    protected int systemReboot(long instancePointer, int mode) {
        return nSystemReboot(instancePointer, mode);
    }

    /**
     * @param instancePointer
     *            instance id
     * @return result
     */
    protected int systemShutdown(long instancePointer) {
        return nSystemShutdown(instancePointer);
    }

    /**
     * @param instancePointer
     *            instance id
     * @return result
     */
    protected Object systemSupportedCommands(long instancePointer) {

        LoaderCommunicationType cmdType = (LoaderCommunicationType) nSystemSupportedCommands(instancePointer);

        if (cmdType.getStatus() > 0) {
            return null;
        }

        return cmdType;

    }

    /**
     * @param instancePointer
     * 
     *            TBD
     * @param sourcePath
     *            TBD
     * @param length
     *            TBD
     * @param useBulk
     *            TBD
     * @param l
     *            TBD
     * @return TBD
     */
    protected int systemExecuteSoftware(long instancePointer, String sourcePath, long length, boolean useBulk,
        Observer l) {
        return nSystemExecuteSoftware(instancePointer, sourcePath, useBulk);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param type
     *            type of authentication
     * @param authenticate
     *            Authentication interface
     * @return result
     * 
     */
    protected int systemAuthenticate(long instancePointer, int type, IAuthenticate authenticate) {

        Object[] response;
        int result = 0;

        AuthenticationType authenticateResult = (AuthenticationType) nSystemAuthenticate(instancePointer, type);

        if (authenticateResult.getStatus() != 0) {
            return authenticateResult.getStatus();
        }

        if (type == LCDefinitions.AUTHENTICATE_CONTROL_KEYS
            || type == LCDefinitions.AUTHENTICATE_CONTROL_KEYS_PERSISTENT) {
            response = authenticate.getControlKeys();
            result = nSystemGetControlKeys(instancePointer, response);
        } else if (type == LCDefinitions.AUTHENTICATE_CERTIFICATE
            || type == LCDefinitions.AUTHENTICATE_CERTIFICATE_PERSISTENT) {
            byte[] challengeResponse = authenticate.getChallengeResponse(authenticateResult.getChallengeData());
            if (challengeResponse == null) {
                challengeResponse = new byte[0];
            }
            result = nSystemAuthenticationChallenge(instancePointer, challengeResponse.length, challengeResponse);
        }

        return result;
    }

    /**
     * @param instancePointer
     *            instance id
     * @param type
     *            of flash report
     * @return result
     */
    protected Object systemCollectData(long instancePointer, int type) {
        return nSystemCollectData(instancePointer, type);
    }

    /**
     * 
     * @param instancePointer
     *            instance pointer id
     * @param iType
     *            iType
     * @param iLength
     *            iLength
     * @param puarbData
     *            puarbData
     * @return result
     */
    protected int securityInitArbTable(long instancePointer, int iType, int iLength, byte[] puarbData) {
        return nSecurityInitArbTable(instancePointer, iType, iLength, puarbData);
    }

    /**
     * @param instancePointer
     *            TBD
     * @param length
     *            TBD
     * @param type
     *            TBD
     * @param path
     *            TBD
     * @param useBulk
     *            TBD
     * @param l
     *            TBD
     * @return TBD
     */
    protected int flashProcessFile(long instancePointer, long length, String type, String path, boolean useBulk,
        Observer l) {
        int result = nFlashProcessFile(instancePointer, path, type, useBulk);
        return result;
    }

    /**
     * @param instancePointer
     *            instance id
     * @return result
     */
    protected Object flashListDevices(long instancePointer) {

        return nFlashListDevices(instancePointer);

    }

    /**
     * DEBUG
     * 
     * @param instancePointer
     *            TBD
     * @return TBD
     */
    // protected Object dflashListDevices(long instancePointer) {
    //
    // return createListDevices();
    // }

    /**
     * @param instancePointer
     *            instance id
     * @param sourcePath
     *            source of dump
     * @param start
     *            start address of dump
     * @param length
     *            end address of dump
     * @param destPath
     *            destination path
     * @param useBulk
     *            if true the destination is on PC otherwise the ME
     * @param extraDumpParameters
     *            If true no redundant data will be dumped
     * @return result
     */
    protected int flashDumpArea(long instancePointer, String sourcePath, long start, long length, String destPath,
        int extraDumpParameters, boolean useBulk) {

        int result = nFlashDumpArea(instancePointer, sourcePath, start, length, destPath, extraDumpParameters, useBulk);

        return result;
    }

    /**
     * @param instancePonter
     *            instance id
     * @param sourcePath
     *            source to erase
     * @param start
     *            start address
     * @param length
     *            end address
     * @return result
     */
    protected int flashEraseArea(long instancePonter, String sourcePath, long start, long length) {
        return nFlashEraseArea(instancePonter, sourcePath, start, length);
    }

    /**
     * method sends command set_enhanced_area
     * 
     * @param instancePonter
     *            instance id
     * @param sourcePath
     *            path to ME storage
     * @param start
     *            start address
     * @param length
     *            end address
     * @return result
     */
    protected int flashSetEnhancedArea(long instancePonter, String sourcePath, long start, long length) {
        return nFlashSetEnhancedArea(instancePonter, sourcePath, start, length);
    }

    /**
     * method sends command select_loader_options
     * 
     * @param instancePonter
     *            instance id
     * @param uiProperty
     *            uiProperty
     * @param uiValue
     *            uiValue
     * @return result
     */
    protected int flashSelectLoaderOptions(long instancePonter, long uiProperty, long uiValue) {
        return nFlashSelectLoaderOptions(instancePonter, uiProperty, uiValue);
    }

    // ----------------------FILE SYSTEM----------------------------------

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            device path
     * @return result
     */
    protected Object fileSystemVolumeProperties(long instancePointer, String devicePath) {
        return nFSVolumeProperties(instancePointer, devicePath);

    }

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            device to format
     * @return result
     */
    protected int fileSystemFormatVolume(long instancePointer, String devicePath) {
        return nFSFormatVolume(instancePointer, devicePath);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param fileSystemPath
     *            file system path
     * @return result
     */
    protected Object fileSystemListDirectory(long instancePointer, String fileSystemPath) {
        return nFSListDirectory(instancePointer, fileSystemPath);

    }

    /**
     * @param instancePointer
     *            instance id
     * @param sourcePath
     *            source
     * @param destPath
     *            destination
     * @return result
     */
    protected int fileSystemMoveFile(long instancePointer, String sourcePath, String destPath) {
        return nFSMoveFile(instancePointer, sourcePath, destPath);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param fileSystemFile
     *            file to delete
     * @return result
     */
    protected int fileSystemDeleteFile(long instancePointer, String fileSystemFile) {
        return nFSDeleteFile(instancePointer, fileSystemFile);
    }

    /**
     * if (srcUseBulk == true && destUseBulk == false) copy file from PC to ME
     * 
     * if(srcUseBulk == false && destUseBulk == true) copy file from ME to PC
     * 
     * if(srcUseBulk == false && destUseBulk == false) copy file internally in ME
     * 
     * 
     * @param instancePointer
     *            instance id
     * @param sourcePath
     *            source
     * @param destPath
     *            destination
     * @param srcUseBulk
     *            if use bulk
     * @param destUseBulk
     *            if use bulk
     * @return result
     */
    protected int fileSystemCopyFile(long instancePointer, String sourcePath, String destPath, boolean srcUseBulk,
        boolean destUseBulk) {
        int result = nFSCopyFile(instancePointer, sourcePath, destPath, srcUseBulk, destUseBulk);
        return result;
    }

    /**
     * @param instancePointer
     *            instance id
     * @param targetPath
     *            target path for new directory
     * @return result
     */
    protected int fileSystemCreateDirectory(long instancePointer, String targetPath) {
        return nFSCreateDirectory(instancePointer, targetPath);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param targetPath
     *            target for properties
     * @return result
     */
    protected Object fileSystemProperties(long instancePointer, String targetPath) {
        return nFSProperties(instancePointer, targetPath);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param targetPath
     *            target to change access
     * @param access
     *            type of access
     * @return result
     */
    protected int fileSystemChangeAccess(long instancePointer, String targetPath, int access) {
        return nFSChangeAccess(instancePointer, targetPath, access);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param targetPath
     *            path to write manifest data
     * @param sourcePath
     *            path for load modules
     * @return result for this command
     */
    protected int fileSystemReadLoadModuleManifests(long instancePointer, String targetPath, String sourcePath) {
        return nFSReadLoadModuleManifests(instancePointer, targetPath, sourcePath);
    }

    // -----------------------------------
    // OTP---------------------------------------

    /**
     * @param instancePointer
     *            instance id
     * @param id
     *            OTP id
     * @param start
     *            start address of OTP
     * @param length
     *            end address of OTP
     * @return OTPReadBitsType
     */
    protected Object otpReadBits(long instancePointer, int id, int start, int length) {
        return nOTPReadBits(instancePointer, id, start, length);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param id
     *            OTP id
     * @param start
     *            start address of OTP
     * @param length
     *            end address of OTP
     * @param bits
     *            the bits to write
     * @return result
     */
    protected int otpSetBits(long instancePointer, int id, int start, int length, byte[] bits) {
        return nOTPSetBits(instancePointer, id, start, length, bits);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param id
     *            otp id
     * @return result
     */
    protected int otpWriteAndLock(long instancePointer, int id) {
        return nOTPWriteAndLockBits(instancePointer, id);
    }

    // -------------------- Parameter
    // Storage----------------------------------------

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            String id of storage
     * @param unitId
     *            id of unit
     * @return PSReadGlobalDataUnitType
     */
    protected Object parameterStorageReadGlobalDataUnit(long instancePointer, String devicePath, int unitId) {
        return nPSReadGlobalDataUnit(instancePointer, devicePath, unitId);
    }

    /**
     * @param instancePoiner
     *            TBD
     * @param devicePath
     *            TBD
     * @param unitId
     *            TBD
     * 
     * @param data
     *            TBD
     * @return TBD
     */
    protected int parameterStorageWriteGlobalDataUnit(long instancePoiner, String devicePath, int unitId, byte[] data) {
        return nPSWriteGlobalDataUnit(instancePoiner, devicePath, unitId, data, data.length);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            String id of storage
     * @param destPath
     *            destination to put GD data
     * @param useBulk
     *            if use bulk
     * @param l
     *            listener for progress events
     * @return result
     */
    protected int parameterStorageReadGlobalDataSet(long instancePointer, String devicePath, String destPath,
        boolean useBulk, Observer l) {

        String tempPath = LCGDFSParser.getTempGDFSPath();

        int result = nPSReadGlobalDataSet(instancePointer, devicePath, tempPath, useBulk);
        if (result == 0) {
            File temp = new File(tempPath);
            temp.deleteOnExit();
            LCGDFSParser.convertToGDF(temp, new File(destPath));
        }

        return result;
    }

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            String id of storage
     * @param dataLength
     *            the length of data
     * @param sourcePath
     *            source of data
     * @param useBulk
     *            if use bulk
     * @param l
     *            listener for progress event
     * @return result
     */
    protected int parameterStorageWriteGlobalDataSet(long instancePointer, String devicePath, long dataLength,
        String sourcePath, boolean useBulk, Observer l) {
        File binFile = LCGDFSParser.convertToBinary(new File(sourcePath));
        return nPSWriteGlobalDataSet(instancePointer, devicePath, binFile.length(), binFile.getPath(), useBulk);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param devicePath
     *            String id of storage
     * @return result
     */
    protected int parameterStorageEraseGlobalDataSet(long instancePointer, String devicePath) {
        return nPSEraseGlobalDataSet(instancePointer, devicePath);
    }

    // -------------------------- Security-----------------------------

    /**
     * @param instancePointer
     *            instance id
     * @param domain
     *            domain to set
     * @return result
     */
    protected int securitySetDomain(long instancePointer, int domain) {
        return nSecuritySetDomain(instancePointer, domain);
    }

    /**
     * @param instancePointer
     *            instance id
     * @return DomainType
     */
    protected Object securityGetDomain(long instancePointer) {
        return nSecurityGetDomain(instancePointer);
    }

    /**
     * @param instancePointer
     *            TBD
     * @param propertyId
     *            TBD
     * @return TBD
     */
    protected Object securityGetProperties(long instancePointer, int propertyId) {
        return nSecurityGetProperties(instancePointer, propertyId);
    }

    /**
     * @param instancePointer
     *            TBD
     * @param propertyId
     *            TBD
     * @param dataLength
     *            TBD
     * @param buffer
     *            TBD
     * @return TBD
     */
    protected int securitySetProperties(long instancePointer, int propertyId, int dataLength, byte[] buffer) {
        return nSecuritySetProperties(instancePointer, propertyId, dataLength, buffer);
    }

    /**
     * @param instancePointer
     *            instance id
     * @return result
     */
    protected int securityBindProperties(long instancePointer) {
        return nSecurityBindProperties(instancePointer);
    }

    /**
     * @param instancePointer
     *            instance id
     * @param deviceId
     *            device id
     * @param commercial
     *            is commercial
     * @return result
     */
    protected int securityWriteRpmbKey(long instancePointer, int deviceId, boolean commercial) {
        return nSecurityWriteRpmbKey(instancePointer, deviceId, commercial);
    }

    /**
     * @param instancePointer
     *            TBD
     * @param sourcePath
     *            TBD
     * @param destination
     *            TBD
     * @param useBulk
     *            TBD
     * @return TBD
     */
    protected int otpStoreSecurityObject(long instancePointer, String sourcePath, int destination, boolean useBulk) {
        return nOTPStoreSecurityObject(instancePointer, sourcePath, destination, useBulk);
    }

    // /**
    // *
    // * @param instancePointer
    // * Instance Id
    // * @return result
    // */
    // protected Object resetInitRequest(long instancePointer) {
    // return nResetInitRequest(instancePointer);
    // }
    //
    // /**
    // * @param instancePointer
    // * TBD
    // * @param buffer
    // * TBD
    // * @return TBD
    // */
    // protected int resetUpdateRequest(long instancePointer, byte[] buffer) {
    // return nResetUpdateRequest(instancePointer, buffer.length, buffer);
    // }

    /**
     * DEBUG returns a dummy SupportedCommandsType
     * 
     * @return supportedCommandType
     */
    // private SupportedCommandsType createSupportedCommands() {
    //
    // SupportedCommandsType list = new SupportedCommandsType(0, 34);
    //
    // list.addSupportedCommand(1, 1, 1);
    // list.addSupportedCommand(1, 2, 1);
    // list.addSupportedCommand(1, 3, 1);
    // list.addSupportedCommand(1, 4, 1);
    // list.addSupportedCommand(1, 5, 1);
    // list.addSupportedCommand(1, 6, 1);
    // list.addSupportedCommand(1, 7, 1);
    // list.addSupportedCommand(1, 8, 1);
    // list.addSupportedCommand(1, 9, 1);
    // list.addSupportedCommand(2, 1, 1);
    //
    // list.addSupportedCommand(2, 2, 1);
    // list.addSupportedCommand(2, 3, 1);
    // list.addSupportedCommand(2, 4, 1);
    // list.addSupportedCommand(3, 1, 1);
    // list.addSupportedCommand(3, 2, 1);
    // list.addSupportedCommand(3, 3, 1);
    // list.addSupportedCommand(3, 4, 1);
    // list.addSupportedCommand(3, 5, 1);
    // list.addSupportedCommand(3, 6, 1);
    // list.addSupportedCommand(3, 7, 1);
    //
    // list.addSupportedCommand(3, 8, 1);
    // list.addSupportedCommand(3, 9, 1);
    // list.addSupportedCommand(3, 10, 1);
    // list.addSupportedCommand(3, 11, 1);
    // list.addSupportedCommand(4, 1, 1);
    // list.addSupportedCommand(4, 2, 1);
    // list.addSupportedCommand(5, 1, 1);
    // list.addSupportedCommand(5, 2, 1);
    // list.addSupportedCommand(5, 3, 1);
    // list.addSupportedCommand(5, 4, 1);
    // list.addSupportedCommand(5, 5, 1);
    // list.addSupportedCommand(5, 6, 1);
    // list.addSupportedCommand(5, 7, 1);
    //
    // list.addSupportedCommand(6, 2, 1);
    // list.addSupportedCommand(6, 3, 1);
    // list.addSupportedCommand(6, 4, 1);
    // list.addSupportedCommand(6, 5, 1);
    //
    // return list;
    // }

    /**
     * DEBUG return a dummy listDeviceType
     * 
     * @return TDB
     * @throws Exception
     */
    // private ListDevicesType createListDevices() {
    //
    // ListDevicesType list = new ListDevicesType(0, 1);
    //
    // list.addDevice("/flash0/GDFS", "/flash0/GDFS".length(), "GD", 2, 4096, 0, (Long.MAX_VALUE /
    // 4096) * 4096);
    // list.addDevice("/flash0/TA_GD_Backup", "/flash0/TA_GD_Backup".length(), "GD", 2, 4096, 0,
    // (Long.MAX_VALUE / 4096) * 4096);
    // list.addDevice("/flash0/TA_Misc", "/flash0/TA_Misc".length(), "GD", 2, 4096, 0,
    // (Long.MAX_VALUE / 4096) * 4096);
    //
    // return list;
    //
    // }

    private LoaderCommunicationInterface() throws Exception {
        String lcdPath = SystemProperties.getProperty(SystemProperties.BACKEND_LCD_FILE);
        if (SystemProperties.getProperty(SystemProperties.BACKEND_NATIVE_PATH) == null) {
            logger.info("Variable " + SystemProperties.BACKEND_NATIVE_PATH + " must be set in VM arguments");
            throw new Exception();
        }
        if ((lcdPath == null) || "".equals(lcdPath)) {
            lcdPath =
                SystemProperties.getProperty(SystemProperties.BACKEND_NATIVE_PATH) + File.separator
                    + getNativeLCDLibName();
        }
        String lcmPath = SystemProperties.getProperty(SystemProperties.BACKEND_LCM_FILE);
        if ((lcmPath == null) || "".equals(lcmPath)) {
            lcmPath =
                SystemProperties.getProperty(SystemProperties.BACKEND_NATIVE_PATH) + File.separator
                    + getNativeLCMLibName();
        }

        logger.debug("Using LCD " + lcdPath);
        logger.debug("Using LCM " + lcmPath);

        setLCDPath(lcdPath);
        setLCMPath(lcmPath);
    }

    /**
     * Checks if the system is 64bit and if the user is using 64bit LCD driver
     * @return true or false
     */
    public boolean check64BitLCDDriversExists(){
        try {
            String libname = "";
            if (isMac()) {
                libname = LCD_DYLIB_NAME_X64;
            } else if (isUnix()) {
                libname = LCD_SO_NAME_X64;
            } else if (isWindows()) {
                libname = LCD_DLL_NAME_X64;
            }
            String lcdPath = SystemProperties.getProperty(SystemProperties.BACKEND_NATIVE_PATH) + File.separator + libname;
            if(new File(lcdPath).exists()){
                return true;
            }
        } catch (Exception e) {
            logger.error("ERROR while checking if LCD/LCM drivers exist: " + e.getMessage());
        }
        return false;
    }

    /**
     * Checks if the system is 64bit and if the user is using 64bit LCM driver
     * @return true or false
     */
    public boolean check64BitLCMDriversExists(){
        try {
            String libname = "";
            if (isMac()) {
                libname = LCM_DYLIB_NAME_X64;
            } else if (isUnix()) {
                libname = LCM_SO_NAME_X64;
            } else if (isWindows()) {
                libname = LCM_DLL_NAME_X64;
            }
            String lcdPath = SystemProperties.getProperty(SystemProperties.BACKEND_NATIVE_PATH) + File.separator + libname;
            if(new File(lcdPath).exists()){
                return true;
            }
        } catch (Exception e) {
            logger.error("ERROR while checking if LCD/LCM drivers exist: " + e.getMessage());
        }
        return false;
    }

    /**
     * start read/write threads
     * 
     * @param id
     *            tbd
     */
    public void startThreads(long id) {
        ReaderThread readerThread = new ReaderThread();
        readerThread.setPriority(Thread.MAX_PRIORITY);
        WriterThread writerThread = new WriterThread();
        writerThread.setPriority(Thread.MAX_PRIORITY);
        readerThread.setName("LC reader - " + id);
        writerThread.setName("LC writer - " + id);
        readerThread.start();
        writerThread.start();
        instanceReaderThreadMap.put(id, readerThread);
        instanceWriterThreadMap.put(id, writerThread);

    }

    /**
     * @return the instance
     */
    public static LoaderCommunicationInterface getInstance() {

        if (interfaceInstance == null) {

            try {
                String libNativeName = getNativeLibName();
                System.loadLibrary(libNativeName);
                logger.debug("Loader communication library loaded: " + libNativeName + getLibraryExtension());

                interfaceInstance = new LoaderCommunicationInterface();
            } catch (Exception e) {
                logger.debug("failed to load library: " + e.getMessage());
            }
        }

        return interfaceInstance;
    }

    /**
     * This method is use to get full name of native LC JNI library
     * 
     * @return full name
     * @throws Exception
     */
    private static String getNativeLibName() throws Exception {
        if (isUnix() || isWindows() || isMac()) {
            if (is64BitVM()) {
                return LIB_NAME_X64;
            } else {
                return LIB_NAME;
            }
        } else {
            throw new Exception("Could not detect system type (system must be Unix, Windows or Mac OS)");
        }
    }

    private static String getNativeLCMLibName() throws Exception {
        if (isMac()) {
            if (is64BitVM()) {
                return LCM_DYLIB_NAME_X64;
            } else {
                throw new Exception("The 32 bit Mac OS does not supported.");
            }
        } else if (isUnix()) {
            if (is64BitVM()) {
                return LCM_SO_NAME_X64;
            } else {
                return LCM_SO_NAME;
            }
        } else if (isWindows()) {
            if (is64BitVM()) {
                return LCM_DLL_NAME_X64;
            } else {
                return LCM_DLL_NAME;
            }
        } else {
            throw new Exception("Could not detect system type (system must be Unix, Windows or Mac OS)");
        }
    }

    private static String getNativeLCDLibName() throws Exception {
        if (isMac()) {
            if (is64BitVM()) {
                return LCD_DYLIB_NAME_X64;
            } else {
                throw new Exception("The 32 bit Mac OS does not supported.");
            }
        } else if (isUnix()) {
            if (is64BitVM()) {
                return LCD_SO_NAME_X64;
            } else {
                return LCD_SO_NAME;
            }
        } else if (isWindows()) {
            if (is64BitVM()) {
                return LCD_DLL_NAME_X64;
            } else {
                return LCD_DLL_NAME;
            }
        } else {
            throw new Exception("Could not detect system type (system must be Unix, Windows or Mac OS)");
        }
    }

    /**
     * @param instancePointer
     *            TBD
     * @param cmdAck
     *            TBD
     * @param bulkPacketReceived
     *            TBD
     * @param bulkSessionEnd
     *            TBD
     * @param bulkDataReceived
     *            TBD
     * @param rto
     *            TBD
     * @param sto
     *            TBD
     * @return TBD
     */
    protected int setTimeouts(long instancePointer, int cmdAck, int bulkPacketReceived, int bulkSessionEnd,
        int bulkDataReceived, int rto, int sto) {
        return nSetTimeouts(instancePointer, cmdAck, bulkPacketReceived, bulkSessionEnd, bulkDataReceived);
    }

    /**
     * @param port
     *            TBD
     * @return TBD
     */
    protected boolean isAlreadyInitialized(IPort port) {

        if (port == null) {
            return false;
        }

        for (LoaderCommunicationContext c : ctxList.values()) {
            if (c.getPort() == null) {
                continue;
            }
            if (((IPort) c.getPort()).getPortName().equals(port.getPortName())) {
                return true;
            }
        }
        return false;
    }

    /**
     * @param instancePointer
     *            TBD
     * @return TBD
     */
    protected LoaderCommunicationContext getContext(long instancePointer) {
        return ctxList.get(instancePointer);
    }

    /**
     * @param eventListener
     *            Handler of progress information in task.
     * @param instancePointer
     *            Instance pointer for which registered listener is.
     */
    protected static void addEventListener(long instancePointer, ILoaderCommunicationListener eventListener) {
        listeners.put(instancePointer, eventListener);
    }

    /**
     * @param eventListener
     *            TBD
     */
    protected void removeEventListener(ILoaderCommunicationListener eventListener) {
        listeners.remove(eventListener);
    }

    /**
     * 
     * @param lcCtx
     *            the context to init
     * @return status of the command
     * @throws Exception
     *             exception
     */
    public long init(LoaderCommunicationContext lcCtx) throws Exception {

        // break here when debugging in JNI and LCD

        long id = interfaceInstance.initialize();

        startThreads(id);

        registerInstance(id, lcCtx);

        return id; // TODO: error handling
    }

    // private void setTimeouts(int id) {
    //
    // int cmdAck = (int)
    // LCTimeouts.getInstance().getTimeouts(LCDefinitions.COMMAND_ACK);
    // int bulkCDMReceived = (int)
    // LCTimeouts.getInstance().getTimeouts(LCDefinitions.BULK_CMD_RECEIVED);
    // int bulkSessionEnd = (int)
    // LCTimeouts.getInstance().getTimeouts(LCDefinitions.BULK_SESSION_END);
    // int bulkDataReceived = (int)
    // LCTimeouts.getInstance().getTimeouts(LCDefinitions.BULK_DATA_RECEIVED);
    // setTimeouts(id, cmdAck, bulkCDMReceived, bulkSessionEnd,
    // bulkDataReceived, Integer.MAX_VALUE,
    // Integer.MAX_VALUE);
    // }

    /**
     * @param instanceId
     *            Instance identifier
     * @return Result
     */
    public static boolean deInit(long instanceId) {

        unregisterInstance(instanceId);

        try {
            Thread.sleep(500); // hot fix, otherwise JVM crashes

            interfaceInstance.deInitialize(instanceId);
        } catch (Exception e) {
            logger.debug("failed to deinitialize intstance:" + instanceId);
        }

        return true;
    }

    /**
     * @param messageLen
     *            TBD
     * @param message
     *            TBD
     */
    public static synchronized void message(int messageLen, String message) {

        if (useCommunicationPrintouts) {
            logger.debug("JNI message: " + message.substring(0, messageLen));
        }
    }

    /**
     * Method which used for passing JNI log message to j4Log.
     * 
     * @param level
     *            number which correspond with j4log level value
     * @param message
     *            log message
     */
    protected static synchronized void logMessage(int level, String message) {
        logger.log(Level.toLevel(level * 1000), message);
    }

    /**
     * this is the static method used when getting a read request by the LCM
     * 
     * @param length
     *            length to read
     * @param dataPointer
     *            pointer to data to fill
     * @param instancePointer
     *            instance pointer
     */
    public static void read(int length, long dataPointer, long instancePointer) {

        logger.debug("Enqueue read for instance: " + instancePointer + ", length: " + length);

        while (!ctxList.containsKey(instancePointer)) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                logger.warn("Thread interrupted");
            }
        }

        instanceReaderThreadMap.get(instancePointer).addToQueue(
            new ReaderTask(length, dataPointer, instancePointer, ctxList.get(instancePointer).port));
    }

    /**
     * this is the static method used when getting a write request by the LCM, this will call a
     * method when finished to free the lock for the write request
     * 
     * @param data
     *            the data to send
     * @param length
     *            the size to write
     * @param dataPointer
     *            pointer to data to be written
     * @param instancePointer
     *            the instance pointer
     */
    public static void write(byte[] data, int length, long dataPointer, long instancePointer) {

        logger.debug("Enqueue write for instance: " + instancePointer + ", length: " + length);

        Task t = new WriterTask(length, dataPointer, instancePointer, ctxList.get(instancePointer).getPort());
        t.setData(data);

        instanceWriterThreadMap.get(instancePointer).addToQueue(t);

    }

    /**
     * @param instancePointer
     *            the lcm instance
     * @param totalBytes
     *            total bytes to transfer
     * @param transferredBytes
     *            total transferred bytes
     */
    public static synchronized void progress(long instancePointer, long totalBytes, long transferredBytes) {
        ILoaderCommunicationListener listener = listeners.get(instancePointer);
        if (listener != null) {
            listener.loaderCommunicationProgress(totalBytes, transferredBytes);
        }
    }

    /**
     * a list to keep track of the streams and the instances of the lcm
     * 
     * @param instanceId
     * @param lcm
     */
    private static void registerInstance(long instancePointer, LoaderCommunicationContext lcCtx) {
        logger.debug("Registered instance: " + instancePointer);
        ctxList.put(instancePointer, lcCtx);
    }

    private static void unregisterInstance(long instancePointer) {
        instanceReaderThreadMap.get(instancePointer).terminate();
        ctxList.remove(instancePointer);
    }

    static Object cancelOperation;

    /**
     * cancel the current action for whatever reason, the mds states that there should be a status
     * parameter in the callback function but in lcm code the function pointer is: errorCode (*
     * Cancel)() <- this callback function should have the following: status of current operation to
     * be able to resume the operation. I dont really know what this is used for at this moment so
     * it will reflect the implementation of the method
     * 
     * @param id
     *            tbd
     */
    public static void cancel(long id) {

        // is not used
        // unregisterInstance(id);
    }

    /**
     * @param port
     *            tbd
     * @param message
     *            tbd
     */
    protected static void notifyLoaderCommunicationError(IPort port, String message) {
        ServiceReference[] references = null;

        try {
            references =
                Activator.getBundleContext().getServiceReferences(ILoaderCommunicationListener.class.getName(), null);
            if (references != null) {
                for (ServiceReference reference : references) {
                    ILoaderCommunicationListener communicationListener =
                        (ILoaderCommunicationListener) Activator.getBundleContext().getService(reference);
                    communicationListener.loaderCommunicationError(port, message);
                    Activator.getBundleContext().ungetService(reference);
                }
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

    }

}
