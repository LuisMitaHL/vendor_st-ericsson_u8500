/**
 * 
 */
package com.stericsson.sdk.common.ui.behaviors;

import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Text;

/**
 * Defines basic behavior for text field containing path.
 * 
 * @author xadazim
 * 
 */
public class SelectAllBehavior implements IBehavior {

    private SelectionListener selectionListener;
    private boolean registered;
    private Text tf;
    private FocusListener focusListener;

    /**
     * @param pTf TODO
     */
    public SelectAllBehavior(Text pTf) {
        tf = pTf;
        registered = false;
    }

    /**
     * {@inheritDoc}
     */
    public void switchOn() {
        if (!registered) {
            selectionListener = new SelectionListener() {
                public void widgetDefaultSelected(SelectionEvent pE) {
                    tf.setSelection(0, tf.getText().length());
                }

                public void widgetSelected(SelectionEvent pE) {
                    tf.setSelection(0, tf.getText().length());
                }
            };
            tf.addSelectionListener(selectionListener);

            focusListener = new FocusListener() {
                public void focusGained(FocusEvent pE) {
                    tf.setSelection(0, tf.getText().length());
                }

                public void focusLost(FocusEvent fe) {
                }
            };
            tf.addFocusListener(focusListener);

            registered = true;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void switchOff() {
        if(registered) {
            tf.removeSelectionListener(selectionListener);
            tf.removeFocusListener(focusListener);
            registered = false;
        }
    }
}
