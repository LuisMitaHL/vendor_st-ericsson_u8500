package com.stericsson.sdk.equipment.ui.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import com.stericsson.sdk.backend.remote.io.ServerProperties;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Class used to initialize default preference values.
 */
public class FlashKitPreferenceInitializer extends AbstractPreferenceInitializer {

    /**
     * {@inheritDoc}
     */
    public void initializeDefaultPreferences() {
        IPreferenceStore store = Activator.getDefault().getPreferenceStore();
        store.setDefault(FlashKitPreferenceConstants.BACKEND_CONFIGURATION_FILE, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_LCD_FILE, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_LCM_FILE, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_LOADER_PATH, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_ACCEPT_EQUIPMENT, false);
        store.setDefault(FlashKitPreferenceConstants.BACKEND_ACCEPT_CLIENTS, false);
        store.setDefault(FlashKitPreferenceConstants.BACKEND_USB_ENABLED, true);
        store.setDefault(FlashKitPreferenceConstants.BACKEND_ACTIVE_PROFILE, "");
        store.setDefault(FlashKitPreferenceConstants.BACKEND_BRP_PORT, ServerProperties.DEFAULT_PORT_NUMBER);
        store.setDefault(FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT,
            FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_ASK);
        store.setDefault(FlashKitPreferenceConstants.SIGNING_ROOT_KEY_HASH_TYPE,
            "SHA-1");

    }
}
