package com.stericsson.sdk.signing.cli;

import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;

/**
 * The ISignerFactory interface provides methods for creating signer, signer service and signer
 * settings.
 * 
 * @author xtomlju
 */
public interface ISignerFactory {

    /**
     * Create a signer settings object
     * 
     * @param arguments
     *            Any additional arguments needed to the signer settings object
     * @param cmdLineProxy
     *            settings that was set by the commandline.
     * @return An ISignerSettings interface
     * 
     * @throws SignerException
     *             If a signer setting related error occurred
     * @throws ParseException
     *             parseException
     * @throws NullPointerException
     *             nullPointerException
     */
    ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy) throws SignerException,
        NullPointerException, ParseException;

    /**
     * @param type facade type
     * @return the signer facade to use
     */
    ISignerFacade createSignerFacade(String type);
}
