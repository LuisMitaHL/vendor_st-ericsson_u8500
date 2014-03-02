package com.stericsson.sdk.common.ui.controls.newdesign;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.forms.widgets.FormToolkit;

/**
 * This factory serves for creating UITextField controls and setting the common L&F to it.
 * 
 * @author xmicroh
 * 
 */
public final class UITextFieldsFactory implements ITextFieldsFactory {

    private static final String OPEN_FILE = "Open file";

    private static final String SAVE_FILE = "Save file";

    private static final String OPEN_DIR = "Open folder";

    private static final String SAVE_DIR = "Save to folder";

    private static UITextFieldsFactory instance;

    private final FormToolkit toolkit;

    /**
     * Will create factory which is capable to generate UI controls. Factory will use for UI control
     * creation given toolkit if it's set.
     * 
     * @param pToolkit
     *            Set toolkit or null depending on how you want to create UI controls.
     * @return Instance of this factory.
     */
    public static UITextFieldsFactory getInstance(FormToolkit pToolkit) {
        if (pToolkit != null) {
            return new UITextFieldsFactory(pToolkit);
        } else if (instance == null) {
            instance = new UITextFieldsFactory(null);
        }
        return instance;
    }

    private UITextFieldsFactory(FormToolkit pToolkit) {
        toolkit = pToolkit;
    }

    private Composite createNewComposite(Composite pParent, int pColumns) {
        Composite c = null;
        if (toolkit != null) {
            c = toolkit.createComposite(pParent);
        } else {
            c = new Composite(pParent, SWT.NONE);
        }
        c.setLayout(new GridLayout(pColumns, false));
        return c;
    }

    private UITextField createTF(Composite pParent, int pStyle, String pName, String pValue, int pSpanColumns) {
        Label tfDescription = null;
        if (toolkit != null) {
            tfDescription = toolkit.createLabel(pParent, pName, SWT.NONE);
        } else {
            tfDescription = new Label(pParent, SWT.NONE);
            tfDescription.setText(pName);
        }
        tfDescription.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

        Text tf = null;
        if (toolkit != null) {
            tf = toolkit.createText(pParent, "", pStyle);
        } else {
            tf = new Text(pParent, pStyle);
        }
        GridData gd = new GridData(SWT.FILL, SWT.CENTER, true, false);
        gd.horizontalIndent = UIControl.HORIZONTAL_INDENT;
        gd.widthHint = UIControl.COMP_MIN_SIZE;
        tf.setLayoutData(gd);
        if (pValue == null) {
            pValue = "";
        }
        tf.setText(pValue);

        if (pSpanColumns > 0) {// will add placeholder
            GridData gd2 = new GridData(SWT.FILL, SWT.FILL, true, false);
            gd2.horizontalSpan = pSpanColumns;
            Label placeHolder = null;
            if (toolkit != null) {
                placeHolder = toolkit.createLabel(pParent, "");
            } else {
                placeHolder = new Label(pParent, SWT.NONE);
            }
            placeHolder.setLayoutData(gd2);
            return new UITextField(tfDescription, tf, placeHolder);
        } else {
            return new UITextField(tfDescription, tf, null);
        }

    }

