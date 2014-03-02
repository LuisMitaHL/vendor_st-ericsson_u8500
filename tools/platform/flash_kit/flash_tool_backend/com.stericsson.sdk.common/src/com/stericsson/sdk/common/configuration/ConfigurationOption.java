package com.stericsson.sdk.common.configuration;

/**
 * @author xtomlju
 */
public final class ConfigurationOption {

    /**
     * Header for Monitor attributes
     */
    public static final String MONITOR = "Monitor";

    /***/
    public static final String SIGN_TOOL = "SignTool";

    /**
     * Determines if Backend server accept clients
     */
    public static final String MONITOR_ACCEPT_CLIENTS = "AcceptClients";

    /**
     * For setting of Autosense
     */
    public static final String MONITOR_ACCEPT_EQUIPMENTS = "AcceptEquipments";

    /**
     * Header for Profile attributes
     */
    public static final String PROFILE = "Profile";

    /**
     * Alias for profile
     */
    public static final String PROFILE_ALIAS = "Alias";

    /**
     * Name for profile
     */
    public static final String PROFILE_NAME = "Name";

    /**
     * Path for profile
     */
    public static final String PROFILE_PATH = "Path";

    /** */
    public static final String ACTIVE_PROFILE = "ActiveProfile";

    /***/
    public static final String USE_LOCAL_SIGNING = "UseLocalSigning";

    /**
     * Header for USB attributes
     */
    public static final String PORT_USB = "USB";

    /**
     * For enabling/disabling of USB
     */
    public static final String PORT_USB_ENABLED = "Enabled";

    /**
     * Header for UART attributes
     */
    public static final String PORT_UART = "UART";

    /**
     * For enabling/disabling of UART port
     */
    public static final String PORT_UART_TRIGGER = "Trigger";

    /**
     * For enabling/disabling of UART port
     */
    public static final String PORT_UART_ENABLED = "Enabled";

    /**
     * Alias for UART port
     */
    public static final String PORT_UART_ALIAS = "Alias";

    /**
     * Baudrate for UART connection
     */
    public static final String PORT_UART_BAUDRATE = "Baudrate";

    /**
     * Databits for UART connection
     */
    public static final String PORT_UART_DATABITS = "Databits";

    /**
     * Parity for UART connection
     */
    public static final String PORT_UART_PARITY = "Parity";

    /**
     * Stopbit for UART connection
     */
    public static final String PORT_UART_STOPBIT = "Stopbit";

    /** Header for Platform attributes */
    public static final String PLATFORM = "Platform";

    /** Platform filter */
    public static final String PLATFORM_FILTER = "Filter";

    /** Header for Software attributes */
    public static final String SOFTWARE = "Software";

    /** Software type */
    public static final String SOFTWARE_TYPE = "Type";

    /** Software path */
    public static final String SOFTWARE_PATH = "Path";

    /** Header for Property */
    public static final String PROPERTY = "Property";

    /** uses loader communication property name; applicable only if BootIndication != normal */
    public static final String PROPERTY_LOADER_COMMUNICATION = "UsesLoaderCommunication";

    /** boot indication property name */
    public static final String PROPERTY_BOOT_INDICATION = "BootIndication";

    /**debug trace on uart property*/
    public static final String PROPERTY_DEBUG_TRACE_ON_UART = "DebugTraceOnUART";

    /**x-loader debug trace on uart property*/
    public static final String PROPERTY_XLOADER_DEBUG_TRACE_ON_UART = "X-LoaderDebugTraceOnUART";

    /**with DMA on USB property*/
    public static final String PROPERTY_WITH_DMA_ON_USB = "WithDMAonUSB";

    /** GDFS area path property */
    public static final String PROPERTY_GDFS_AREA_PATH = "GDFSAreaPath";

    /** Trim area path property */
    public static final String PROPERTY_TRIM_AREA_PATH = "TrimAreaPath";

    /** Header for Security Property attributes */
    public static final String SECURITY_PROPERTY = "SecurityProperty";

    /** Name of security property */
    public static final String SECURITY_PROPERTY_NAME = "Name";

    /** Id of security property */
    public static final String SECURITY_PROPERTY_ID = "ID";

    /** write access for security property */
    public static final String SECURITY_PROPERTY_WRITABLE = "Write";

    /** read access for security property */
    public static final String SECURITY_PROPERTY_READABLE = "Read";

    /** UART port ID to be resolved automatically. */
    public static final String AUTO_UART_PORT_ID = "auto";

    /** header of ethernet configuration record */
    public static final String ETHERNET_PORT = "Ethernet";

    /** Enable/Disable ethernet port */
    public static final String ETHERNET_PORT_ENABLED = "enabled";

    /** Ethernet port number */
    public static final String ETHERNET_PORT_NUMBER = "port";

    /**
     * Constructor.
     */
    private ConfigurationOption() {
    }
}
