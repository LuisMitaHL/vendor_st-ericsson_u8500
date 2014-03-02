/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Validator;

import java.io.File;
import java.io.IOException;

import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

/**
 * This Class is responsible for validating xml files describing module tocs.
 * 
 * @author Thomas Palmqvist
 * 
 */
public class XSDValidator {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(XSDValidator.class);

    /** The object which validates the XML files */
    private Validator validator;

    /** Use the W3C XML Schema language */
    private static String schemaLanguage = "http://www.w3.org/2001/XMLSchema";

    /**
     * Specialized error handler class that updates the logs and validates the
     * entire xml file. Standard validator stops after the first error. This
     * error handler stops only after a fatal error.
     */
    class RefManErrorHandler implements ErrorHandler {

        /** error counter */
        private int errorCount = 0;

        /** warning counter */
        private int warningCount = 0;

        /** file name (for log printouts) */
        private String fileName;

        /**
         * Constructor - requires filename to enable meaningful printouts
         * 
         * @param filePath
         *            The file path to the xml file that will be validated.
         */
        public RefManErrorHandler(String filePath) {
            super();
            fileName = filePath;
        }

        /**
         * Overwrite of corresponding error handler method. Added error logging.
         */
        public void fatalError(SAXParseException e) throws SAXException {
            logger.error("Fatal validation error (" + fileName + "). Error: "
                    + e.getMessage());
            errorCount++;
            throw e;
        }

        /**
         * Overwrite of corresponding error handler method. Added error logging.
         * Unlike overwritten method this method does not throw exception.
         * Because of that the validation does not stop after the first error is
         * found.
         */
        public void error(SAXParseException e) throws SAXException {
            logger.error("Validation error (" + fileName + "). Error: "
                    + e.getMessage());
            errorCount++;
            // continue with validation process, do not throw exception
        }

        /**
         * Overwrite of corresponding error handler method. Added error logging.
         */
        public void warning(SAXParseException e) throws SAXException {
            warningCount++;
            logger.warn("Validation warning (" + fileName + "). Warning: "
                    + e.getMessage());
        }

        /**
         * Method use to check if validation succeeded without errors.
         * 
         * @return false if errors were found, true otherwise
         */
        public boolean noErrors() {
            return (errorCount == 0);
        }

        /**
         * Method use to check if validation succeeded without warnings.
         * 
         * @return false if errors were found, true otherwise
         */
        public boolean noWarnings() {
            return (warningCount == 0);
        }

        /**
         * Method returns the info about the number of errors found in the
         * validation process.
         * 
         * @return number of errors found in validation
         */
        public int getErrorCount() {
            return errorCount;
        }

        /**
         * Method returns the info about the number of warnings found in the
         * validation process.
         * 
         * @return number of warnings found in validation
         */
        public int getWarningCount() {
            return errorCount;
        }

    }

    /**
     * Creates an <code>XSDValidator</code> that is used to validate an
     * <code>XML</code> files against the <code>XSDSchema</code> provided as
     * parameter.
     * 
     * @param XSDSchema
     *            The schema that will be used to validate <code>XML</code>
     *            files.
     * @throws SAXException
     *             If a <code>SAX</code> error occurs during parsing of the
     *             schema.
     */
    public XSDValidator(File XSDSchema) throws SAXException {
        // Lookup a factory for the Schema language
        SchemaFactory factory = SchemaFactory.newInstance(schemaLanguage);

        try {
            // Compile the schema.
            Schema schema = factory.newSchema(XSDSchema);
            // Get a validator from the schema.
            validator = schema.newValidator();
        } catch (SAXException e) {
            throw e;
        }

    }

    /**
     * @param xmlFilePath
     *            The file path to the xml file that will be validated.
     * @return True if the xml file was valid according to the schema.
     * @throws SAXException
     *             If the validator's underlying ErrorHandler throws a
     *             SAXException or if a fatal error is found.
     * @throws IOException
     *             If the validator is processing a
     *             javax.xml.transform.sax.SAXSource and the underlying
     *             org.xml.sax.XMLReader throws an IOException.
     */

    public boolean validate(String xmlFilePath) throws IOException,
            SAXException {

        // Parse the xml file that will be validated.
        Source source = new StreamSource(xmlFilePath);
        // Result result = new StreamResult(System.out);
        RefManErrorHandler errHandler = new RefManErrorHandler(xmlFilePath);
        validator.setErrorHandler(errHandler);
        try {
            // Validate the document
            validator.validate(source);
        } catch (SAXException ex) {
            /** fatal error case */
            logger
                    .error("Validation of "
                            + xmlFilePath
                            + " failed with a fatal error. Total number of errors found: "
                            + Integer.toString(errHandler.getErrorCount())
                            + ". See log for details");
            return false;
        } catch (IOException e) {
            throw e;
        }

        /** return validation status */
        if (errHandler.noErrors()) {
            if (errHandler.noWarnings()) {
                logger.debug("Validation of " + xmlFilePath + " succeded.");
            } else {
                logger
                        .debug("Validation of "
                                + xmlFilePath
                                + " succeded but there are some warnings. Number of warnings: "
                                + errHandler.getWarningCount()
                                + ". See log for details.");
            }
            return true;

        } else {
            logger.error("Validation of " + xmlFilePath
                    + " failed. Total number of errors found: "
                    + Integer.toString(errHandler.getErrorCount())
                    + ". See log for details.");
            return false;
        }
    }
}
