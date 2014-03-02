package com.stericsson.sdk.signing.signerservice;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.util.HashMap;

import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.junit.Test;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * @author xtomlju
 */
public class SignerServiceTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testListener() {

        ISignerService service = new AbstractSignerService() {

            public void connect(InputStream input, OutputStream output) throws SignerServiceException {
                notifyMessage("CONNECT");
            }

            public void disconnect() {
                notifyMessage("DISCONNECT");
            }

            public ISignPackage getSignPackage(String alias, boolean enableHelloHandshake)
                throws SignerServiceException {
                notifyMessage("GETSIGNPACKAGE");
                return null;
            }

            public void configure() throws SignerServiceException {
                notifyMessage("SETUP");
            }

            public byte[] signA2(byte[] data) throws SignerServiceException {
                notifyMessage("SIGNA2");
                return null;
            }

            public byte[] encryptSessionKey(int intValue, int[] key) {
                notifyMessage("encryptSessionKey");
                return null;
            }

            public ISignPackageInformation[] getSignPackagesInformation() throws SignerServiceException {
                notifyMessage("GETAVAILABLESIGNPACKAGES");

                return null;
            }

            public ISignPackageInformation[] getSignPackagesInformation(boolean enableHelloHandshake,
                boolean includeSubPackages) throws SignerServiceException {
                if (enableHelloHandshake) {
                    notifyMessage("GETAVAILABLESIGNPACKAGES with hello enabled");
                } else {
                    notifyMessage("GETAVAILABLESIGNPACKAGES with hello disabled");
                }
                return null;
            }

            public HashMap<String, String> getSubPackages(String parent) throws SignerServiceException {
                notifyMessage("GETSUBPACKAGES for " + parent);
                return null;
            }

            public byte[] signU8500(byte[] data) throws SignerServiceException {
                notifyMessage("SIGNU8500");
                return null;
            }

            public String getSubPackageAlias(String pkgName, int exponent, byte[] modulus, String encrRoot) {
                // TODO Auto-generated method stub
                return null;
            }

            public byte[] signU5500(byte[] data, int signatureHashTypeAlgoritm, SignatureType pSignatureType)
                throws SignerServiceException {
                return null;
            }

            public byte[] signX509(byte[] data) throws SignerServiceException {
                return null;
            }

            public ISignPackage
                    getSignPackage(ISignPackage alias, BigInteger exponent, BigInteger modulus) throws SignerServiceException,
                            FileNotFoundException {
                notifyMessage("PACKAGE FOR " + alias);
                return null;
            }

            public void setSignKeyPath(String pSignKeyPath,
                    boolean enableHelloHandshake) throws SignerServiceException {

            }
        };

        ISignerServiceListener mockListener = EasyMock.createMock(ISignerServiceListener.class);
        mockListener.signerServiceMessage(EasyMock.eq(service), EasyMock.eq("CONNECT"));
        EasyMock.expectLastCall();
        mockListener.signerServiceMessage(EasyMock.eq(service), EasyMock.eq("DISCONNECT"));
        EasyMock.expectLastCall();
        mockListener.signerServiceMessage(EasyMock.eq(service), EasyMock.eq("GETSIGNPACKAGE"));
        EasyMock.expectLastCall();
        mockListener.signerServiceMessage(EasyMock.eq(service), EasyMock.eq("SETUP"));
        EasyMock.expectLastCall();
        mockListener.signerServiceMessage(EasyMock.eq(service), EasyMock.eq("SIGNA2"));
        EasyMock.expectLastCall();

        EasyMock.replay(mockListener);

        service.addListener(mockListener);

        try {
            service.connect(null, null);
            service.disconnect();
            service.getSignPackage(null, true);
            service.configure();
            service.signA2(null);
        } catch (SignerException e) {
            fail(e.getMessage());
        }

        service.removeListener(mockListener);
        service.disconnect();

        EasyMock.verify(mockListener);
    }

}
