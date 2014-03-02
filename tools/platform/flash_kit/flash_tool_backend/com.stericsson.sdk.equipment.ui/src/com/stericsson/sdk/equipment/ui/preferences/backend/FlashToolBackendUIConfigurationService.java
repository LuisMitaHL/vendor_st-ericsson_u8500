package com.stericsson.sdk.equipment.ui.preferences.backend;

import org.eclipse.jface.preference.IPreferenceStore;

import com.stericsson.sdk.common.configuration.system.ISystemPropertiesContributor;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Configuration service implementation to keep track of the flash tool backend configuration via an
 * eclipse preference page.
 * 
 * @author xolabju
 * 
 */
public class FlashToolBackendUIConfigurationService implements ISystemPropertiesContributor {

    /**
     * 
     * {@inheritDoc}
     */
    public String getProperty(String pKey) {
        IPreferenceStore preferenceStore = Activator.getDefault().getPreferenceStore();
        String value = preferenceStore.getString(pKey);
        return value.equals("") ? null : value;
    }

}
