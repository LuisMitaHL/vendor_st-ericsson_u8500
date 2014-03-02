package com.stericsson.sdk.common.ui.editors.mesp;

import com.stericsson.sdk.common.ui.EditorUIControl.CONTROL_TYPE;


/**
 * Instance of this class represents generic description of attribute of mesp configuration record. Main aim for 
 * for creating instances of this class is to provide all necessary information needed for generating UI control. 
 * 
 * @author xmicroh
 * 
 */
public class MespAttributeDefiniton {

    private String name;

    private CONTROL_TYPE type;

    private Object[] arguments;

    /**
     * Will construct instance of this class which is later on used for generating appropriate UI control.
     * Optional arguments must be given in order expected by class responsible for constructing of UI control.
     * 
     * @param pName Name of mesp record attribute.
     * @param pType Type of component which will be used for representing attribute.
     * @param pArguments Optional arguments which are later on used when UI control is generated.
     */
    public MespAttributeDefiniton(String pName, CONTROL_TYPE pType, Object... pArguments) {
        name = pName;
        type = pType;
        arguments = pArguments;
    }

    /**
     * @return Name of the attribute.
     */
    public String getName() {
        return name;
    }

    /**
     * @return Type of UI control which should be used for representing attribute.
     */
    public CONTROL_TYPE getType() {
        return type;
    }

    /**
     * @return Optional arguments needed for constructing attribute UI control.
     */
    public Object[] getArguments() {
        return arguments;
    }

}
