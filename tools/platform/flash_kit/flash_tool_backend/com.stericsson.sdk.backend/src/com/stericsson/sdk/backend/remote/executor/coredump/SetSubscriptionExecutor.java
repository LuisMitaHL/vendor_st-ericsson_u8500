package com.stericsson.sdk.backend.remote.executor.coredump;

import java.net.InetAddress;
import java.net.UnknownHostException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.io.ip.subscription.Notification;
import com.stericsson.sdk.equipment.io.ip.subscription.Subscription;
import com.stericsson.sdk.equipment.io.ip.subscription.SubscriptionNotificationService;

/**
 * Class that executes actions for a SetSubscription command.
 *
 * @author qkarhed
 */
public class SetSubscriptionExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetSubscriptionExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        Subscription subscription = null;
        String message = "";
        String ipAddress = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_IP);
        String port = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_PORT);
        String type = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIPTION_TYPE);
        int notificationInt = 0;
        try {
            notificationInt = Integer.parseInt(type);
        } catch (NumberFormatException e) {
            throw new ExecutionException("The provided subscription type is not a parsable integer.");
        }
        if (notificationInt < Notification.values().length && notificationInt >= 0) {

            try {
                subscription =
                    new Subscription(InetAddress.getByName(ipAddress), Integer.parseInt(port),
                        Notification.values()[notificationInt]);
            } catch (NumberFormatException e) {
                throw new ExecutionException("The provided port number is not a parsable integer ");
            } catch (UnknownHostException e) {
                throw new ExecutionException("The provided ip address is incorrect");
            }

            SubscriptionNotificationService.getInstance().addSubscription(subscription);
            message =
                "Host " + ipAddress + ":" + port + " is now subscribed for notifications of type: "
                    + Notification.values()[notificationInt] + " (" + type + ")";
            log.info(message);

        } else {
            message = "Notification type " + notificationInt + " is unknown.";
        }
        return message;

    }

}
