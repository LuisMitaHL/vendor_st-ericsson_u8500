package com.stericsson.sdk.common.ui.editors.mesp;

import static com.stericsson.sdk.common.configuration.ConfigurationOption.ACTIVE_PROFILE;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR_ACCEPT_CLIENTS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_ALIAS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_BAUDRATE;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_DATABITS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_PARITY;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_STOPBIT;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_USB;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_USB_ENABLED;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PROFILE;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PROFILE_ALIAS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PROFILE_PATH;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorUIControl.CONTROL_TYPE;

/**
 * Factory for generating UI controls based on generic mapping.
 * 
 * @author xmicroh
 *
 */
public final class MespEditorUIControlFactory {

    /**
     * Identify what kind of UI control should be used for editing this attribute.
     * 
     * @author xmicroh
     */
    private static Map<String, List<MespAttributeDefiniton>> configMapping;

    static {
        configMapping = new HashMap<String, List<MespAttributeDefiniton>>();

        List<MespAttributeDefiniton> attributeList = new ArrayList<MespAttributeDefiniton>();
        attributeList.add(new MespAttributeDefiniton(getCamelString(MONITOR_ACCEPT_CLIENTS), CONTROL_TYPE.COMBO, 
            Arrays.asList("true", "false")));
        attributeList.add(new MespAttributeDefiniton(getCamelString(MONITOR_ACCEPT_EQUIPMENTS), CONTROL_TYPE.COMBO,
            Arrays.asList("true", "false")));

        configMapping.put(getCamelString(MONITOR), attributeList);

        attributeList = new ArrayList<MespAttributeDefiniton>();
        attributeList.add(new MespAttributeDefiniton(getCamelString(PROFILE_ALIAS), CONTROL_TYPE.TEXT_FIELD, "[a-zA-Z0-9_]+"));
        attributeList.add(new MespAttributeDefiniton(getCamelString(PROFILE_PATH), CONTROL_TYPE.PATH, "*.prfl"));
        configMapping.put(getCamelString(PROFILE), attributeList);

        attributeList = new ArrayList<MespAttributeDefiniton>();
        attributeList.add(new MespAttributeDefiniton(getCamelString(PROFILE_ALIAS), CONTROL_TYPE.TEXT_FIELD, "[a-zA-Z0-9_]+"));
        configMapping.put(getCamelString(ACTIVE_PROFILE), attributeList);

        attributeList = new ArrayList<MespAttributeDefiniton>();
        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_USB_ENABLED), CONTROL_TYPE.COMBO, 
            Arrays.asList("true", "false")));
        configMapping.put(getCamelString(PORT_USB), attributeList);

        attributeList = new ArrayList<MespAttributeDefiniton>();
        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_UART_ALIAS), CONTROL_TYPE.TEXT_FIELD,
            "^[/a-zA-Z]+[a-zA-Z0-9]+"));
        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_UART_BAUDRATE), CONTROL_TYPE.COMBO,
            Arrays.asList("56000", "57600", "9600", "115200", "128000", "230400", "460800", "921600", "1152000")));
        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_UART_DATABITS), CONTROL_TYPE.COMBO,
            Arrays.asList("5", "6", "7", "8")));

        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_UART_PARITY), CONTROL_TYPE.COMBO,
            Arrays.asList("0 - None", "1 - Even", "2 - Odd", "3 - Mark", "4 - Space"), //values in combo
            Arrays.asList("0", "1", "2", "3", "4"))); //values in mesp record
        attributeList.add(new MespAttributeDefiniton(getCamelString(PORT_UART_STOPBIT), CONTROL_TYPE.COMBO, 
            Arrays.asList("1", "2")));
        configMapping.put(getCamelString(PORT_UART), attributeList);
    }

    private static String getCamelString(String str) {
        StringBuilder sb = new StringBuilder();
        Matcher matcher = Pattern.compile("(\\S)+").matcher(str.toLowerCase(Locale.getDefault()));
        int pos = 0;
        while (matcher.find(pos)) {
            String word = matcher.group();
            sb.append(" " + Character.toUpperCase(word.charAt(0)));
            sb.append(word.substring(1));
            pos = matcher.end();
        }
        return sb.toString().trim();
    }

    private MespEditorUIControlFactory() {} //Utility class - creating instances is not allowed

    /**
     * Generate UI control according given attributes.
     * 
     * @param toolkit Toolkit used for creation of UI controls.
     * @param parent Parent composite.
     * @param record Record which is the UI control related to.
     * @return UI control for MESP Editor representing given configuration record.
     */
    public static EditorUIControl getMespEditorUIControl(FormToolkit toolkit, Composite parent, IConfigurationRecord record) {
        /*List<MespAttributeDefiniton> configDefinition = configMapping.get(getCamelString(record.getName()));
        if (configDefinition != null) {
            return new EditorUIControl(toolkit, parent, configDefinition, record);
        } else {
            return null;
        }*/
        return null;
    }

}
