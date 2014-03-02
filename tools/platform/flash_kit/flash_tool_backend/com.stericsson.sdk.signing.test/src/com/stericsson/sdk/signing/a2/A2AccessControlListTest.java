package com.stericsson.sdk.signing.a2;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.cops.COPSControlFields;

/**
 * @author xtomzap
 */
public class A2AccessControlListTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testA2Acl() {
        int initialValueBABE0200 = 29149;
        int resultValueBABE0200;

        A2AccessControlList a = new A2AccessControlList(COPSControlFields.HID_BABE0200, initialValueBABE0200);
        resultValueBABE0200 = a.getValue();

        assertEquals(initialValueBABE0200, resultValueBABE0200);

        A2AccessControlList b = new A2AccessControlList(COPSControlFields.HID_BABE0200);
        b.getValue();

        Exception expected;

        try {
            expected = null;
            a.setDebug((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setTargetCPU(A2AccessControlList.TARGET_CPU_AUDIO);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setTargetCPU((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setSWType((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setETXLevel((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setETXLevelHeader((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setInteractiveLoadBit((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setApplicationSecurity((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setPayloadType((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

        try {
            expected = null;
            a.setHeaderAction((byte) 100);
        } catch (IllegalArgumentException e) {
            expected = e;
        }

        assertNotNull(expected);

    }
}
