package com.stericsson.sdk.assembling;

import org.junit.Test;

import static org.junit.Assert.*;

/**
 * 
 * @author xolabju
 * 
 */
public class AbstractAssemblerTest {

    private boolean test = false;

    private int testCount = 0;

    /**
     * 
     */
    @Test
    public void testAddRemoveListener() {

        IAssemblerListener listener = new IAssemblerListener() {

            public void assemblerMessage(IAssembler assembler, String message) {
                test = true;
            }
        };
        AbstractAssembler asm = new AbstractAssembler() {

            public void assemble(IAssemblerSettings settings) throws AssemblerException {

            }
        };
        asm.addListener(listener);
        asm.notifyMessage("test");
        assertTrue(test);
        test = false;
        asm.removeListener(listener);
        asm.notifyMessage("test");
        assertFalse(test);

        IAssemblerListener countListener = new IAssemblerListener() {

            public void assemblerMessage(IAssembler assembler, String message) {
                testCount++;
            }
        };
        asm.addListener(countListener);
        asm.addListener(countListener);
        asm.notifyMessage("test");
        assertEquals(1, testCount);
    }
}
