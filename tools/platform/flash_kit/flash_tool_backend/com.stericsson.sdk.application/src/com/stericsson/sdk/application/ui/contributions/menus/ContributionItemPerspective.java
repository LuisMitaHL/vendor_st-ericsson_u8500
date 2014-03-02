package com.stericsson.sdk.application.ui.contributions.menus;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.action.IContributionItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.CompoundContributionItem;
import org.eclipse.ui.actions.ContributionItemFactory;

/**
 * This class is used to provide the perspective list.
 * 
 * @author pfurma01
 * 
 */
public class ContributionItemPerspective extends CompoundContributionItem {

    /**
     * Default constructor
     */
    public ContributionItemPerspective() {
    }

    /**
     * Default constructor with ID
     * 
     * @param id
     *            contribution item view ID
     */
    public ContributionItemPerspective(String id) {
        super(id);
    }

    /**
     * provide the list of the perspective items
     * 
     * @return array of the perspective items
     */
    protected IContributionItem[] getContributionItems() {
        List<IContributionItem> menuContributionList = new ArrayList<IContributionItem>();
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IContributionItem item = ContributionItemFactory.PERSPECTIVES_SHORTLIST.create(window);
        menuContributionList.add(item); // add the list of views in the menu
        return menuContributionList.toArray(new IContributionItem[menuContributionList.size()]);
    }
}
