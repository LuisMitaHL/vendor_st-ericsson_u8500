package com.stericsson.sdk.equipment.io.ip.internal;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * Class to keep track of added and removed "warm" equipments(ports). FtpPortHandler also keeps
 * track of user specific setting concerning dump handling.
 * 
 * @author eolabor
 */
public final class FtpPortHandler {

    private static FtpPortHandler instance;

    private static final String DEFAULT_DOWNLOAD_DIRECTORY = "AutoDownloadedDumps";

    static final Logger log = Logger.getLogger(FtpPortHandler.class);

    private Map<String, FtpPortProvider> ipDevices = new HashMap<String, FtpPortProvider>();

    private boolean isAutoDownload = false;

    private boolean isAutoDelete = false;

    private String autoDownloadDirectory;

    /**
     * @return Retrieve the only instance of this singleton class.
     */
    public static FtpPortHandler getInstance() {
        if (instance == null) {
            try {
                instance = new FtpPortHandler();
            } catch (Exception e) {
                log.error(e.getMessage());
            }
        }
        return instance;
    }

    /**
     * Creates a new device (Port). Also adds the device to internal Map
     * 
     * @param identifier
     *            identifier containing information about the DNS broadcast IP address.
     */
    public void addDevice(String identifier) {
        try {
            String ipAdress = identifier;
            if (!ipDevices.containsKey(ipAdress)) {
                FtpPortProvider ipDevice = createDevice(ipAdress);
                ipDevice.trigger();
                ipDevices.put(ipAdress, ipDevice);
            } else {
                log.info("Device already connected");
            }
        } catch (PortException e) {
            log.error(e.getMessage());
        }
    }

    private FtpPortProvider createDevice(String ipAdress) {
        FtpPortProvider ipDevice = new FtpPortProvider(ipAdress);
        return ipDevice;
    }

    /**
     * Called when dump broadcast received from target. Sets the dumping flag on device. If device
     * does not exist it will be created before setting the dump flag.
     * 
     * @param identifier
     *            identifier containing information about the DNS broadcast IP address.
     */
    public void dumpingDevice(String identifier) {
        FtpPortProvider ipDevice = getFtpPortProvider(identifier);
        if (ipDevice != null) {
            ipDevice.dumping(true);
        } else {
            addDevice(identifier);
            FtpPortProvider ftpPortProvider = getFtpPortProvider(identifier);
            if (ftpPortProvider != null) {
                getFtpPortProvider(identifier).dumping(true);
            }
        }
    }

    /**
     * Removes/releases the device (Port)
     * 
     * @param event
     *            event containing information about the DNS broadcast.
     */
    public void removeDevice(ServiceEvent event) {
        if (!ipDevices.isEmpty()) {
            String deviceKey = findRemovedDevice(event);
            FtpPortProvider ipDevice = ipDevices.get(deviceKey);
            if (ipDevice != null) {
                ipDevice.release();
                ipDevices.remove(ipDevice.getIdentifier());
            }
        } else {
            log.info("No device connected");
        }
    }

    /**
     * Method to return port provider connected to specified identifier
     * 
     * @param identifier
     *            IP address of port
     * @return Port provider of type FtpPortProvider or null if identifier not found.
     */
    public FtpPortProvider getFtpPortProvider(String identifier) {
        return ipDevices.get(identifier);
    }

    /**
     * Compares the current available broadcasts with the internal Map.
     * 
     * @param event
     *            event containing information about the DNS broadcast.
     * @return IP address of removed device
     */
    private String findRemovedDevice(ServiceEvent event) {
        ServiceInfo[] infos = event.getDNS().list(DNSConfiguration.SERVICE_TYPE);
        String result = null;

        Set<String> deviceList = ipDevices.keySet();
        Iterator<String> iterator = deviceList.iterator();
        while (iterator.hasNext()) {
            String currentKey = iterator.next();
            boolean itemListed = false;
            for (int i = 0; i < infos.length; i++) {
                if (currentKey.equalsIgnoreCase(infos[i].getHostAddress())) {
                    itemListed = true;
                }
            }
            if (!itemListed) {
                result = currentKey;
            }
        }
        return result;
    }

    /**
     * Method only used for tests
     * 
     * @return the ipDevices
     */
    public Map<String, FtpPortProvider> getIpDevices() {
        return ipDevices;
    }

    /**
     * Toggle backend to automatically download new dumps.
     * 
     * @param status
     *            True if enabled. False if disabled
     */
    public void setAutoDownload(boolean status) {
        isAutoDownload = status;
    }

    /**
     * Get if backend is automatically downloading new dumps.
     * 
     * @return boolean true or false
     */
    public boolean getAutoDownload() {
        return isAutoDownload;
    }

    /**
     * Toggle backend to automatically delete downloaded dumps.
     * 
     * @param status
     *            True if enabled. False if disabled
     * 
     * @return True/False. If AutoDownload is not set, then it will return false. Autodownload must
     *         be set to true to get this functionality.
     */
    public boolean setAutoDelete(boolean status) {
        if (isAutoDownload) {
            isAutoDelete = status;
            return true;
        } else {
            isAutoDelete = false;
            return false;
        }

    }

    /**
     * Get if backend is automatically delete downloaded new dumps.
     * 
     * @return boolean true or false
     */
    public boolean getAutoDelete() {
        return isAutoDelete;
    }

    /**
     * Sets the dump download path
     * 
     * @param downloadDirectory
     *            the autoDownloadDirectory to set
     */
    public void setAutoDownloadDirectory(String downloadDirectory) {
        autoDownloadDirectory = downloadDirectory;
    }

    /**
     * Gets the dump download path. If no path exists default path will be returned.
     * 
     * @return the autoDownloadDirectory
     */
    public String getAutoDownloadDirectory() {
        if (autoDownloadDirectory == null) {
            String directory = System.getProperty("user.home") + "/" + DEFAULT_DOWNLOAD_DIRECTORY;
            return new File(directory).getAbsolutePath();
            // return directory;
        }
        return autoDownloadDirectory;
    }

}
