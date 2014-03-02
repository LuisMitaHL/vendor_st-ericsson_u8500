package com.stericsson.sdk.equipment.ui.perspective;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.eclipse.ui.console.IConsoleConstants;

/**
 * 
 * @author xolabju
 * 
 */
public class FlashKitPerspectiveFactory implements IPerspectiveFactory {

    private static final String FLASHKIT_PERSPECTIVE = "com.stericsson.sdk.equipment.ui.perspective";

    // private static final String EEDITOR_ID =
    // "com.stericsson.sdk.equipment.ui.editors.EquipmentEditor";

    private static final String EQUIPMENT_VIEW_ID = "com.stericsson.sdk.equipment.ui.views.EquipmentView";

    private static final String NAVIGATOR_ID = "org.eclipse.ui.views.ResourceNavigator";

    private static final String PROGRESS_VIEW_ID = "org.eclipse.ui.views.ProgressView";

    private static final String LEFT_TOP = "leftTop";

    private static final String BOTTOM = "bottom";

    private static final String LEFT_BOTTOM = "leftBottom";

    // private static final String LEFT_MIDDLE = "leftMiddle";

    /**
     * {@inheritDoc}
     */
    public void createInitialLayout(IPageLayout layout) {
        defineLayout(layout);
        layout.addPerspectiveShortcut(FLASHKIT_PERSPECTIVE);
        layout.addShowViewShortcut(EQUIPMENT_VIEW_ID);
        layout.addShowViewShortcut(NAVIGATOR_ID);
        layout.addShowViewShortcut(PROGRESS_VIEW_ID);
        layout.addShowViewShortcut(IConsoleConstants.ID_CONSOLE_VIEW);

        layout.setEditorAreaVisible(true);

    }

    private void defineLayout(IPageLayout layout) {
        String editorArea = layout.getEditorArea();

        // layout.createFolder(folderId, relationship, ratio, refId)
        IFolderLayout leftTop = layout.createFolder(LEFT_TOP, IPageLayout.LEFT, 0.30f, editorArea);
        IFolderLayout leftBottom = layout.createFolder(LEFT_BOTTOM, IPageLayout.BOTTOM, 0.5f, LEFT_TOP);
        IFolderLayout bottom = layout.createFolder(BOTTOM, IPageLayout.BOTTOM, 0.70f, editorArea);

        leftTop.addView(NAVIGATOR_ID);

        leftBottom.addView(EQUIPMENT_VIEW_ID);

        // layout.addView(EEDITOR_ID, IPageLayout.RIGHT, 0.70f, editorArea);
        bottom.addView(PROGRESS_VIEW_ID);
        bottom.addView(IConsoleConstants.ID_CONSOLE_VIEW);
    }
}
