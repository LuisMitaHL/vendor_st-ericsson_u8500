package com.stericsson.sdk.common.ui.validators;

import java.util.ArrayList;

import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;

/**
 * Section validator. Must be internal class as it works on Selection class data.
 * 
 * @author mielcluk
 * 
 */
public class SectionValidator extends AbstractValidator {

    private static final String SECTION_SECURITY_PROPERTIES = "Security Properties";

    private static final String SEC_PROP_MULTIPLE_NAME_MESSAGE = "Security properties must have different names";

    private static final String SECURITY_PROPERTIES_NAME = "Name";

    private boolean changeOk;

    private Section section;

    private ArrayList<IConfigurationRecord> securityPropertiesDefined;

    /**
     * Validates section
     * 
     * @param pValidatedObject
     *            validated section
     * @param pSecurityPropertiesDefined
     *            defined security properties
     */
    public SectionValidator(Section pValidatedObject, ArrayList<IConfigurationRecord> pSecurityPropertiesDefined) {
        super(pValidatedObject);
        section = pValidatedObject;
        securityPropertiesDefined = pSecurityPropertiesDefined;
    }

    /**
     * {@inheritDoc}
     */
    public void checkCurrentValue() {

        // for Security Properties section
        // Check whether properties does not have same name.
        if (section.getText().equals(SECTION_SECURITY_PROPERTIES)) {

            for (int i = 0; i < securityPropertiesDefined.size(); i++) {
                for (int j = i + 1; j < securityPropertiesDefined.size(); j++) {
                    if (securityPropertiesDefined.get(i).getValue(SECURITY_PROPERTIES_NAME).equals(
                        securityPropertiesDefined.get(j).getValue(SECURITY_PROPERTIES_NAME))) {
                        changeOk = false;
                        notifyValidatorListeneres(SEC_PROP_MULTIPLE_NAME_MESSAGE);
                        return;
                    }
                }
            }
        }
        changeOk = true;
        notifyValidatorListeneres(null);
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueOk() {
        return changeOk;
    }

    /**
     * {@inheritDoc}
     */
    public void switchOff() {
        // TODO
    }

    /**
     * {@inheritDoc}
     */
    public void switchOn() {
        // TODO
    }

}
