package com.stericsson.sdk.equipment.ui.contributions.menus;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.CompoundContributionItem;
import org.eclipse.ui.menus.CommandContributionItem;
import org.eclipse.ui.menus.CommandContributionItemParameter;

import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.BackendViewerAdapter;

/**
 * Contributes dynamically to eclipse context menus by creating a menu with currently connected
 * equipment
 * 
 * @author xolabju
 * 
 */
public class ProcessFileMenuContributionItem extends CompoundContributionItem {

    /**
     * {@inheritDoc}
     */
    @Override
    protected IContributionItem[] getContributionItems() {
        List<IContributionItem> items = new Vector<IContributionItem>();
        List<IEquipment> connectedEquipment = BackendViewerAdapter.getInstance().getConnectedEquipment();
        for (IEquipment e : connectedEquipment) {
            CommandContributionItemParameter ccip = createParameter(e);
            if(ccip == null) {
                continue;
            }
            CommandContributionItem item = new CommandContributionItem(ccip);
            if (ccip.visibleEnabled) {
                item.setVisible(true);
                items.add(item);
            }

        }
        return items.toArray(new IContributionItem[items.size()]);
    }

    private CommandContributionItemParameter createParameter(IEquipment e) {
        CommandContributionItemParameter ccip = null;
        if (e != null && e.getStatus() != null && e.getPort() != null) {
            ccip =
                new CommandContributionItemParameter(PlatformUI.getWorkbench().getActiveWorkbenchWindow(),
                    "com.stericsson.sdk.equipment.ui.menucontribution.compoundcontribution",
                    "com.stericsson.sdk.equipment.ui.commands.processfile", CommandContributionItem.STYLE_PUSH);
            ccip.label = e.toString();
            ccip.icon = ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/me.png"));
            ccip.visibleEnabled = isFlashable(e.getStatus().getState());
            Map<String, String> map = new HashMap<String, String>();
            map.put("com.stericsson.sdk.equipment.ui.commands.processfile.equipmentparam", e.getPort().getPortName());
            ccip.parameters = map;
        }
        return ccip;
    }

    private boolean isFlashable(EquipmentState state) {
        switch (state) {
            case ERROR:
            case LC_NOT_INITIALIZED:
            case BOOTING:
                return false;
            default:
                return true;
        }
    }
}
