package com.stericsson.sdk.common.ui.wizards;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;

import com.stericsson.sdk.common.ui.decorators.ControlDecorator;

/**
 * Basic wizard page providing unified messaging.
 * 
 * @author xmicroh
 * 
 */
public abstract class AbstractWizardPage extends WizardPage {

    /**
     * @author xmicroh
     * 
     */
    public enum MESSAGE_TYPE { // ordered by severity
        /** */
        INFO(IStatus.INFO), WARNING(IStatus.WARNING), ERROR(IStatus.ERROR);

        final int severity;

        private MESSAGE_TYPE(int pSeverity) {
            severity = pSeverity;
        }
    }

    private Map<Object, IStatus> messages;

    private Map<Control, ControlDecorator> decorators;

    /**
     * @param pageName
     *            Name of this page.
     */
    public AbstractWizardPage(String pageName) {
        super(pageName);
        messages = new ConcurrentHashMap<Object, IStatus>();
        decorators = new ConcurrentHashMap<Control, ControlDecorator>();
        setTitle(pageName);
    }

    /**
     * Will add new message based on given key and show most severe messages. Adding messages with
     * the same key will cause their rewriting.
     * 
     * @param key
     *            Key object for the given message.
     * @param pMessage
     *            Message text.
     * @param msgType
     *            Type of message specifying message severity.
     */
    public void addMessage(Object key, String pMessage, MESSAGE_TYPE msgType) {
        IStatus message = new Status(msgType.severity, "no_name", pMessage);
        messages.put(key, message);
        ControlDecorator cd = null;
        if (decorators.get(key) != null) {// decorator already exists
            cd = decorators.get(key);
        } else if (key instanceof Control) {
            cd = new ControlDecorator(getControl().getParent(), (Control) key, SWT.LEFT);
            decorators.put((Control) key, cd);
        } else {
            return; // nothing todo
        }
        if (msgType != MESSAGE_TYPE.INFO) {
            changeWindowStatus(pMessage, msgType);
        }

        cd.showMessage(message);
    }

    private void changeWindowStatus(String pMessage, MESSAGE_TYPE msgType) {
        if (msgType == MESSAGE_TYPE.ERROR) {
            setErrorMessage(pMessage); // standard set message does not work for errors
        } else {
            setMessage(pMessage, msgType.severity);
        }
    }

    /**
     * Remove message couple with given key and from the remaining messages will show the most
     * severe ones.
     * 
     * @param key
     *            Key
     */
    public void removeMessage(Object key) {
        clearMessages();
        messages.remove(key);

        if (decorators.get(key) != null) {
            ControlDecorator cd = decorators.get(key);
            cd.removeMessage();
            cd.dispose();
            decorators.remove(key);
        }
    }

    private void clearMessages() {
        setErrorMessage(null); // every severenity level must be handled separately
        setMessage(null, WARNING);
        setMessage(null, INFORMATION);
    }

    /**
     * Will remove all messages.
     */
    public void removeAllMessages() {
        for (Object key : messages.keySet()) {
            removeMessage(key);
        }
    }

    // private void showMostSevere() {
    // //using LinkedHashMap to sort messages by severity by default
    // Map<Integer, List<String>> messagesBySeverity = new LinkedHashMap<Integer, List<String>>();
    // for (IStatus s : messages.values()) {
    // if (messagesBySeverity.get(s.getSeverity()) == null) {
    // List<String> msgs = new ArrayList<String>();
    // msgs.add(s.getMessage());
    // messagesBySeverity.put(s.getSeverity(), msgs);
    // } else {
    // messagesBySeverity.get(s.getSeverity()).add(s.getMessage());
    // }
    // }
    //
    // List<Integer> severities = new ArrayList<Integer>(messagesBySeverity.keySet());
    // if (severities.size() > 0) {
    // int highestSeverity = severities.get(severities.size() - 1);
    // applyToStatusLine(highestSeverity, messagesBySeverity.get(highestSeverity));
    // } else {
    // applyToStatusLine(IStatus.OK, null);
    // }
    // }
    //
    // private void applyToStatusLine(int severity, List<String> msgs) {
    //
    // switch (severity) {
    // case IStatus.OK:
    // setErrorMessage(null);
    // setMessage(null);
    // break;
    // case IStatus.WARNING:
    // setErrorMessage(null);
    // if (msgs != null) {
    // if (msgs.size() == 1) {
    // setMessage(msgs.get(0), WizardPage.WARNING);
    // } else {
    // setMessage(msgs.size() + " warnings", WizardPage.WARNING);
    // }
    // }
    // break;
    // case IStatus.INFO:
    // setErrorMessage(null);
    // if (msgs != null) {
    // if (msgs.size() == 1) {
    // setMessage(msgs.get(0), WizardPage.INFORMATION);
    // } else {
    // setMessage(msgs.size() + " info messages", WizardPage.INFORMATION);
    // }
    // }
    // break;
    // default:
    // if (msgs != null) {
    // if (msgs.size() == 1) {
    // setErrorMessage(msgs.get(0));
    // } else {
    // setErrorMessage(msgs.size() + " errors");
    // }
    // }
    // setMessage(null);
    // break;
    // }
    // }
}
