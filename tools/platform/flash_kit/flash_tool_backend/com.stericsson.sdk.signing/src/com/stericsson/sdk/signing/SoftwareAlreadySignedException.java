package com.stericsson.sdk.signing;

/**
 * @author xtomzap
 *  Exception thrown if input file, that is going to be sign, is already signed. 
 */
public class SoftwareAlreadySignedException extends SignerException {


        /** Error code for unknown error */
        public static final int UNKNOWN_ERROR = 1;

        private static final long serialVersionUID = 7922684889188075169L;

        private int resultValue;

        /**
         * Constructor
         * 
         * @param message
         *            Exception message
         */
        public SoftwareAlreadySignedException(String message) {
            super(message);
            resultValue = UNKNOWN_ERROR;
        }

        /**
         * @param message
         *            Exception message
         * @param value
         *            Error code value
         */
        public SoftwareAlreadySignedException(String message, int value) {
            super(message);
            resultValue = value;
        }

         /**
         * @return Result value
         */
        public int getResultValue() {
            return resultValue;
        }


}
