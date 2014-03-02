package com.stericsson.sdk.signing.cli.loadmodules;

import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.ISign;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomzap
 *
 */
public class DummyLoadModulesFacade implements ISignerFacade {

    private int result = 0;

    /**
     * @param pResult result
     */
    public DummyLoadModulesFacade(int pResult) {
        result = pResult;
    }
    /**
     * {@inheritDoc}
     */
    public ISignPackage getSignPackage(String pPackageName, ISignerServiceListener pListener, boolean pLocal, 
            boolean common, ISignerSettings signerSettings)
        throws SignerServiceException {
        // TODO Auto-generated method stub
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackages(ISignerServiceListener pListener, boolean pLocal, 
            boolean common, ISignerSettings signerSettings)
        throws SignerException {
        // TODO Auto-generated method stub
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public void sign(ISignerSettings settings, ISignerListener pListener, boolean pLocal) throws SignerException {
        if (settings == null) {
            throw new SignerException("Signer settings object is null");
        }

        ISign sign = createSign();
        settings.setSignerSetting(LoadModulesSignerSettings.KEY_SIGN, sign);

        ISigner signer = new LoadModulesSigner();
        if (signer == null) {
            throw new SignerException("Failed to create signer");
        }

        signer.sign(settings);
    }

    private ISign createSign() {
        return new ISign() {
            public int execute(String[] pArguments) {
                return result;
            }
        };
    }
}
