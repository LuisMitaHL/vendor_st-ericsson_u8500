/**
 * 
 */
package com.stericsson.sdk.common.configuration.test;

import org.junit.Test;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationTokenizer;

import junit.framework.TestCase;


/**
 * @author qdaneke
 * 
 */
public class MESPConfigurationTokenizerTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testParseCharacter() {
        MESPConfigurationTokenizer tokenizer = new MESPConfigurationTokenizer();
        tokenizer.parseCharacter('=');
        assertEquals(MESPConfigurationTokenizer.TOKEN_EQUALS, tokenizer.getToken());
        tokenizer.parseCharacter('{');
        assertEquals(MESPConfigurationTokenizer.TOKEN_LEFT_BRACE, tokenizer.getToken());
        tokenizer.parseCharacter('}');
        assertEquals(MESPConfigurationTokenizer.TOKEN_RIGHT_BRACE, tokenizer.getToken());
    }

    /**
     * 
     */
    @Test
    public void test() {
    }
}
