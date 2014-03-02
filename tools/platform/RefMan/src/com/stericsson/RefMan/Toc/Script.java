/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

public class Script {

    /** A <code>String</code> containing the script to run. */
    private String script;

    /** The path to the directory where the script should be started from. */
    private String startDir;

    /**
     * Constructor
     *
     * @param script
     *            A <code>String</code> containing the script to run.
     * @param startDir
     *            The path to the directory where the script should be started
     *            from.
     */
    public Script(String script, String startDir) {
        this.script = script;
        this.startDir = startDir;
    }

    /**
     * Set the script to run
     *
     * @param script
     *            the script to run.
     */
    public void setScript(String script) {
        this.script = script;
    }

    /**
     * Get the script to run
     *
     * @return The script to run
     */
    public String getScript() {
        return script;
    }

    /**
     * Set the directory to start the script in.
     *
     * @param startDir
     *            The directory to start the script in
     */
    public void setStartDir(String startDir) {
        this.startDir = startDir;
    }

    /**
     * Get the directory to start the script in.
     *
     * @return The directory to start the script in.
     */
    public String getStartDir() {
        return startDir;
    }
}
