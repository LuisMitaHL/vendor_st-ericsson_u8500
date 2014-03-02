/**
 *
 */
package com.stericsson.sdk.equipment.io.ip.dns;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

import javax.jmdns.JmDNS;
import javax.jmdns.ServiceInfo;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortProvider;

/**
 * Class to handle mDNS broadcast listeners.
 * 
 * @author eolabor
 */
public class DNSBroadcastHandler implements IConfigurationServiceListener {

    static final Logger log = Logger.getLogger(DNSBroadcastHandler.class);

    private JmDNS jmDNS;

    private DNSBroadcastListener broadcastListener;

    private NetworkInterfacePoll interfacePollTimer;

    /**
     * Constructor for DNSBroadcastHandler
     * 
     */
    public DNSBroadcastHandler() {
        interfacePollTimer = new NetworkInterfacePoll();
    }

    /**
     * Starts the polling of NetworkIntefaces.
     */
    public void startInterfacePoll() {
        interfacePollTimer.start(new Timer("NetworkInterfacePoll.timer", true));
        log.info("Network interface listener added");
    }

    private void create(InetAddress inetAdress) {
        try {
            if (jmDNS != null) {
                log
                    .error("Trying to invoke the JmDNS create method but the JmDNS is already created and has not been closed!");
                throw new RuntimeException();
            }
            jmDNS = JmDNS.create(inetAdress, inetAdress.getHostAddress());
            broadcastListener = new DNSBroadcastListener();
            jmDNS.addServiceListener(DNSConfiguration.SERVICE_TYPE, broadcastListener);
        } catch (IOException e) {
            log.error("Not able to create mDNS listener on interface " + inetAdress.getHostAddress());
        }
    }

    /**
     * Method to add services available during startup of backend
     */
    public void addAvailableServices() {
        Thread listThread = new Thread("AvailableBroadcasts") {
            public void run() {
                ServiceInfo[] infos = jmDNS.list(DNSConfiguration.SERVICE_TYPE);
                for (int index = 0; index < infos.length; index++) {
                    if (infos[index].getName().equalsIgnoreCase(DNSConfiguration.AVAILABLE_ME)) {
                        FtpPortHandler.getInstance().addDevice(infos[index].getHostAddress());
                    } else if (infos[index].getName().equalsIgnoreCase(DNSConfiguration.DUMPING_ME)) {
                        FtpPortHandler.getInstance().dumpingDevice(infos[index].getHostAddress());
                    }
                }
            }
        };
        listThread.start();
    }

    /**
     * Close the JmDNS object
     * 
     */
    public void close() {
        if (jmDNS != null) {
            jmDNS.close();
            jmDNS = null;
        }
    }

    /**
     * Used for unit tests only
     * 
     * @param testJmDNS
     *            test implementation of JmDNS
     */
    public void setJmDNS(JmDNS testJmDNS) {
        jmDNS = testJmDNS;
    }

    /**
     * Used for unit tests only
     * 
     * @return jmDNS object
     */
    public JmDNS getJmDNS() {
        return jmDNS;
    }

    /**
     * Listener for configuration change
     * 
     * @param service
     *            Configuration service
     * @param oldRecord
     *            Old Configuration Record
     * @param newRecord
     *            New Configuration Record
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {
        if (newRecord != null) {
            if ((oldRecord == null) && (newRecord.getName().equals(ConfigurationOption.ACTIVE_PROFILE))
                && (newRecord.getValue(ConfigurationOption.PROFILE_ALIAS) != null)) {
                // Init of auto settings for core dumps
                FtpPortHandler portHandler = FtpPortHandler.getInstance();
                portHandler.setAutoDownload(Boolean.getBoolean(SystemProperties.BACKEND_AUTO_DOWNLOAD_ENABLED));
                portHandler.setAutoDelete(Boolean.getBoolean(SystemProperties.BACKEND_AUTO_DELETE_ENABLED));
                portHandler.setAutoDownloadDirectory(System.getProperty(SystemProperties.BACKEND_DOWNLOAD_PATH));

                if (jmDNS != null) {
                    addAvailableServices();
                }
            }
        }

    }

    /**
     * Polls network interfaces for changes. If Ethernet over usb interface is detected JmDNS will
     * be set up to listen on found device. If Ethernet over usb interface is removed Jmdns will be
     * closed.
     * 
     * @author eolabor
     * 
     */
    private class NetworkInterfacePoll extends TimerTask {

        private boolean usbEthConnected = false;

        private boolean backendInTestMode = EnvironmentProperties.isRunningInTestMode();

        public NetworkInterfacePoll() {
            super();
        }

        /**
         * Starts polling
         * 
         * @param timer
         *            Timer to use
         */
        public void start(Timer timer) {
            timer.schedule(this, 0, DNSConfiguration.POLL_INTERVAL);
        }

        @Override
        public void run() {
            try {
                Enumeration<NetworkInterface> availableInterfaces = NetworkInterface.getNetworkInterfaces();
                if (availableInterfaces != null) {
                    checkForUsbEthInterface(availableInterfaces);
                } else {
                    throw new SocketException();
                }
            } catch (SocketException e) {
                log.error("Unable to get available network interfaces");
            }

        }

        private void checkForUsbEthInterface(Enumeration<NetworkInterface> networkInterfaces) {
            boolean interfaceFound = false;
            while (networkInterfaces.hasMoreElements() && !interfaceFound) {
                NetworkInterface netInterface = networkInterfaces.nextElement();
                String name = netInterface.getDisplayName().toLowerCase(new Locale("en", "US"));
                if (name.contains(DNSConfiguration.USB_ETH_NAME_WINDOWS)
                    || name.contains(DNSConfiguration.USB_ETH_NAME_LINUX) || backendInTestMode) {
                    interfaceFound = true;
                    if (!usbEthConnected || jmDNS == null) {
                        usbEthConnected = true;
                        startJmdns(netInterface.getInetAddresses());
                    }
                }
            }
            if (!interfaceFound && usbEthConnected) {
                closeJmdns();
            }
        }

        private void closeJmdns() {
            close();
            usbEthConnected = false;
            removeDevice();
            log.info("USB network interface removed. mDNS broadcast listener will be closed.");
        }

        private void removeDevice() {
            FtpPortProvider portProvider = null;
            try {
                portProvider = FtpPortHandler.getInstance().getIpDevices().get(broadcastListener.getTargetIP());
            } catch (Exception e) {
                log.info("USB network device removed.");
            }
            if (portProvider != null) {
                portProvider.release();
            }
        }

        private void startJmdns(Enumeration<InetAddress> inetAddresses) {
            while (inetAddresses.hasMoreElements()) {
                InetAddress address = inetAddresses.nextElement();
                if (address instanceof Inet4Address) {
                    create(address);
                    log.info("USB network interface found. Start to listen for mDNS broadcast on interface "
                        + address.getHostAddress());
                    if (backendInTestMode) {
                        backendInTestMode = false;
                    }
                }
            }
        }
    }

}