    private UITextField createPathTF(Composite pParent, int pStyle, final String pName, final String pValue,
        int pSpanColumns, final boolean pDirectory, final boolean pOpen, final String pFilterPath,
        final String[] pFilterExtensions) {

        final UITextField pathTF = createTF(pParent, pStyle, pName, pValue, 0);

        Button bt = null;
        if (toolkit != null) {
            bt = toolkit.createButton(pParent, "", SWT.PUSH);
        } else {
            bt = new org.eclipse.swt.widgets.Button(pParent, SWT.PUSH);
        }
        bt.setText("Browse...");

        bt.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                String dialogTitle;
                if (pDirectory) {
                    dialogTitle = pOpen ? OPEN_DIR : SAVE_DIR;
                } else {
                    dialogTitle = pOpen ? OPEN_FILE : SAVE_FILE;
                }
                String path =
                    openFileDialog(((Button) e.getSource()).getShell(), dialogTitle, pDirectory, pFilterPath,
                        pFilterExtensions, pOpen);
                if (path != null) {
                    pathTF.getUIControl().setText(path);
                }
                super.widgetSelected(e);
            }
        });
        pathTF.setTextFieldButton(bt);

        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, false);
        if (pSpanColumns > 0) {// will add placeholder
            gd.horizontalSpan = pSpanColumns;
            Label placeHolder = null;
            if (toolkit != null) {
                placeHolder = toolkit.createLabel(pParent, "");
            } else {
                placeHolder = new Label(pParent, SWT.NONE);
            }
            placeHolder.setLayoutData(gd);
            pathTF.setPlaceholder(placeHolder);
        }

        return pathTF;
    }

    /**
     * Will open either file or directory dialog for either saving or opening resource according
     * given parameters.
     * 
     * @param pShell
     *            {@link Shell} which will be used for opening dialog.
     * @param pDialogTitle
     *            Title for the dialog.
     * @param pDirectory
     *            True means that resource treated by created dialog is directory, false means file.
     * @param pFilterPath
     *            Initial path for searching/saving resource, null means OS default path.
     * @param pFilterExtensions
     *            In case of file dialog this parameter contains list of extensions to be
     *            opened/saved, null otherwise.
     * @param pOpen
     *            True means that dialog will be used for opening resources, false means saving.
     * @return Path Path selected by the dialog, null in case that dialog was canceled.
     */
    public static String openFileDialog(Shell pShell, String pDialogTitle, boolean pDirectory, String pFilterPath,
        String[] pFilterExtensions, boolean pOpen) {
        if (pDirectory) {
            DirectoryDialog dd = new DirectoryDialog(pShell, SWT.NONE);
            dd.setText(pDialogTitle);
            dd.setFilterPath(pFilterPath);
            return dd.open();
        } else {
            FileDialog fd = new FileDialog(pShell, pOpen ? SWT.OPEN : SWT.SAVE);
            fd.setText(pDialogTitle);
            fd.setFilterPath(pFilterPath);
            fd.setFilterExtensions(pFilterExtensions);
            return fd.open();
        }
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createTextField(Composite pParent, int pStyle, String pName, String pValue, int pSpanColumns) {
        return createTF(pParent, pStyle, pName, pValue, pSpanColumns);
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createTextField(Composite pParent, int pStyle, String pName, String pValue, int pColumns,
        int pSpan) {
        Composite c = createNewComposite(pParent, pColumns);
        UITextField ret = createTF(c, pStyle, pName, pValue, pSpan);
        ret.setParent(c);
        return ret;
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createDirPathTextField(Composite pParent, int pStyle, String pName, String pValue,
        int pSpanColumns, String pFilterPath, boolean pOpen) {
        return createPathTF(pParent, pStyle, pName, pValue, pSpanColumns, true, pOpen, pFilterPath, null);
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createDirPathTextField(Composite pParent, int pStyle, String pName, String pValue, int pColumns,
        int pSpan, String pFilterPath, boolean pOpen) {
        Composite c = createNewComposite(pParent, pColumns);
        UITextField ret = createPathTF(c, pStyle, pName, pValue, pSpan, true, pOpen, pFilterPath, null);
        ret.setParent(c);
        return ret;
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createFilePathTextField(Composite pParent, int pStyle, String pName, String pValue,
        int pSpanColumns, String pFilterPath, String[] pFilterExtensions, boolean pOpen) {
        return createPathTF(pParent, pStyle, pName, pValue, pSpanColumns, false, pOpen, pFilterPath, pFilterExtensions);
    }

    /**
     * {@inheritDoc}
     */
    public UITextField createFilePathTextField(Composite pParent, int pStyle, String pName, String pValue,
        int pColumns, int pSpan, String pFilterPath, String[] pFilterExtensions, boolean pOpen) {
        Composite c = createNewComposite(pParent, pColumns);
        UITextField ret = createPathTF(c, pStyle, pName, pValue, pSpan, true, pOpen, pFilterPath, pFilterExtensions);
        ret.setParent(c);
        return ret;
    }

}
