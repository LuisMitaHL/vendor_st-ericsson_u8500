package com.stericsson.sdk.signing.u5500;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPart;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWSecurityRomHeader;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;


/**
 * @author xtomzap
 * 
 */
public class U5500ISSWCertificateTest extends TestCase {

    private GenericISSWCertificate cert;

    ArrayList<Short> modelIds;

    /**
     * Constructor.
     */
    public U5500ISSWCertificateTest() {
        modelIds = new ArrayList<Short>();
        modelIds.add((short) ((Short.MAX_VALUE * 2) + 1));
        modelIds.add((short) 234);

    }

    /**
     * Test Model Id
     */
    public void testModelId() {
        GenericISSWCustomerPart custPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE1);
        List<Short> test = new ArrayList<Short>();

        // test empty, default 0 model IDs
        custPart.setModelId(test);
        assertEquals(0, custPart.getModelIds().size());

        test.add((short) 0xCAFE);
        byte[] modelIdExpected = new byte[]{(byte)0xFE, (byte)0xCA};

        custPart.setModelId(test);
        assertEquals(1, custPart.getModelIds().size());
        byte[] data = custPart.getData();
        assertEquals(1, data[40]);
        byte[] modelId = new byte[2];
        System.arraycopy(data, 42, modelId, 0, modelId.length);
        assertTrue(Arrays.equals(modelIdExpected, modelId));

    }

    /***/
    public void testSetData() {
        cert = new GenericISSWCertificate();

        cert.setCustomerPart(getCustomerPart());
        cert.setSecRomHeader(getSecurityRomHeader(getCustomerPart().getData().length));
        cert.setRootKey(getRootKey());

        assertEquals(modelIds, cert.getCustomerPart().getModelIds());

        byte[] data1 = cert.getData();
        try {
            cert.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        byte[] data2 = cert.getData();

        assertTrue(Arrays.equals(data1, data2));

        byte[] signableData = cert.getSignableData();
        byte[] signature = cert.getSignature().getData();
        byte[] uncheckedBlob = cert.getUncheckedBlob().getData();
        byte[] data3 = new byte[signableData.length + uncheckedBlob.length + signature.length];

        // add signable data
        System.arraycopy(signableData, 0, data3, 0, signableData.length);
        // add unchecked blob
        System.arraycopy(uncheckedBlob, 0, data3, signableData.length, uncheckedBlob.length);
        // add signature
        System.arraycopy(signature, 0, data3, signableData.length + uncheckedBlob.length, signature.length);

        assertTrue(Arrays.equals(data3, data2));
    }

    /***/
    public void testSetDataExceptions() {
        cert = new GenericISSWCertificate();
        boolean exceptionThrowen = false;

        try {
            cert.setData(null);
        } catch (Exception e) {
            exceptionThrowen = true;
        }
        if (exceptionThrowen) {
            assertTrue(true);
        } else {
            assertTrue(false);
        }
        exceptionThrowen = false;

        try {
            cert.setData(new byte[] {});
        } catch (Exception e) {
            exceptionThrowen = true;
        }
        if (exceptionThrowen) {
            assertTrue(true);
        } else {
            assertTrue(false);
        }
    }

    private GenericISSWCustomerPart getCustomerPart() {
        GenericISSWCustomerPart custPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE1);
        Random random = new Random();
        byte[] pModulus = new byte[32];

        random.nextBytes(pModulus);

        custPart.setMinorBuildVersion((short) 99);
        custPart.setMajorBuildVersion((short) 99);
        custPart.setUTCTime(99);
        custPart.setFlags(99);
        custPart.setSigningConstraints(new byte[] {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

        custPart.setModelId(modelIds);

        GenericISSWCustomerPartKey key = new GenericISSWCustomerPartKey();

        key.setKeySize(48);

        key.setKeyTypeWithRevocationHashTypes(new SignatureTypeWithRevocationHashTypes(SignatureType.RSASSA_PKCS_V1_5));
        key.setExponent(6);
        key.setDummy((short) 4);
        key.setModulusSize((short) pModulus.length);
        key.setModulus(pModulus);

        custPart.addKey(key);

        return custPart;
    }

    private GenericKey getRootKey() {
        GenericKey rootKey = new GenericKey();
        Random random = new Random();
        byte[] pModulus = new byte[32];

        random.nextBytes(pModulus);

        rootKey.setPublicExponent(6);
        rootKey.setDummy((short) 4);
        rootKey.setModulusSize((short) pModulus.length);
        rootKey.setModulus(pModulus);

        return rootKey;
    }

    private GenericISSWSecurityRomHeader getSecurityRomHeader(int customerPartLength) {
        Random random = new Random();
        GenericISSWSecurityRomHeader securityRomHeader = new GenericISSWSecurityRomHeader();

        securityRomHeader.setRootKeySize(40);
        securityRomHeader.setCustomerPartSize(customerPartLength);
        securityRomHeader.setUncheckedBlobSize(0);
        securityRomHeader.setSignatureSize(0);
        securityRomHeader.setSpeedUpMask(99);

        byte[] speedUpData = new byte[256];
        random.nextBytes(speedUpData);

        securityRomHeader.setSpeedUpData(speedUpData);
        securityRomHeader.setSpeedUpPool(new byte[] {
            1, 2, 3, 4});
        securityRomHeader.setSpeedUpSpare(new byte[] {
            1, 2, 3, 4});

        byte[] isswHashData = new byte[64];
        random.nextBytes(isswHashData);

        securityRomHeader.setHash(isswHashData);
        securityRomHeader.setCodeLength(99);
        securityRomHeader.setRWDataLength(99);
        securityRomHeader.setZIDataLength(99);
        securityRomHeader.setISSWLoadLocation(99);
        securityRomHeader.setISSWStartAddr(99);

        return securityRomHeader;
    }

    /**
     * 
     */
    public void testCustomerPart() {
        GenericISSWCustomerPart custPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE1);
        custPart.setMajorBuildVersion((short) 1);
        assertEquals((short) 1, custPart.getMajorBuildVersion());
        custPart.setMinorBuildVersion((short) 2);
        assertEquals((short) 2, custPart.getMinorBuildVersion());
        custPart.setFlags(0xFFFFFFFF);
        assertEquals(0xFFFFFFFF, custPart.getFlags());
        custPart.setUTCTime(1);
        assertEquals(1, custPart.getUTCTime());

        custPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE2);
        custPart.setMajorBuildVersion((short) 1);
        assertEquals((short) 0, custPart.getMajorBuildVersion());
        custPart.setMinorBuildVersion((short) 2);
        assertEquals((short) 0, custPart.getMinorBuildVersion());
        custPart.setFlags(0xFFFFFFFF);
        assertEquals(0xFFFFFFFF, custPart.getFlags());
        custPart.setUTCTime(1);
        assertEquals(0, custPart.getUTCTime());
        custPart.setReserved(new byte[] {
            1});
    }

}
