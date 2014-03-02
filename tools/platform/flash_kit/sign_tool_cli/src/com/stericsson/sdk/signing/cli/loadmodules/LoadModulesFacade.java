package com.stericsson.sdk.signing.cli.loadmodules;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.ISign;
import com.stericsson.sdk.signing.cli.Sign;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomzap
 *
 */
public class LoadModulesFacade implements ISignerFacade {

    /**
     * {@inheritDoc}
     */
    public ISignPackage getSignPackage(String pPackageName, ISignerServiceListener pListener, boolean pLocal, 
            boolean common, ISignerSettings signerSettings)
        throws SignerServiceException {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackages(ISignerServiceListener pListener, boolean pLocal, 
            boolean common, ISignerSettings signerSettings)
        throws SignerException {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public void sign(ISignerSettings settings, ISignerListener listener, boolean local) throws SignerException {
        if (settings == null) {
            throw new SignerException("Signer settings object is null");
        }

        ISign sign = new Sign(Logger.getLogger(Sign.class.getName()), null);
        settings.setSignerSetting(LoadModulesSignerSettings.KEY_SIGN, sign);

        ISigner signer = new LoadModulesSigner();

        if (listener != null) {
            signer.addListener(listener);
        }

        signer.sign(settings);
    }
}
