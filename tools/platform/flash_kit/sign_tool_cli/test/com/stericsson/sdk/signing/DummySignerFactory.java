package com.stericsson.sdk.signing;

import com.stericsson.sdk.signing.cli.SignerFactory;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomlju
 */
public class DummySignerFactory extends SignerFactory {

    /**
     * {@inheritDoc}
     */
    public ISignerFacade createSignerFacade(String type) {
        return new ISignerFacade() {

            public void sign(ISignerSettings settings, ISignerListener listener, boolean local) throws SignerException {

            }

            public ISignPackageInformation[] getSignPackages(ISignerServiceListener listener, boolean local, 
                    boolean common, ISignerSettings signerSettings) {

                ISignPackageInformation pkgInfo = new ISignPackageInformation() {

                    public boolean isEncryptRequired() {
                        return true;
                    }

                    public String getType() {
                        return "DummyType";
                    }

                    public String getParent() {
                        return "DummyParent";
                    }

                    public String getName() {
                        return "DummyName";
                    }

                    public String[] getChipIdentifiers() {
                        return new String[] {
                            "123", "456"};
                    }
                };
                return new ISignPackageInformation[] {
                    pkgInfo};
            }

            public ISignPackage getSignPackage(String packageName, ISignerServiceListener listener, 
                    boolean local, boolean common, ISignerSettings signerSettings)
                throws SignerServiceException {
                return new ISignPackage() {

                    public String getType() {
                        return "DummyType";
                    }

                    public String getRevision() {
                        return "Dummy R1A";
                    }

                    public String getParent() {
                        return null;
                    }

                    public String getDescription() {
                        return "Dummy Sign Package";
                    }

                    public String getAlias() {
                        return "Dummy sign package";
                    }
                };
            }
        };
    }
}
