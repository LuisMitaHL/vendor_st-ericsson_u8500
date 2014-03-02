package com.stericsson.sdk.assembling.internal;

import java.util.LinkedList;
import java.util.List;

/**
 * @author ezaptom
 *
 */
public class MESPConfigurationRecord {

    private String recordName;

    private List<Value> recordValues;

    /**
     * @param line
     *            Line to parse
     * @return MESPConfigurationRecord instance
     * @throws MESPConfigurationException
     *             If a MESP configuration related error occurred
     */
    public static MESPConfigurationRecord parse(String line) throws MESPConfigurationException {

        MESPConfigurationRecord record = new MESPConfigurationRecord();
        boolean result = true;
        String recordValueName;
        String recordValue;

        MESPConfigurationTokenizer tokenizer = new MESPConfigurationTokenizer(line);

        if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_IDENTIFIER) {
            throw new MESPConfigurationException("Configuration line must start with a name");
        }

        record.setName(tokenizer.getValue());
        tokenizer.next();

        while (result) {

            boolean arrayValueExpected = false;

            if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_IDENTIFIER) {
                throw new MESPConfigurationException("Missing value name");
            }

            recordValueName = tokenizer.getValue();
            tokenizer.next();

            if (recordValueName.endsWith("[]")) {
                arrayValueExpected = true;
            }

