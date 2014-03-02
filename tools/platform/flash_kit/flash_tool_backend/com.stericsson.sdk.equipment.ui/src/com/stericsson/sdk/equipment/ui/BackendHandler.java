package com.stericsson.sdk.equipment.ui;

import org.eclipse.core.runtime.Platform;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleException;

/**
 * Class that makes sure that certain bundles are started at eclipse startup
 * 
 * @author xolabju
 * 
 */
final class BackendHandler {

    private BackendHandler() {
    }

    /** Bundles to be started in starting order */
    private static final String[] BUNDLE_IDS = new String[] {
        "com.stericsson.sdk.common.log4j",

        "com.stericsson.sdk.common",

        "com.stericsson.sdk.loader.communication",

        "com.stericsson.sdk.ftp.communication",

        "com.stericsson.sdk.equipment",

        "com.stericsson.sdk.equipment.io.usb",

        "com.stericsson.sdk.equipment.io.uart",

        "com.stericsson.sdk.equipment.io.ip",

        "com.stericsson.sdk.equipment.u8500",

        "com.stericsson.sdk.equipment.u8500.warm",

        "com.stericsson.sdk.equipment.u5500",

        "com.stericsson.sdk.equipment.u5500.warm",

        "com.stericsson.sdk.equipment.m7x00",

        "com.stericsson.sdk.equipment.l9540",

        "com.stericsson.sdk.backend"};

    /**
     * Starts the backend
     */
    @Deprecated
    static void startBackend() {
        for (String bundleId : BUNDLE_IDS) {
            Bundle bundle = Platform.getBundle(bundleId);
            if (bundle != null) {
                int state = bundle.getState();
                switch (state) {
                    case Bundle.ACTIVE:
                        // logger.debug("Active: " + bundleId);
                        break;
                    default:
                        // logger.debug("Not Active: " + bundleId);
                        try {
                            bundle.start();
                        } catch (BundleException e) {
                            e.printStackTrace();
                        }
                        break;
                }
            }
        }
    }

    /**
     * Stops the backend
     */
    @Deprecated
    static void stopBackend() {
        for (int i = BUNDLE_IDS.length - 1; i >= 0; i--) {
            Bundle bundle = Platform.getBundle(BUNDLE_IDS[i]);
            if (bundle != null) {
                int state = bundle.getState();
                switch (state) {
                    case Bundle.ACTIVE:
                        try {
                            bundle.stop();
                        } catch (BundleException e) {
                            e.printStackTrace();
                        }
                        break;
                    default:

                        break;
                }
            }
        }
    }
}
