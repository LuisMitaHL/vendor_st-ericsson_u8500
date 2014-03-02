package com.stericsson.sdk.signing.ui.jobs;

import java.io.File;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.signing.creators.KeyPairCreator;
import com.stericsson.sdk.signing.data.KeyPairData;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class GenerateKeyPairJob extends Job {

    private static final String GENERATE_RSA_KEY_PAIR = "GENERATE RSA KEY PAIR";

    private static final String CREATE_RSA_KEYS = null;

    private File privF;

    private File pubF;

    private int keyLength;

    /**
     * @param pPrivF
     *            File to which private key should be written.
     * @param pPubF
     *            File to which public key should be written.
     * @param pKeyLength
     *            Key length in bits.
     */
    public GenerateKeyPairJob(File pPrivF, File pPubF, int pKeyLength) {
        super(GENERATE_RSA_KEY_PAIR);
        privF = pPrivF;
        pubF = pPubF;
        keyLength = pKeyLength;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        monitor.beginTask(CREATE_RSA_KEYS, IProgressMonitor.UNKNOWN);
        KeyPairData keyPair = new KeyPairData(privF.getAbsolutePath(), pubF.getAbsolutePath(), keyLength);
        try {
            KeyPairCreator creator = new KeyPairCreator();
            monitor.setTaskName("Writing");
            creator.create(keyPair);
        } catch (NoSuchAlgorithmException e) {
            return new Status(IStatus.ERROR, "com.stericsson.sdk.signing.ui",
                "Implementation or RSA could not be found.", e);
        } catch (IOException e) {
            return new Status(IStatus.ERROR, "com.stericsson.sdk.signing.ui",
                "I/O error occurs. Propably file could not be written.", e);
        } finally {
            monitor.done();
        }
        return Status.OK_STATUS;
    }
}
