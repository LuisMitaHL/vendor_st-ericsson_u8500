package com.stericsson.sdk.equipment.ui;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.common.configuration.system.ISystemPropertiesContributor;
import com.stericsson.sdk.equipment.ui.preferences.backend.FlashToolBackendUIConfigurationService;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {

    /** */
    public static final String EQUIPMENT_ICON = "com.stericsson.sdk.equipment.ui.icon.Equipment";

    /** */
    public static final String EQUIPMENT_ICON_SHUTDOWN = "com.stericsson.sdk.equipment.ui.icon.Shutdown";

    /** */
    public static final String EQUIPMENT_ICON_REBOOT = "com.stericsson.sdk.equipment.ui.icon.Reboot";

    /** */
    public static final String EQUIPMENT_ICON_ERASE = "com.stericsson.sdk.equipment.ui.icon.Erase";

    /** */
    public static final String EQUIPMENT_ICON_PROCESS = "com.stericsson.sdk.equipment.ui.icon.Process";

    /** */
    public static final String EQUIPMENT_ICON_DUMP = "com.stericsson.sdk.equipment.ui.icon.Dump";

    /** */
    public static final String WARM_EQUIPMENT_ICON_DUMP = "com.stericsson.sdk.equipment.ui.icon.WarmDump";

    /** */
    public static final String ICON_DUMP_DELETE = "com.stericsson.sdk.equipment.ui.icon.Delete";

    /** */
    public static final String EQUIPMENT_ICON_AUTHENTICATE_CERT =
        "com.stericsson.sdk.equipment.ui.icon.AuthenticateCertificate";

    /** */
    public static final String EQUIPMENT_ICON_AUTHENTICATE = "com.stericsson.sdk.equipment.ui.icon.AuthenticateKey";

    /** The plug-in ID */
    public static final String PLUGIN_ID = "com.stericsson.sdk.equipment.ui";

    // The shared instance
    private static Activator plugin;

    private FlashToolBackendUIConfigurationService configurationService;

    private static BundleContext bundleContext;

    /**
     * @return Bundle context instance
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    private static void setPlugin(Activator pluginActivator) {
        plugin = pluginActivator;
    }

    /**
     * The constructor
     */
    public Activator() {
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        setPlugin(this);
        Activator.setBundleContext(context);

        getImageRegistry().put(EQUIPMENT_ICON,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/me.png")));
        getImageRegistry().put(EQUIPMENT_ICON_PROCESS,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/process.png")));
        getImageRegistry().put(EQUIPMENT_ICON_DUMP,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/dump.png")));
        getImageRegistry().put(EQUIPMENT_ICON_ERASE,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/erase.png")));
        getImageRegistry().put(EQUIPMENT_ICON_REBOOT,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/reboot.png")));
        getImageRegistry().put(EQUIPMENT_ICON_SHUTDOWN,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/shutdown.png")));
        getImageRegistry().put(WARM_EQUIPMENT_ICON_DUMP,
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/warm_dump.png")));
        getImageRegistry().put(ICON_DUMP_DELETE,
            PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_DELETE));
        getImageRegistry().put(IDE.SharedImages.IMG_OBJ_PROJECT,
            PlatformUI.getWorkbench().getSharedImages().getImage(IDE.SharedImages.IMG_OBJ_PROJECT));
        getImageRegistry().put(
            EQUIPMENT_ICON_AUTHENTICATE_CERT,
            ImageDescriptor
                .createFromURL(Activator.getDefault().getBundle().getResource("icons/authenticate_cert.png")));
        getImageRegistry().put(
            EQUIPMENT_ICON_AUTHENTICATE,
            ImageDescriptor
                .createFromURL(Activator.getDefault().getBundle().getResource("icons/authenticate_keys.png")));

        // setSystemVariables();

        configurationService = new FlashToolBackendUIConfigurationService();
        Activator.getBundleContext().registerService(ISystemPropertiesContributor.class.getName(),
            configurationService, null);

        BackendHandler.startBackend();
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        Activator.setPlugin(null);
        super.stop(context);
    }

    /**
     * Returns the shared instance
     * 
     * @return the shared instance
     */
    public static Activator getDefault() {
        return plugin;
    }

    /**
     * Sets backend specific system variables
     */
    @Deprecated
    public static void setSystemVariables() {
        //
        // IPreferenceStore store = Activator.getDefault().getPreferenceStore();
        // if (store != null) {
        // System.setProperty(SystemProperites.BACKEND_CONFIGURATIONS_ROOT, store
        // .getString(FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE));
        // System.setProperty(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH, store
        // .getString(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH));
        // System.setProperty(FlashKitPreferenceConstants.BACKEND_LOADER_PATH, store
        // .getString(FlashKitPreferenceConstants.BACKEND_LOADER_PATH));
        // System.setProperty(FlashKitPreferenceConstants.BACKEND_LCD_FILE, store
        // .getString(FlashKitPreferenceConstants.BACKEND_LCD_FILE));
        // System.setProperty(FlashKitPreferenceConstants.BACKEND_LCM_FILE, store
        // .getString(FlashKitPreferenceConstants.BACKEND_LCM_FILE));
        // System.setProperty(FlashKitPreferenceConstants.BACKEND_BRP_PORT, store
        // .getString(FlashKitPreferenceConstants.BACKEND_BRP_PORT));
        // System.setProperty(SystemProperites.BACKEND_AUTO_DOWNLOAD_ENABLED, store
        // .getString(SystemProperites.BACKEND_AUTO_DOWNLOAD_ENABLED));
        // System.setProperty(SystemProperites.BACKEND_AUTO_DELETE_ENABLED, store
        // .getString(SystemProperites.BACKEND_AUTO_DELETE_ENABLED));
        // System.setProperty(SystemProperites.BACKEND_DOWNLOAD_PATH, store
        // .getString(SystemProperites.BACKEND_DOWNLOAD_PATH));
        // }
    }

    /**
     * Returns an image descriptor for the image file at the given plug-in relative path.
     * 
     * @param path
     *            the path
     * @return the image descriptor
     */
    public static ImageDescriptor getImageDescriptor(final String path) {
        return imageDescriptorFromPlugin(PLUGIN_ID, path);
    }

}
