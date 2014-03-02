package com.stericsson.sdk.signing.util;

import com.stericsson.sdk.signing.AbstractSignerSettings;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerSettingsException;

public class ListPackagesSignerSettings extends AbstractSignerSettings{

	public ListPackagesSignerSettings(){
        addSignerSetting(KEY_SIGN_SERVICE_SERVERS);
        addSignerSetting(KEY_SIGN_PACKAGE_ROOT);
        addSignerSetting(KEY_LOCAL_SIGN_PACKAGE_ROOT);
        addSignerSetting(KEY_LOCAL_KEY_ROOT);
        addSignerSetting(KEY_LOCAL_ENCRYPTION_KEY_ROOT);
	}
	
	public void setFrom(ISignerSettings settings)
			throws SignerSettingsException {
	}

}
