/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons;

import java.util.Arrays;
import java.util.HashSet;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A FilenameFilter which extends the MustConvertToHtmlFilenameFilterer and also
 * lists all files of the Linux kernel 2.29 which needs conversion to HTML.
 *
 * @author qfrelun
 *
 */
public class KernelDocumentationFilenameFilterer
        extends
            MustConvertToHtmlFilenameFilterer {
    /** The logger */
    public final static Logger logger = LoggerFactory
            .getLogger(KernelDocumentationFilenameFilterer.class);
    /**
     * The set of filenames that needs conversion
     */
    private static HashSet<String> kernelFileNamesToConvert = new HashSet<String>(
            Arrays.asList("README-2.5", "7.AdvancedTopics", "CARDLIST.au0828",
                    "CARDLIST.bttv", "HiSax.cert", "3270.ChangeLog",
                    "4.Coding", "8.Conclusion", "hostapd.conf",
                    "Modprobe.conf", "Modules.conf", "wpa_supplicant.conf",
                    "CARDLIST.cx23885", "CARDLIST.cx88", "3.Early-stage",
                    "CARDLIST.em28xx", "syncPPP.FAQ", "INTERFACE.fax",
                    "interactive.fig", "shape.fig", "6.Followthrough",
                    "w1.generic", "linux.inf", "1.Intro", "CARDLIST.ivtv",
                    "caif.man", "fb.modes", "viafb.modes", "w1.netlink",
                    "5.Posting", "2.Process", "AU1xxx_IDE.README",
                    "CARDLIST.saa7134", "CARDLIST.tuner", "CARDLIST.usbvision",
                    "ChangeLog.ide-cd.1994-2004",
                    "ChangeLog.ide-floppy.1996-2002",
                    "ChangeLog.ide-tape.1995-2002"));

    /**
     * Constructor not to be used.
     *
     * @param specialFileNamesToConvert
     */
    protected KernelDocumentationFilenameFilterer(
            HashSet<String> specialFileNamesToConvert) {
    }

    /**
     * Constructor.
     */
    public KernelDocumentationFilenameFilterer() {
        super(kernelFileNamesToConvert);
    }

}
