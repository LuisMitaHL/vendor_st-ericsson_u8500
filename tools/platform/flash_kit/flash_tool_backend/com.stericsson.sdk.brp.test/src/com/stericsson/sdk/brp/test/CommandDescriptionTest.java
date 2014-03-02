package com.stericsson.sdk.brp.test;

import org.junit.Test;

import com.stericsson.sdk.brp.CommandDescription;
import com.stericsson.sdk.brp.CommandName;

import junit.framework.TestCase;

/**
 * @author xtomlju
 */
public class CommandDescriptionTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testGetCommandDescription() {
        for (CommandName cmd : CommandName.values()) {
            System.out.println(CommandDescription.getCommandDescription(cmd.name(), true));
        }

    }

}
