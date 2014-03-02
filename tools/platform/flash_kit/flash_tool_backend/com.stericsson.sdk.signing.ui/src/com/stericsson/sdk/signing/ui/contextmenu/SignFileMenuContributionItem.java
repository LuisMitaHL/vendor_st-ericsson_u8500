package com.stericsson.sdk.signing.ui.contextmenu;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.eclipse.jface.action.IContributionItem;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.CompoundContributionItem;
import org.eclipse.ui.menus.CommandContributionItem;
import org.eclipse.ui.menus.CommandContributionItemParameter;

import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;


/**
 * 
 * @author MielcLuk
 * 
 */
public class SignFileMenuContributionItem extends CompoundContributionItem {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    protected IContributionItem[] getContributionItems() {
        List<IContributionItem> items = new Vector<IContributionItem>();

        for (String typeName : SignedSoftwareUtils.getPayloadTypeItems()) {
            CommandContributionItemParameter ccip = createParameter(typeName);

            CommandContributionItem item = new CommandContributionItem(ccip);
            if (ccip.visibleEnabled) {
                item.setVisible(true);
                items.add(item);
            }
        }

        return items.toArray(new IContributionItem[items.size()]);
    }

    private CommandContributionItemParameter createParameter(String s) {
        CommandContributionItemParameter ccip =
            new CommandContributionItemParameter(PlatformUI.getWorkbench().getActiveWorkbenchWindow(),
                "com.stericsson.sdk.signing.ui.contextmenu.dynamic1",
                "com.stericsson.sdk.signing.ui.contextmenu.signu5500", CommandContributionItem.STYLE_PUSH);
        ccip.label = s;
        Map<String, String> map = new HashMap<String, String>();
        map.put("com.stericsson.sdk.signing.ui.contextmenu.commandPayloadTypeParameter", s);
        ccip.visibleEnabled = true;
        ccip.parameters = map;
        return ccip;
    }

}
