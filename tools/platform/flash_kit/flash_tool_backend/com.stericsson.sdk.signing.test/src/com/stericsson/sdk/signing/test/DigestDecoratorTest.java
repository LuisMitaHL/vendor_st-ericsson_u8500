/**
 * 
 */
package com.stericsson.sdk.signing.test;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * @author qknufun
 * 
 */
public class DigestDecoratorTest extends TestCase {

    private DigestDecorator mClassUnderTest;

    private DigestDecorator mClassUnderTest2;

    private DigestDecorator mClassUnderTest3;

    private DigestDecorator mClassUnderTest4;

    /**
     * @throws java.lang.Exception
     *             e
     */
    @Before
    public void setUp() throws Exception {

        mClassUnderTest = new DigestDecorator(DigestDecorator.SHA_1);
        mClassUnderTest2 = new DigestDecorator(DigestDecorator.SHA_256);
        mClassUnderTest3 = new DigestDecorator(DigestDecorator.SHA_384);
        mClassUnderTest4 = new DigestDecorator(DigestDecorator.SHA_512);

    }

    /**
     * @throws java.lang.Exception
     *             e
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for {@link com.stericsson.sdk.common.DigestDecorator#digest()}.
     */
    @Test
    public final void testDigest() {

        byte[] digest = mClassUnderTest.digest();
        byte[] digest2 = mClassUnderTest2.digest();
        byte[] digest3 = mClassUnderTest3.digest();
        byte[] digest4 = mClassUnderTest4.digest();

        assertNotNull(digest);
        assertNotNull(digest2);
        assertNotNull(digest3);
        assertNotNull(digest4);

        assertEquals(DigestDecorator.getHashSize(DigestDecorator.SHA_1), digest.length);
        assertEquals(DigestDecorator.getHashSize(DigestDecorator.SHA_256), digest2.length);
        assertEquals(DigestDecorator.getHashSize(DigestDecorator.SHA_384), digest3.length);
        assertEquals(DigestDecorator.getHashSize(DigestDecorator.SHA_512), digest4.length);
    }

    /**
     * Test method for {@link com.stericsson.sdk.common.DigestDecorator#digest()}.
     */
    @Test
    public final void testUpdateByteArray() {
        mClassUnderTest.update(new byte[] {
            (char) 0x11});
        byte[] digest = mClassUnderTest.digest();
        assertNotNull(digest);
    }

    /**
     * Test method for {@link com.stericsson.sdk.common.DigestDecorator#digest()}.
     */
    @Test
    public final void testDigestAndVerifyThatItDoesntClear() {
        mClassUnderTest.update(new byte[] {
            (char) 0x11});
        byte[] digest1 = mClassUnderTest.digest();
        byte[] digest2 = mClassUnderTest.digest();
        byte[] digest3 = mClassUnderTest.digest();
        byte[] digest4 = mClassUnderTest.digest();
        assertEquals("Hash result should always be the same!", digest1[0], digest2[0]);
        assertEquals("Hash result should always be the same!", digest2[0], digest3[0]);
        assertEquals("Hash result should always be the same!", digest3[0], digest4[0]);
    }

    /**
     * Test method for {@link com.stericsson.sdk.common.DigestDecorator#digest()}.
     */
    @Test
    public final void testUpdateByteArrayIntInt() {
        mClassUnderTest.update(new byte[0], 0, 0);
        byte[] digest = mClassUnderTest.digest();
        assertNotNull(digest);
    }

    /**
     * Test method for {@link com.stericsson.sdk.common.DigestDecorator#getHashSize(int)}
     */
    @Test
    public final void testGetHashSize() {
        assertEquals(20, DigestDecorator.getHashSize(DigestDecorator.SHA_1));
        assertEquals(32, DigestDecorator.getHashSize(DigestDecorator.SHA_256));
    }

}
