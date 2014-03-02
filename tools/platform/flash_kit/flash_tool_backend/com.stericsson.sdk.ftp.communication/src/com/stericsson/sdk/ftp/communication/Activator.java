package com.stericsson.sdk.ftp.communication;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * Activator for FTP communicator service
 * 
 * @author esrimpa
 * 
 */
public class Activator implements BundleActivator {

    /**
     * {@inheritDoc}
     * 
     * @see org.osgi.framework.BundleActivator#start(org.osgi.framework.BundleContext)
     */
    public void start(BundleContext context) throws Exception {
        Hashtable<String, String> normalDictionary = new Hashtable<String, String>();
        normalDictionary.put("type", "warm");

        IFTPCommunicationService ftpService = new FTPCommunicationService();
        context.registerService(IFTPCommunicationService.class.getName(), ftpService, normalDictionary);
    }

    /**
     * {@inheritDoc}
     * 
     * @see org.osgi.framework.BundleActivator#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
    }

}
