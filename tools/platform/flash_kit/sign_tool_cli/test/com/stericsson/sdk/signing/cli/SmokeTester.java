package com.stericsson.sdk.signing.cli;

import org.junit.Assert;
import org.junit.Test;

/**
 * @author emicroh
 *
 */
public class SmokeTester {

    /** */
    @Test
    public void confOptionsSmokeTest() {
        for (ConfOptions confOption : ConfOptions.values()) {
            Assert.assertTrue(confOption.getName().length() > 0);
        }
    }
}
