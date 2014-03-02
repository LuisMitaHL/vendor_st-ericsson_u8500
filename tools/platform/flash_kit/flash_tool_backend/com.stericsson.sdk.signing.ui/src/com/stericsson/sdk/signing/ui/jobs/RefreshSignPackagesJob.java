/**
 * 
 */
package com.stericsson.sdk.signing.ui.jobs;

import java.util.ArrayList;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;

/**
 * @author xhelciz
 * 
 */
public class RefreshSignPackagesJob extends Job {

    private static final String JOB_NAME = "Refresh Sign Packages";

    private boolean local = false;

    private IProgressMonitor monitor = null;

    private ArrayList<ISignPackageInformation> signPackagesToReturn = null;

    private boolean useCommonFolder = false;
    /**
     * Creates a job with specified name
     * 
     * @param pLocal
     *            true if the local signing is requested
     * @param pUseCommonFolder
     *            use common folder for keys and packages
     */
    public RefreshSignPackagesJob(boolean pLocal, boolean pUseCommonFolder) {
        super(JOB_NAME);
        local = pLocal;
        useCommonFolder = pUseCommonFolder;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        monitor = pMonitor;
        monitor.beginTask("Refreshing...", IProgressMonitor.UNKNOWN);
        signPackagesToReturn = new ArrayList<ISignPackageInformation>();

        final ISignerListener listener = new ISignerListener() {

            public void signerMessage(ISigner signer, String pMessage) {
                monitor.setTaskName(pMessage);
            }

            public void signerServiceMessage(ISignerService signerService, String pMessage) {
                monitor.setTaskName(pMessage);
            }
        };

        try {
            ISignPackageInformation[] signPackages = new SignerFacade().getSignPackages(listener, local, 
                    useCommonFolder,(ISignerSettings) null);
            for (ISignPackageInformation info : signPackages) {
                signPackagesToReturn.add(info);
            }
        } catch (final SignerException e) {
            return new Status(IStatus.ERROR,
                "Failed to refresh sign packages. Is the sign preferences set up correctly? Possible reason:\n", e
                    .getMessage());
        } finally {
            monitor.done();
        }

        return Status.OK_STATUS;
    }

    /**
     * Returns list of sign packages which is updated when this job is finished
     * 
     * @return list of sign packages
     */
    public ArrayList<ISignPackageInformation> getSignPackages() {
        return signPackagesToReturn;
    }
}
