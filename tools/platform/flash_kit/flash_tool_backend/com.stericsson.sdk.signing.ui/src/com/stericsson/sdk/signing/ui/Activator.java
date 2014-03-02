package com.stericsson.sdk.signing.ui;

import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {

    /** */
    public static final String PLUGIN_ID = "com.stericsson.sdk.signing.ui";

    // The shared instance
    private static Activator plugin;

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
        SignedSoftwareUtils.setSystemVariables();
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        setPlugin(null);
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

    private static void setPlugin(Activator pPlugin) {
        plugin = pPlugin;
    }
}
