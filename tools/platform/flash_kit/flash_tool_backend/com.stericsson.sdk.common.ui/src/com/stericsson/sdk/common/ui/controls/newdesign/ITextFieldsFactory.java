package com.stericsson.sdk.common.ui.controls.newdesign;

import org.eclipse.swt.widgets.Composite;

/**
 * <p>
 * This factory automates creation of text fields based components which are considered to be always
 * layout using {@link GridLayout}. All methods for creating text fields could be divided into
 * several categories depending on where all components are created, if should use place-holder at
 * the end and how many columns should be span by place-holder.
 * </p>
 * 
 * <p>
 * Created components depending on used method could be placed into parent directly or into newly
 * created composite. If new composite is created methods provide way to set number of columns for
 * grid layout.
 * </p>
 * 
 * <p>
 * Methods for creating text fields containing paths could be divided according type of path and its
 * directions. By other words type of path here means either file or directory and direction could
 * be input or output.
 * </p>
 * 
 * <p>
 * <b>Prerequisites & Constraints:</b>
 * </p>
 * <ul>
 * <li>When components should be created directly in given parent, parent must use
 * {@link GridLayout}.</li>
 * <li>When components are created in new composite inside parent than parent can use any layout.
 * However GridLayout will be used automatically in newly created composite.</li>
 * <li>Every method returns text field object which is usually needed for implementing some other
 * functionality, like applying custom layout or do validation, decoration etc.
 * </ul>
 * 
 * @author xmicroh
 * 
 */
public interface ITextFieldsFactory {

    /**
     * Will create three components in parent - {@link Label} with TF name, {@link Text}
     * representing TF and place-holder at the end with custom span for place-holder.
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param spanColumns
     *            Number of columns to span for place-holder.
     * @return Actual text field object.
     */
    UITextField createTextField(Composite parent, int style, String name, String value, int spanColumns);

    /**
     * Will create three components in newly created composite in parent - {@link Label} with TF
     * name, {@link Text} representing TF and place-holder with custom span for place-holder. Newly
     * created composite will be set to use {@link GridLayout} with given number of columns.
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param columns
     *            Number of columns in GridLayout for newly created composite.
     * @param span
     *            Number of columns to span for place-holder.
     * @return Actual text field object.
     */
    UITextField createTextField(Composite parent, int style, String name, String value, int columns, int span);

    /**
     * Will create four components in parent - {@link Label} with TF name, non editable {@link Text}
     * representing TF, button for handling path selection and place-holder at the end with custom
     * span for place-holder.
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param spanColumns
     *            Number of columns to span for place-holder.
     * @param filterPath
     *            Path where searching should started or null when OS default should be used.
     * @param open
     *            True means that path serves as input path, use false for output paths.
     * @return Actual text field object.
     */
    UITextField createDirPathTextField(Composite parent, int style, String name, String value, int spanColumns,
        String filterPath, boolean open);

    /**
     * Will create four components in newly created composite in parent - {@link Label} with TF
     * name, non editable {@link Text} representing TF, button for handling path selection and
     * place-holder at the end with custom span for place-holder. Newly created composite will be
     * set to use {@link GridLayout} with given number of columns.
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param columns
     *            Number of columns in GridLayout for newly created composite.
     * @param span
     *            Number of columns to span for place-holder.
     * @param filterPath
     *            Path where searching should started or null when OS default should be used.
     * @param open
     *            True means that path serves as input path, use false for output paths.
     * @return Actual text field object.
     */
    UITextField createDirPathTextField(Composite parent, int style, String name, String value, int columns, int span,
        String filterPath, boolean open);

    /**
     * Will create four components in parent - {@link Label} with TF name, non editable {@link Text}
     * representing TF, button for handling path selection and place-holder at the end with custom
     * span for place-holder.
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param spanColumns
     *            Number of columns to span for place-holder.
     * @param filterPath
     *            Path where searching should started or null when OS default should be used.
     * @param filterExtensions
     *            File extensions to be filtered.
     * @param open
     *            True means that path serves as input path, use false for output paths.
     * @return Actual text field object.
     */
    UITextField createFilePathTextField(Composite parent, int style, String name, String value, int spanColumns,
        String filterPath, String[] filterExtensions, boolean open);

    /**
     * Will create four components in newly created compsite in parent - {@link Label} with TF name,
     * non editable {@link Text} representing TF, button for handling path selection and
     * place-holder at the end with custom span. Newly created composite will be set to use
     * {@link GridLayout} with given number of columns
     * 
     * @param parent
     *            Parent composite which text field components should be created in.
     * @param style
     *            style of the component
     * @param name
     *            Name of the text field
     * @param value
     *            value of text field or empty string
     * @param columns
     *            Number of columns in GridLayout for newly created composite.
     * @param span
     *            Number of columns to span for place-holder.
     * @param filterPath
     *            Path where searching should started or null when OS default should be used.
     * @param filterExtensions
     *            File extensions to be filtered.
     * @param open
     *            True means that path serves as input path, use false for output paths.
     * @return Actual text field object.
     */
    UITextField createFilePathTextField(Composite parent, int style, String name, String value, int columns, int span,
        String filterPath, String[] filterExtensions, boolean open);
}
