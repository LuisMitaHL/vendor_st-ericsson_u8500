/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import com.stericsson.RefMan.Commons.log4j.TestLogTracker;

/**
 *
 * @author Thomas Palmqvist
 *
 */
@RunWith(Suite.class)
@Suite.SuiteClasses( { AllRefManTests.class,
    TestLogTracker.class})
public class AllTests {
}