            if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_EQUALS) {
                throw new MESPConfigurationException("Missing '=' delimeter");
            }

            tokenizer.next();

            if (arrayValueExpected) {

                parseArrayValue(tokenizer, record, recordValueName);

            } else {
                if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_IDENTIFIER) {
                    throw new MESPConfigurationException("Missing value");
                }

                recordValue = tokenizer.getValue();
                record.setValue(recordValueName, recordValue);

            }

            tokenizer.next();

            if (tokenizer.getToken() == MESPConfigurationTokenizer.TOKEN_EOI) {
                result = false;
            }
        }

        return record;
    }

    /**
     * Constructor
     */
    public MESPConfigurationRecord() {
        recordValues = new LinkedList<Value>();
    }

    // /**
    // * Copy-constructor
    // *
    // * @param record
    // * Record to copy
    // */
    // public MESPConfigurationRecord(MESPConfigurationRecord record) {
    // recordName = new String(record.recordName);
    // recordValues = new LinkedList<Value>();
    // for (MESPConfigurationRecord.Value value : record.recordValues) {
    // recordValues.add(new Value(value));
    // }
    // }

    /**
     * @param name
     *            Record name
     */
    public void setName(String name) {
        recordName = name;
    }

    /**
     * @return Records name
     */
    public String getName() {
        return recordName;
    }

    /**
     * @param name
     *            Value name
     * @param value
     *            Value
     */
    public void setValue(String name, String value) {

        Value recordValue = null;

        for (Value v : recordValues) {
            if (v.getName().equalsIgnoreCase(name)) {
                recordValue = v;
                break;
            }
        }

        if (recordValue == null) {
            recordValue = new Value();
            recordValue.setName(name);
            recordValues.add(recordValue);
        }

        recordValue.setValue(value);

    }

    /**
     * @param name
     *            Value name
     * @return Value for specified name
     */
    public String getValue(String name) {

        String value = null;

        for (Value v : recordValues) {
            if (v.getName().equalsIgnoreCase(name)) {
                value = v.getValue();
                break;
            }
        }

        return value;
    }

    /**
     * {@inheritDoc}
     */
    public void setArrayValue(String valueName, String[] values) {
        Value recordValue = null;

        for (Value v : recordValues) {
            if (v.getName().equalsIgnoreCase(valueName)) {
                recordValue = v;
                break;
            }
        }

        if (recordValue == null) {
            recordValue = new Value();
            recordValue.setName(valueName);
            recordValues.add(recordValue);
        }

        recordValue.setArrayValue(values);
    }

    /**
     * @param name
     *            Value name
     * @return Value for specified name
     */
    public String[] getArrayValue(String name) {

        String[] value = null;

        for (Value v : recordValues) {
            if (v.getName().equalsIgnoreCase(name)) {
                value = v.getArrayValue();
                break;
            }
        }

        return value;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    /**
     * @return TBD
     */
    @Override
    public String toString() {

        StringBuffer buffer = new StringBuffer();

        buffer.append(recordName);

        for (Value value : recordValues) {
            buffer.append(' ');
            buffer.append(value.getName());
            if (value.isArrayValue()) {
                buffer.append("=");
                buffer.append(stringArrayAsString(value.getArrayValue()));
            } else {
                buffer.append("=\"");
                buffer.append(value.getValue());
                buffer.append("\"");
            }
        }

        return buffer.toString();
    }

    private String stringArrayAsString(String[] a) {
        if (a == null) {
            return "null";
        }
        int iMax = a.length - 1;
        if (iMax == -1) {
            return "{}";
        }

        StringBuilder b = new StringBuilder();
        b.append('{');
        for (int i = 0;; i++) {
            b.append("\"");
            b.append(String.valueOf(a[i]));
            b.append("\"");
            if (i == iMax) {
                return b.append('}').toString();
            }
            b.append(" ");
        }
    }

    /**
     * A name value pair. The value part can be an array of values.
     * 
     * @author xtomlju
     */
    private class Value implements Cloneable {

        /** Name of pair */
        private String name;

        /** Value if single value */
        private String singleValue;

        /** Value if array value */
        private String[] arrayValue;

        /**
         * Constructor
         */
        public Value() {
        }

        /**
         * Protected constructor used for cloning. (Copy constructor).
         * 
         * @param value
         *            Value to copy
         */
        public Value(Value value) {
            name = new String(value.name);
            if (value.singleValue != null) {
                singleValue = new String(value.singleValue);
            }
            if (value.arrayValue != null) {
                arrayValue = value.arrayValue.clone();
            }
        }

        /**
         * Set name
         * 
         * @param n
         *            Name
         */
        public void setName(String n) {
            name = n;
        }

        /**
         * Check if array value.
         * 
         * @return True if array value
         */
        public boolean isArrayValue() {
            return (arrayValue != null);
        }

        /**
         * Get name.
         * 
         * @return Name
         */
        public String getName() {
            return name;
        }

        /**
         * Set single value.
         * 
         * @param v
         *            Value
         */
        public void setValue(String v) {
            singleValue = v;
        }

        /**
         * Set array value.
         * 
         * @param value
         *            Array of values.
         */
        public void setArrayValue(String[] value) {
            arrayValue = value;
        }

        /**
         * Get single value.
         * 
         * @return Value
         */
        public String getValue() {
            return singleValue;
        }

        /**
         * Get array value.
         * 
         * @return Array of values
         */
        public String[] getArrayValue() {
            return arrayValue;
        }
    }

    /**
     * Parse array value for a MESP configuration record.
     * 
     * @param tokenizer
     *            Tokenizer object
     * @param record
     *            Configuration record
     * @param recordValueName
     *            Value name
     * @throws MESPConfigurationException
     *             If an MESP related parse error occurred
     */
    protected static void parseArrayValue(MESPConfigurationTokenizer tokenizer, MESPConfigurationRecord record,
        String recordValueName) throws MESPConfigurationException {
        LinkedList<String> values = new LinkedList<String>();

        if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_LEFT_BRACE) {
            throw new MESPConfigurationException("Missing '{' delimeter");
        }

        tokenizer.next();

        boolean done = false;

        while (!done) {
            if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_IDENTIFIER) {
                throw new MESPConfigurationException("Missing identifier");
            }

            values.add(tokenizer.getValue());
            tokenizer.next();

            if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_IDENTIFIER) {
                done = true;
            }
        }

        if (tokenizer.getToken() != MESPConfigurationTokenizer.TOKEN_RIGHT_BRACE) {
            throw new MESPConfigurationException("Missing '}' delimeter");
        }

        record.setArrayValue(recordValueName, values.toArray(new String[0]));
    }

    /**
     * {@inheritDoc}
     */
    public MESPConfigurationRecord deepCopy() {
        MESPConfigurationRecord copy = new MESPConfigurationRecord();
        copy.recordName = new String(recordName);

        for (Value value : recordValues) {
            copy.recordValues.add(new Value(value));
        }

        return copy;
    }

    /**
     * @return new instance of MESPConfigurationRecord
     */
    public static MESPConfigurationRecord newRecord() {
        return new MESPConfigurationRecord();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueNameSet(String pValueName) {
        if (pValueName == null) {
            return false;
        }

        for (Value value : recordValues) {
            if (value.getName().equalsIgnoreCase(pValueName)) {
                return true;
            }
        }

        return false;
    }
}