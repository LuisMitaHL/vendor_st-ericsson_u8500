package com.stericsson.sdk.equipment.io.uart.internal;

/**
 * @author rohacmic
 */
final class SerialPortConfiguration {

    public static enum BAUD_RATE {

        // Some seldom used low rates
        BR_110(110), BR_300(300), BR_600(600), BR_1200(1200), BR_2400(2400), BR_4800(4800),

        // More common used rates
        BR_9600(9600), BR_14400(14400), BR_19200(19200), BR_38400(38400),

        BR_56000(56000), BR_57600(57600),

        BR_115200(115200), BR_128000(128000), BR_230400(230400), BR_256000(256000),

        // Some high rates (maybe not supported by controller)
        BR_460800(460800), BR_921600(921600), BR_1152000(1152000);

        private int baudRate;

        private BAUD_RATE(int pBaudRate) {
            this.baudRate = pBaudRate;
        }

        public int getBaudRate() {
            return baudRate;
        }

        public static BAUD_RATE getByString(String br) {
            if (br == null) {
                return null;
            }

            int baudRate;
            try {
                baudRate = Integer.parseInt(br);
            } catch (NumberFormatException ex) {
                return null;
            }

            for (BAUD_RATE enumVal : BAUD_RATE.values()) {
                if (enumVal.getBaudRate() == baudRate) {
                    return enumVal;
                }
            }
            return null;
        }
    };

    public static enum DATA_BITS {
        DATA_BITS_5(5), DATA_BITS_6(6), DATA_BITS_7(7), DATA_BITS_8(8);

        private int dataBits;

        private DATA_BITS(int pDataBits) {
            this.dataBits = pDataBits;
        }

        public int getDataBits() {
            return dataBits;
        }

        public static DATA_BITS getByString(String pValue) {
            if (pValue == null) {
                return null;
            }
            int dataBits;
            try {
                dataBits = Integer.parseInt(pValue);
            } catch (NumberFormatException ex) {
                return null;
            }

            for (DATA_BITS enumVal : DATA_BITS.values()) {
                if (enumVal.getDataBits() == dataBits) {
                    return enumVal;
                }
            }
            return null;
        }
    };

    public static enum PARITY {
        PAR_NONE, PAR_EVEN, PAR_ODD, PAR_MARK, PAR_SPACE;

        public static PARITY getByString(String pValue) {
            if (pValue == null) {
                return null;
            }
            int parity;
            try {
                parity = Integer.parseInt(pValue);
            } catch (NumberFormatException ex) {
                return null;
            }

            for (PARITY enumVal : PARITY.values()) {
                if (enumVal.ordinal() == parity) {
                    return enumVal;
                }
            }
            return null;
        }
    };

    public static enum STOP_BITS {
        STOP_BITS_1, STOP_BITS_1_5, STOP_BITS_2;

        public static STOP_BITS getByString(String pValue) {
            if (pValue == null) {
                return null;
            }
            try {
                switch (Integer.parseInt(pValue)) {
                    case 1:
                        return STOP_BITS_1;
                    case 2:
                        return STOP_BITS_2;
                    default:
                        return null;
                }
            } catch (NumberFormatException ex) {
                return null;
            }
        }
    };

    public static enum ON_OFF {
        DEFAULT, ON, OFF
    };

    public static enum DTR_CIRCUIT {
        DTR_DEFAULT, DTR_ON, DTR_OFF, DTR_HS
    };

    public static enum RTS_CIRCUIT {
        RTS_DEFAULT, RTS_ON, RTS_OFF, RTS_HS, RTS_TG
    };

    // implemented settings
    BAUD_RATE baudRate = BAUD_RATE.BR_9600;

    DATA_BITS dataBits = DATA_BITS.DATA_BITS_8;

    PARITY parity = PARITY.PAR_NONE;

    STOP_BITS stopBits = STOP_BITS.STOP_BITS_1;

    // not implemented settings are set to default and appropriate setter is not provided!
    // implementation is needed in JNI for linux
    ON_OFF timeout = ON_OFF.DEFAULT;

    ON_OFF xOnOff = ON_OFF.DEFAULT;

    ON_OFF dsrHandshake = ON_OFF.DEFAULT;

    ON_OFF ctsHandshake = ON_OFF.DEFAULT;

    DTR_CIRCUIT dtr = DTR_CIRCUIT.DTR_DEFAULT;

    RTS_CIRCUIT rts = RTS_CIRCUIT.RTS_DEFAULT;

    ON_OFF dsr = ON_OFF.DEFAULT;

    public SerialPortConfiguration() {
    }

    SerialPortConfiguration(BAUD_RATE pBaudRate, DATA_BITS pDataBits, PARITY pParity, STOP_BITS pStopBits) {
        this.baudRate = pBaudRate;
        this.dataBits = pDataBits;
        this.parity = pParity;
        this.stopBits = pStopBits;
    }

    void setBaudRate(BAUD_RATE br) {
        this.baudRate = br;
    }

    /**
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        throw new RuntimeException("Method is not implemented.");
    }

    /**
     * @see java.lang.Object#equals(java.lang.Object)
     */
    // CHECK_CYCLOMATICCOMPLEXITY_OFF
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj != null && obj instanceof SerialPortConfiguration) {
            SerialPortConfiguration other = (SerialPortConfiguration) obj;
            return dsrHandshake == other.dsrHandshake && stopBits == other.stopBits && baudRate == other.baudRate
                && dataBits == other.dataBits && parity == other.parity && ctsHandshake == other.ctsHandshake
                && dtr == other.dtr && dsr == other.dsr && rts == other.rts && timeout == other.timeout
                && xOnOff == other.xOnOff;
        } else {
            return false;
        }
    }
    // CHECK_CYCLOMATICCOMPLEXITY_ON
}
