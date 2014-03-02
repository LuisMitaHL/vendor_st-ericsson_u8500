package com.stericsson.sdk.equipment.tasks;

import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.image.BinaryImage;
import com.stericsson.sdk.equipment.image.TableOfContents;
import com.stericsson.sdk.equipment.image.TableOfContentsItem;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomzap
 * 
 */
public class TokenTypeTest extends TestCase {

    private EquipmentBootTask equipmentBootTask;
    private AbstractPort port;
    private BinaryImage bootImage;

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {

        port = new AbstractPort("test") {
            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return 0;
            }
            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return 0;
            }
        };

        AbstractLoaderCommunicationEquipment equipment = new AbstractLoaderCommunicationEquipment(port, null) {

            public EquipmentType getEquipmentType() {
                return null;
            }
            @Override
            public String toString() {
                return null;
            }
        };

        equipmentBootTask = new EquipmentBootTask(equipment) {
            @Override
            public void notifyTaskMessage(String message) {
            }
            @Override
            public void writeBinary(String binaryName, AbstractPort pPort, BinaryImage bBootImage) throws IOException {
            }
            @Override
            void readFully(AbstractPort pPort, byte[] pBuffer, int pOffset, int pLength) throws PortException {
                System.out.println();
            }
        };

        bootImage = new BinaryImage() {
            @Override
            public TableOfContents getTOC() {
                return new TableOfContents() {
                    @Override
                    public TableOfContentsItem getItem(String fileName) {
                        return new TableOfContentsItem();
                    }
                };
            }
        };
    }

    /**
     * 
     */
    @Test
    public void testTokenTypes() {
        try {
            for (TokenType tokenType : TokenType.values()) {
                if (tokenType != TokenType.MEM_INIT_FAILED_TOKEN) {
                    tokenType.handleToken(equipmentBootTask, port, bootImage);
                }
            }

            assertEquals(TokenType.UNKNOWN, TokenType.getTokenByValue(TokenType.UNKNOWN.getTokenValue()));
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }
}