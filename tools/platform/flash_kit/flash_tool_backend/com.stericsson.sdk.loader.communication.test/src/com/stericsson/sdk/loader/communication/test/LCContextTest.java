package com.stericsson.sdk.loader.communication.test;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationContext;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xdancho
 * 
 */
public class LCContextTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testLoaderCommunicationContextGetPort() {

        LoaderCommunicationContext ctx = new LoaderCommunicationContext(null);
        assertNull(ctx.getPort());
    }

    /**
     * 
     */
    @Test
    public void testLoaderCommunicationContextInstancePointer() {

        LoaderCommunicationContext ctx = new LoaderCommunicationContext(null);
        ctx.setInstancePointer(1L);
        assertEquals(1L, ctx.getInstancePointer());
    }

    /**
     * 
     */
    @Test
    public void testLoaderCommunicationContextSupportedCommands() {

        List<SupportedCommand> l = new ArrayList<SupportedCommand>();
        l.add(new SupportedCommand(1, 1, 1));
        l.add(new SupportedCommand(2, 2, 0));
        LoaderCommunicationContext ctx = new LoaderCommunicationContext(null);
        ctx.setSupportedCommands(l);
        assertEquals(l.get(0).getCommand(), ctx.getSupportedCommands().get(0).getCommand());
        assertEquals(l.get(1).getCommand(), ctx.getSupportedCommands().get(1).getCommand());

        assertTrue(ctx.isCommandPermitted(l.get(0).getGroup(), l.get(0).getCommand()));

        assertFalse(ctx.isCommandPermitted(l.get(1).getGroup(), l.get(1).getCommand()));

        assertTrue(ctx.isCommandGroupSupported(l.get(0).getGroup()));

        assertFalse(ctx.isCommandGroupSupported(l.get(1).getGroup()));

    }

}
