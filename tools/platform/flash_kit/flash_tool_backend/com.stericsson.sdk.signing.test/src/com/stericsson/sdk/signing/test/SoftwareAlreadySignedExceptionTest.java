package com.stericsson.sdk.signing.test;

import org.junit.Assert;
import org.junit.Test;

import com.stericsson.sdk.signing.SoftwareAlreadySignedException;

import junit.framework.TestCase;

/**
 * @author xtomzap
 *
 */
public class SoftwareAlreadySignedExceptionTest extends TestCase {

    private static final String MESSAGE_TEXT = "message text";

    private static final int RESULT_VALUE = 666;

    /**
     * Test method for {@link com.stericsson.sdk.signing.SoftwareAlreadySignedException()}.
     */
    @Test
    public void testResultValues() {
        SoftwareAlreadySignedException e = new SoftwareAlreadySignedException(MESSAGE_TEXT);
        Assert.assertEquals(e.getMessage(), MESSAGE_TEXT);
        Assert.assertEquals(SoftwareAlreadySignedException.UNKNOWN_ERROR, e.getResultValue());

        e = new SoftwareAlreadySignedException(MESSAGE_TEXT, RESULT_VALUE);
        Assert.assertEquals(e.getMessage(), MESSAGE_TEXT);
        Assert.assertEquals(RESULT_VALUE, e.getResultValue());
    }
}
