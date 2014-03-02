package com.stericsson.sdk.application;

import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;

/**
 * This workbench bar advisor instantiate and propagate menus to RCP application.
 * 
 * @author pfurma01
 * 
 */
public class ApplicationActionBarAdvisor extends ActionBarAdvisor {

    /**
     * Constructor
     * 
     * @param configurer
     *            workbench configuration
     */
    public ApplicationActionBarAdvisor(IActionBarConfigurer configurer) {
        super(configurer);
    }

    /**
     * It will create instance and register menu action.
     * 
     * @param window
     *            workbench window
     */
    protected void makeActions(IWorkbenchWindow window) {
        IWorkbenchAction resetView = ActionFactory.RESET_PERSPECTIVE.create(window);
        IWorkbenchAction help = ActionFactory.DYNAMIC_HELP.create(window);
        IWorkbenchAction helpSearch = ActionFactory.HELP_SEARCH.create(window);
        IWorkbenchAction helpContents = ActionFactory.HELP_CONTENTS.create(window);
        IWorkbenchAction saveAll = ActionFactory.SAVE_ALL.create(window);
        IWorkbenchAction saveAs = ActionFactory.SAVE_AS.create(window);
        register(resetView);
        register(help);
        register(helpSearch);
        register(helpContents);
        register(saveAll);
        register(saveAs);
    }

}
