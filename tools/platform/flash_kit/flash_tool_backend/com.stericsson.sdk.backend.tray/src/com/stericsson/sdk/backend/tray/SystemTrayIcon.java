package com.stericsson.sdk.backend.tray;

import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_ALIAS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_TRIGGER;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Set;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTError;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Tray;
import org.eclipse.swt.widgets.TrayItem;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.equipment.io.port.IPortProvider;

/**
 * Shows an system tray icon for the flash tool backend if supported by the host operating system.
 * 
 * @author xtomlju
 */
public class SystemTrayIcon implements Runnable, IConfigurationServiceListener {

    /** */
    private static final String DEFAULT = "default";

    /** Logger instance */
    private static Logger logger = Logger.getLogger(SystemTrayIcon.class.getName());

    /** SWT shell for the system tray icon */
    private Shell shell;

    /** */
    private MenuItem itemAcceptClients;

    /** */
    private MenuItem itemAcceptEquipments;

    private Menu uartPortsSubmenu;

    /** */
    private MenuItem itemReloadConfiguration;

    private Display display;

    private MenuItem itemTriggerUartPort;

    private final UartTracker uartTracker = new UartTracker();

    private MenuItem itemExit;

    private TrayItem trayItem;

    private Menu menu;

    /**
     * {@inheritDoc}
     */
    public void run() {
        Image image = null;
        Tray tray = null;

        try {
            display = Display.getDefault();
            shell = new Shell(display);
            tray = display.getSystemTray();
        } catch (SWTError e) {
            logger.warn("System tray not available");
        }

        if (tray != null) {
            logger.debug("Loading system tray icon image");

            image = new Image(display, Activator.getResourcesPath() + "/systrayicon.png");

            trayItem = new TrayItem(tray, SWT.NONE);
            trayItem.setToolTipText("ST-Ericsson Flash Tool Backend");
            trayItem.setImage(image);

            menu = new Menu(shell, SWT.POP_UP);
            createMenuItems();
            new Controler();

            Activator.getBundleContext().registerService(IConfigurationServiceListener.class.getName(), this, null);

            ServiceTracker tracker =
                new ServiceTracker(Activator.getBundleContext(), IPortProvider.class.getName(), uartTracker);
            tracker.open();

            autoTriggerUart();
            while (!shell.isDisposed()) {
                if (!display.readAndDispatch()) {
                    display.sleep();
                }
            }

            logger.debug("Exiting system tray icon shell");

            if (image != null) {
                image.dispose();
            }
            display.close();
            IBackendService service = getBackendService();
            if (service != null) {
                service.shutdown();
            }
        }
    }

    /**
     * Trigger UART if in backend.mesp is set attribute default in UART configuration.
     */
    private void autoTriggerUart() {
        IBackendService service = getBackendService();
        if (service == null) {
            logger.error("No backend service found");
            return;
        }
        IConfigurationService configService = service.getConfigurationService();
        if (configService == null) {
            logger.error("No configuration service found");
            return;
        }

        IConfigurationRecord confRecord = configService.getRecord(PORT_UART);
        if (confRecord != null) {
            String portName = confRecord.getValue(DEFAULT);
            if (portName != null) {
                triggerUart(portName);
            }
        } else {
            logger.error("No UART configuration reccord found");
            return;
        }

    }

    /**
     * @param portName
     *            port to be triggered.
     */
    private void triggerUart(String portName) {
        IBackendService service = getBackendService();
        if (service == null) {
            logger.error("No backend service found");
            return;
        }
        IConfigurationRecord config = new MESPConfigurationRecord();
        config.setName(PORT_UART);
        config.setValue(PORT_UART_ALIAS, portName);
        config.setValue(PORT_UART_TRIGGER, "" + true);
        service.notifyConfigurationChanged(null, config);
    }

    private IBackendService getBackendService() {
        ServiceReference reference = null;

        reference = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());

