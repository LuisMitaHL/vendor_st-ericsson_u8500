package com.stericsson.sdk.backend.remote.executor.coredump;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.io.ip.subscription.Subscription;
import com.stericsson.sdk.equipment.io.ip.subscription.SubscriptionNotificationService;

/**
 * Class that executes actions for a GetSubscription command.
 *
 * @author qkarhed
 */
public class GetSubscriptionExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetSubscriptionExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        List<Subscription> subscriptions = null;
        String ipAddress = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_IP);
        String port = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_PORT);

        try {
            subscriptions =
                SubscriptionNotificationService.getInstance().getSubscriptionsForSubscriber(
                    InetAddress.getByName(ipAddress), Integer.parseInt(port));
        } catch (UnknownHostException e) {
            throw new ExecutionException("Ipaddress or portnumber incorrect");
        } catch (NumberFormatException e) {
            throw new ExecutionException("Ipaddress or portnumber incorrect");
        }

        String hostString = ipAddress.concat(":").concat(port);
        String message = ("No subscriptions for host:").concat(hostString);

        if (subscriptions != null && subscriptions.size() > 0) {
            StringBuilder sb = new StringBuilder();
            sb.append("\nNotification types: 0 = DETECT_DUMPING_ME, 1 = AUTOMATIC_COREDUMP_DOWNLOADED\n");
            sb.append("Current subscriptions for host:").append(hostString).append(":\n");
            for (Subscription s : subscriptions) {
                sb.append("Subscribing for notifications of type: ").append(s.getType()).append("\n");
            }
            message = sb.toString();
        }
        log.info(message);

        return message;
    }
}
