package com.stericsson.sdk.backend.remote.executor.backend;

import static org.easymock.EasyMock.expect;
import static org.easymock.EasyMock.replay;
import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.junit.Test;

import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentStatus;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IPlatform;


/**
 * @author Xxvs0002
 *
 */
public class GetNextConnectedEquipmentExecutorTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testGetNextConnectedEquipment(){
        GetNextConnectedEquipmentExecutor connectedEquipmentExecutor = new GetNextConnectedEquipmentExecutor();
        try {
            connectedEquipmentExecutor.execute();
            Thread.sleep(1000);
            IEquipmentProfile equipmentProfile = EasyMock.createMock(IEquipmentProfile.class);
            expect(equipmentProfile.getAlias()).andReturn("TEST_ALIAS");
            replay(equipmentProfile);

            EquipmentStatus equipmentStatus = new EquipmentStatus(EquipmentState.BOOTING, "Booting");

            IEquipment equipment = EasyMock.createMock(IEquipment.class);
            expect(equipment.getStatus()).andReturn(equipmentStatus).atLeastOnce();
            expect(equipment.getProfile()).andReturn(equipmentProfile);

            EquipmentProperty equipmentPlatformProperty = new 
                    EquipmentProperty(IPlatform.PLATFORM_PROPERTY, "TEST_PLATFORM", "TEST_NUMBER_PLATFORM", false);
            EquipmentProperty equipmentPortProperty = new 
                    EquipmentProperty(IPlatform.PORT_PROPERTY, "TEST_PORT", "USB0", false);
            expect(equipment.getProperty(IPlatform.PLATFORM_PROPERTY)).andReturn(equipmentPlatformProperty);
            expect(equipment.getProperty(IPlatform.PORT_PROPERTY)).andReturn(equipmentPortProperty);
            replay(equipment);

            connectedEquipmentExecutor.newEquipmentBooted(equipment);
        } catch (ExecutionException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testGetNextConnectedEquipmentErrorState(){
        GetNextConnectedEquipmentExecutor connectedEquipmentExecutor = new GetNextConnectedEquipmentExecutor();
        try {
            connectedEquipmentExecutor.execute();
            Thread.sleep(1000);

            EquipmentStatus equipmentStatus = new EquipmentStatus(EquipmentState.ERROR, "Error");

            IEquipment equipment = EasyMock.createMock(IEquipment.class);
            expect(equipment.getStatus()).andReturn(equipmentStatus).atLeastOnce();

            replay(equipment);

            connectedEquipmentExecutor.newEquipmentBooted(equipment);
        } catch (ExecutionException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
