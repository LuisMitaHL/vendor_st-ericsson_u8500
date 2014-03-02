package com.stericsson.sdk.signing.u5500;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500SignPackageTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testU5500SignPackage() {
        String alias = "alias123";
        String type = "type123";
        String parent = "parent123";
        GenericCertificate certificate = new GenericSignedHeader();
        GenericSignPackage pkg = new GenericSignPackage(alias, parent, type, certificate);
        assertEquals(certificate, pkg.getCertificate());

        assertEquals(alias, pkg.getAlias());
        assertEquals(type, pkg.getType());
        assertEquals(parent, pkg.getParent());
    }

}
