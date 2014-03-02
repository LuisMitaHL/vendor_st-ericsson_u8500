package com.stericsson.sdk.equipment.ui.views.providers;

import java.util.Hashtable;
import java.util.Locale;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Content provider for the equipment view
 * 
 * @author xolabju
 * 
 */
public class EquipmentViewLabelProvider extends LabelProvider implements ITableLabelProvider {

    private Image greenLight;

    private Image yellowLight;

    private Image redLight;

    private Image warmGreenLight;

    private Image warmYellowLight;

    private Image warmRedLight;

    private Image greenLightAuth;

    /**
     * Constructor
     */
    public EquipmentViewLabelProvider() {
        greenLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/green.png"))
                .createImage();
        yellowLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/yellow.png"))
                .createImage();
        redLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/red.png"))
                .createImage();
        warmGreenLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/warm_green.png"))
                .createImage();
        warmYellowLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/warm_yellow.png"))
                .createImage();
        warmRedLight =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/warm_red.png"))
                .createImage();
        greenLightAuth =
            ImageDescriptor.createFromURL(Activator.getDefault().getBundle().getResource("icons/greenauth.png"))
                .createImage();
    }

    /**
     * Platform | Port | State | Profile {@inheritDoc}
     */
    public String getColumnText(Object obj, int index) {
        IEquipment equipment = (IEquipment) obj;
        Hashtable<String, EquipmentProperty> properties = equipment.getProperties();
        switch (index) {
            case 0:
                return properties.get(IPlatform.PLATFORM_PROPERTY).getPropertyValue().toUpperCase(Locale.getDefault());
            case 1:
                return properties.get(IPlatform.PORT_PROPERTY).getPropertyValue();
            case 2:
                return equipment.getStatus().getState().niceName();
            case 3:
                return equipment.getProfile().getAlias();
            default:
                break;
        }

        return "";
    }

    /**
     * {@inheritDoc}
     */
    public Image getColumnImage(Object obj, int index) {
        IEquipment equipment = (IEquipment) obj;
        switch (index) {
            // ME
            case 0:
                if (equipment.isInWarmMode()) {
                    return getWarmStateLight(equipment.getStatus().getState());
                } else {
                    return getStateLight(equipment.getStatus().getState());
                }
            default:
                break;
        }

        return null;
    }

    private Image getWarmStateLight(EquipmentState state) {
        switch (state) {
            case IDLE:
            case LC_NOT_INITIALIZED:
                return warmGreenLight;
            case ERROR:
                return warmRedLight;
            default:
                return warmYellowLight;
        }
    }

    private Image getStateLight(EquipmentState state) {
        switch (state) {
            case IDLE:
            case LC_NOT_INITIALIZED:
                return greenLight;
            case ERROR:
                return redLight;
            case AUTHENTICATING:
                return greenLightAuth;
            default:
                return yellowLight;
        }
    }

    // private Image fixImage(final Image source) {
    // Image image = null;
    // Display.getDefault().disposeExec(new Runnable() {
    // public void run() {
    // if (!source.isDisposed()) {
    // source.dispose();
    // }
    // }
    // });
    // image = new Image(source.getDevice(), source.getBounds().width, source.getBounds().height);
    // GC gc = new GC(image);
    // gc.setAlpha(255);
    // gc.drawImage(source, 0, 0);
    // gc.dispose();
    // return image;
    // }
}
