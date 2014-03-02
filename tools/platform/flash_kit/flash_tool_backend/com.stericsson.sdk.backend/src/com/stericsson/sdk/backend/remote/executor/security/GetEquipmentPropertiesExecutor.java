package com.stericsson.sdk.backend.remote.executor.security;

import java.util.Enumeration;
import java.util.Hashtable;

import com.stericsson.sdk.backend.remote.executor.LCCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentProperty;

/**
 * Executor for getting the ME properties
 * 
 * @author xolabju
 * @author ezaptom
 * @author cizovhel
 * 
 */
public class GetEquipmentPropertiesExecutor extends LCCommandExecutor {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        String resultMessage = "";
        try {
            resultMessage = super.execute();
        } catch (ExecutionException e) {
            // Properties gained during boot should be displayed even when the device does not
            // support gaining its properties during run.
            if (getExecutedTask() != null) {
                throw e;
            }
        }

        resultMessage = createResultMessage(getEquipment().getProperties(), resultMessage);

        return resultMessage;
    }

    private String createResultMessage(Hashtable<String, EquipmentProperty> properties, String taskResultMessage) {
        String message = taskResultMessage;

        // There must be present more properties than just platform and port.
        if (properties.size() > 2) {
            StringBuffer buffer = new StringBuffer();
            Enumeration<String> keys = properties.keys();
            while (keys.hasMoreElements()) {
                String key = keys.nextElement();
                EquipmentProperty property = properties.get(key);
                buffer.append(property.getPropertyName() + "<newcolumn>" + property.getPropertyValue() + "<newcolumn>"
                    + property.isSecurity() + ";");
            }
            message = buffer.toString();
        }

        return message;
    }

}
