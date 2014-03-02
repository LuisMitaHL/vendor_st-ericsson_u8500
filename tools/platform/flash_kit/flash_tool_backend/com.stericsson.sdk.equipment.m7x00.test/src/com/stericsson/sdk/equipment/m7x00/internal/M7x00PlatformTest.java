/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.internal;

import static org.easymock.EasyMock.createMock;
import static org.easymock.EasyMock.expect;
import static org.easymock.EasyMock.replay;
import static org.easymock.EasyMock.verify;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class M7x00PlatformTest extends TestCase {

    private M7x00Platform platform;

    private M7x00Equipment equipment;

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {
        platform = new M7x00Platform();
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void tearDown() throws Exception {

    }


    /**
     * 
     */
    @Test
    public void testTaskMessage() {
        IEquipmentTask task = createMock(IEquipmentTask.class);
        expect(task.getEquipment()).andReturn(equipment);
        replay(task);
        platform.taskMessage(task, "message");
        verify(task);
    }

    /**
     * 
     */
    @Test
    public void testTaskMessageBadEquipment() {
        IEquipmentTask task = createMock(IEquipmentTask.class);
        IEquipment equipmentMock = createMock(IEquipment.class);
        expect(task.getEquipment()).andReturn(equipmentMock);
        replay(task);
        platform.taskMessage(task, "message");
        verify(task);
    }

    /**
     * 
     */
    @Test
    public void testTaskStart() {
        IEquipmentTask task = createMock(IEquipmentTask.class);
        expect(task.getEquipment()).andReturn(equipment);
        replay(task);
        platform.taskStart(task);
        verify(task);
    }

    /**
     * 
     */
    @Test
    public void testTaskStartBadEquipment() {
        IEquipmentTask task = createMock(IEquipmentTask.class);
        IEquipment equipmentMock = createMock(IEquipment.class);
        expect(task.getEquipment()).andReturn(equipmentMock);
        replay(task);
        platform.taskStart(task);
        verify(task);
    }

    /**
     * 
     */
    @Test
    public void testEmptyMethods() {
        IEquipmentTask task = createMock(IEquipmentTask.class);
        platform.taskProgress(task, 10, 10);
        platform.loaderCommunicationMessage(null, null, false);
        platform.loaderCommunicationProgress(0, 0);
    }

    /**
     * 
     */
    @Test
    public void testLoaderCommunicationErrorBadPortID() {
        IPort port = createMock(IPort.class);
        expect(port.getPortName()).andReturn("PortID");
        replay(port);
        platform.loaderCommunicationError(port, "FATAL_ERROR");
        verify(port);
    }

    /**
     * 
     */
    @Test
    public void testCreateEquipment() {
        platform.createEquipment(TestFragmentActivator.getPort(), null, false);
    }
}
