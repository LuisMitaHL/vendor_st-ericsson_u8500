package com.stericsson.sdk.equipment.ui.editors.security;

import java.util.Locale;

/**
 * @author xdancho
 * 
 */
public final class SecurityFactory {

    private SecurityFactory() {

    }

    /**
     * @param platform
     *            platform string
     * @return ISecurityPropertiesHandler of the platform
     * @throws Exception
     *             if platform not supported
     */
    public static ISecurityPropertiesHandler getSecurityPropertiesHandler(String platform) throws Exception {

        if (platform.toLowerCase(Locale.getDefault()).equals("dummy")) {
            return new DummySecurityPropertiesHandler();
        } else if (platform.toLowerCase(Locale.getDefault()).equals("u8500")
            || platform.toLowerCase(Locale.getDefault()).equals("l9540")) {
            return new U8500SecurityPropertiesHandler();
        } else if (platform.toLowerCase(Locale.getDefault()).equals("u5500")) {
            return new U5500SecurityPropertiesHandler();
        } else {
            throw new Exception("No security handler defined for platform: " + platform);
        }
    }

}