        if (reference != null) {
            return (IBackendService) Activator.getBundleContext().getService(reference);
        } else {
            return null;
        }
    }

    /**
     * Create the menu items on the pop-up menu associated with the system tray icon.
     */
    private void createMenuItems() {

        itemAcceptClients = new MenuItem(menu, SWT.CHECK);
        itemAcceptClients.setText("Accept Clients");

        new MenuItem(menu, SWT.SEPARATOR);

        itemAcceptEquipments = new MenuItem(menu, SWT.CHECK);
        itemAcceptEquipments.setText("Accept Equipments");

        itemTriggerUartPort = new MenuItem(menu, SWT.CASCADE);
        itemTriggerUartPort.setText("Trigger UART port");
        uartPortsSubmenu = new Menu(shell, SWT.DROP_DOWN);

        itemTriggerUartPort.setMenu(uartPortsSubmenu);

        new MenuItem(menu, SWT.SEPARATOR);

        itemReloadConfiguration = new MenuItem(menu, SWT.PUSH);
        itemReloadConfiguration.setText("Reload Configuration");

        new MenuItem(menu, SWT.SEPARATOR);

        itemExit = new MenuItem(menu, SWT.PUSH);
        itemExit.setText("Exit");
    }

    /**
     * Closes the "hidden" shell and thus removes the system tray icon.
     */
    public void close() {
        if (shell != null) {
            shell.close();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {
        if (newRecord.getName().equalsIgnoreCase("Monitor")) {
            boolean acceptClients = false;
            boolean acceptEquipments = false;

            if ("true".equalsIgnoreCase(newRecord.getValue("acceptClients"))) {
                acceptClients = true;
            }

            if ("true".equalsIgnoreCase(newRecord.getValue("acceptEquipments"))) {
                acceptEquipments = true;
            }

            final boolean finalAcceptClients = acceptClients;
            final boolean finalAcceptEquipments = acceptEquipments;

            display.asyncExec(new Runnable() {
                public void run() {
                    itemAcceptClients.setSelection(finalAcceptClients);
                    itemAcceptEquipments.setSelection(finalAcceptEquipments);
                }
            });
        } else if (newRecord.getName().equalsIgnoreCase(PORT_UART) && newRecord.isValueNameSet(PORT_UART_ALIAS)
            && !newRecord.isValueNameSet(PORT_UART_TRIGGER)) {
            if (newRecord.getValue(PORT_UART_ALIAS).equalsIgnoreCase("auto")) {
                uartTracker.autoPortDetection();
            } else {
                uartTracker.manualPortDetection(newRecord.getValue(PORT_UART_ALIAS));
            }
        }
    }

    /** */
    void dispose() {
        shell.dispose();
    }

    class Controler {
        /**
         * 
         */
        public Controler() {
            itemAcceptClients.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    IBackendService service = getBackendService();
                    if (service == null) {
                        return;
                    }
                    service.setAcceptClients(itemAcceptClients.getSelection());
                }
            });
            itemAcceptEquipments.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    IBackendService service = getBackendService();
                    if (service == null) {
                        return;
                    }
                    service.setAcceptEquipments(itemAcceptEquipments.getSelection());
                }
            });
            itemReloadConfiguration.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    IBackendService service = getBackendService();
                    if (service == null) {
                        return;
                    }
                    try {
                        // TODO it should be handle by error message ???
                        service.loadConfiguration();
                    } catch (NullPointerException e) {
                        logger.warn(e.getMessage());
                    } catch (FileNotFoundException e) {
                        logger.error(e.getMessage());
                    } catch (IOException e) {
                        logger.fatal("Failed to read configuration file", e);
                    }
                }
            });
            itemExit.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    logger.info("Exiting backend from system tray icon");
                    close();
                }
            });
            trayItem.addListener(SWT.MenuDetect, new Listener() {
                public void handleEvent(Event event) {
                    menu.setVisible(true);
                }
            });
        }
    }

    class UartTracker implements ServiceTrackerCustomizer {

        private final Set<String> ports = Collections.synchronizedSet(new HashSet<String>());

        private String manualPortName;

        public Object addingService(ServiceReference reference) {
            if (isUartServiceReference(reference)) {
                IPortProvider portProvider = (IPortProvider) Activator.getBundleContext().getService(reference);
                ports.add(portProvider.getIdentifier());
                refreshMenu();

                return portProvider;
            } else {
                return null;
            }
        }

        public void modifiedService(ServiceReference reference, Object serviceObject) {
        }

        public void removedService(ServiceReference reference, Object serviceObject) {
            if (isUartServiceReference(reference)) {
                ports.remove(serviceObject);
            }
        }

        void autoPortDetection() {
            manualPortName = null;
            refreshMenu();
        }

        void manualPortDetection(String pPortName) {
            manualPortName = pPortName;
            refreshMenu();
        }

        private boolean isUartServiceReference(ServiceReference reference) {
            return (reference.getProperty(IPortProvider.PORT_PROVIDER_NAME) != null && (reference
                .getProperty(IPortProvider.PORT_PROVIDER_NAME).toString().contains("COM")
                || reference.getProperty(IPortProvider.PORT_PROVIDER_NAME).toString().contains("ttyS") || reference
                .getProperty(IPortProvider.PORT_PROVIDER_NAME).toString().contains("ttyUSB")));
        }

        void refreshMenu() {
            display.asyncExec(new Runnable() {
                public void run() {
                    for (MenuItem item : uartPortsSubmenu.getItems()) {
                        item.dispose();
                    }

                    if (manualPortName == null) {
                        // sort first to have an ordered and nice looking list
                        LinkedList<String> sorted = sortPorts();
                        for (final String port : sorted) {
                            addMenuItem(port);
                        }
                    } else {
                        addMenuItem(manualPortName);
                    }
                }
            });
        }

        /**
         * Create sorted list of ports. COM1 is before COM2 but COM2 is before COM11.
         * 
         * @return sorted list of ports
         */
        private LinkedList<String> sortPorts() {
            LinkedList<String> sorted;
            synchronized (ports) {
                sorted = new LinkedList<String>(ports);
            }
            Collections.sort(sorted, new Comparator<String>() {
                public int compare(String pA, String pB) {
                    if (pA.length() != pB.length()) {
                        return pA.length() - pB.length();
                    } else {
                        return pA.compareTo(pB);
                    }
                }
            });
            return sorted;
        }

        private void addMenuItem(final String portName) {
            MenuItem uartPortItem = new MenuItem(uartPortsSubmenu, SWT.PUSH);
            // remove the \\.\ sequence from the COM name for a better look
            uartPortItem.setText(portName); // .replace("\\\\.\\", ""));
            uartPortItem.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event pEvent) {
                    triggerUart(portName);
                    IBackendService service = getBackendService();
                    if (service != null) {
                        IConfigurationService configService = service.getConfigurationService();
                        if (configService != null) {
                            IConfigurationRecord temp = configService.getRecord(PORT_UART);
                            if (temp == null) {
                                logger.error("No configuration record for UART found");
                                return;
                            }
                            if (temp.getValue(DEFAULT) != null) {
                                temp.setValue(DEFAULT, portName);
                            }
                        } else {
                            logger.error("No configuration service found");
                        }
                    } else {
                        logger.error("No backend service found");
                    }
                }
            });
        }
    }
}
