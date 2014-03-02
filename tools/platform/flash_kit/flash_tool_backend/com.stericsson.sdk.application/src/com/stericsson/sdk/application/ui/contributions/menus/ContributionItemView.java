package com.stericsson.sdk.application.ui.contributions.menus;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.action.IContributionItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.CompoundContributionItem;
import org.eclipse.ui.actions.ContributionItemFactory;

/**
 * This class is used to provide the view list.
 * 
 * @author pfurma01
 * 
 */
public class ContributionItemView extends CompoundContributionItem {

    /**
     * Default constructor
     */
    public ContributionItemView() {
    }

    /**
     * Default constructor with ID
     * 
     * @param id
     *            contribution item view ID
     */
    public ContributionItemView(String id) {
        super(id);
    }

    /**
     * It provides the view list.
     * 
     * @return array of the view items
     */
    @Override
    protected IContributionItem[] getContributionItems() {
        List<IContributionItem> menuContributionList = new ArrayList<IContributionItem>();
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IContributionItem item = ContributionItemFactory.VIEWS_SHORTLIST.create(window);
        menuContributionList.add(item); // add the list of views in the menu
        return menuContributionList.toArray(new IContributionItem[menuContributionList.size()]);
    }

}
