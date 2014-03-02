package com.stericsson.sdk.signing;

import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerService;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.signerservice.local.LocalSignerService;

/**
 * 
 * @author xolabju
 * 
 */
public final class SignerFacade implements ISignerFacade {
    /**
     * 
     */
    public SignerFacade() {
    }

    /**
     * 
     * @param settings
     *            the signer settings instance
     * @param listener
     *            the listener object
     * @param local
     *            true to use local sign packages and keys, false to use a sign server
     * @throws SignerException
     *             if anything goes wrong
     */
    public void sign(ISignerSettings settings, ISignerListener listener, boolean local) throws SignerException {
        if (settings == null) {
            throw new SignerException("Signer settings object is null");
        }
        ISignerService service = null;
        try {
            Boolean useCommonFolder = (Boolean)settings.getSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES);
            if(useCommonFolder == null){
                useCommonFolder = false;
                settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, useCommonFolder);
            }
            settings.setSignerSetting(ISignerSettings.KEY_LOCAL, local);
            service = local ? new LocalSignerService(useCommonFolder, (ISignerSettings)settings) 
                : new SignerService(useCommonFolder, (ISignerSettings)settings);
            if (listener != null) {
                service.addListener(listener);
            }
            if (settings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY) == null) {
                service.configure();
                Object signPackageAlias = settings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS);
                if (signPackageAlias == null) {
                    throw new SignerException("No sign package is specified");
                }
                ISignPackage signPackage = service.getSignPackage(signPackageAlias.toString(), true);

                // Store important objects into signer settings
                settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE, signPackage);
                settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias.toString());
            } else {
                service.setSignKeyPath(settings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY).toString(), true);
            }

            settings.setSignerSetting(ISignerSettings.KEY_SIGNER_SERVICE, service);
            ISigner signer = SignerFactory.createSigner(settings);
            if (signer == null) {
                throw new SignerException("Failed to create signer");
            }
            if (listener != null) {
                signer.addListener(listener);
            }
            signer.sign(settings);
        } finally {
            if (service != null) {
                service.disconnect();
            }
        }
    }

    /**
     *@param listener
     *            the listener object
     * @param local
     *            true to use local sign packages and keys, false to use a sign server
     * @param useCommonFolder
     *            common folder where keys and packages are store
     * @param signerSettings
     *         passing info for overriding env variables.
     * @return the found sign packages
     * @throws SignerException
     *             if anything goes wrong
     */
    public ISignPackageInformation[] getSignPackages(ISignerServiceListener listener, boolean local, boolean useCommonFolder,
            ISignerSettings signerSettings)
        throws SignerException {
        ISignerService service = null;
        ISignPackageInformation[] packages = null;
        try {
            service = local ? new LocalSignerService(useCommonFolder, (ISignerSettings)signerSettings) 
                : new SignerService(useCommonFolder, (ISignerSettings)signerSettings);
            if (listener != null) {
                service.addListener(listener);
            }
            service.configure();
            packages = service.getSignPackagesInformation();
        } finally {
            if (service != null) {
                service.disconnect();
            }
        }
        return packages;
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackage getSignPackage(String packageName, ISignerServiceListener listener, boolean local, 
            boolean useCommonFolder, ISignerSettings signerSettings) throws SignerServiceException {

        ISignerService service = null;
        ISignPackage pkg = null;
        try {
            service = local ? new LocalSignerService(useCommonFolder, (GenericSignerSettings)signerSettings) 
                : new SignerService(useCommonFolder, (GenericSignerSettings)signerSettings);
            if (listener != null) {
                service.addListener(listener);
            }
            service.configure();
            pkg = service.getSignPackage(packageName, true);
        } finally {
            if (service != null) {
                service.disconnect();
            }
        }
        return pkg;
    }
}
