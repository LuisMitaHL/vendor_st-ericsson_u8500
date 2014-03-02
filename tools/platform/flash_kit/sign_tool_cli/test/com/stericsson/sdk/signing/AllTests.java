package com.stericsson.sdk.signing;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import com.stericsson.sdk.signing.cli.A2CLISignerSettingsTest;
import com.stericsson.sdk.signing.cli.CommandLineSignerSettingsSetupTest;
import com.stericsson.sdk.signing.cli.GenericCommandLineOptionsTest;
import com.stericsson.sdk.signing.cli.SignTest;
import com.stericsson.sdk.signing.cli.SmokeTester;
import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptionsTest;
import com.stericsson.sdk.signing.cli.a2.A2SignTest;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesConfigurationReaderTest;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesSignerSettingsTest;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesTest;
import com.stericsson.sdk.signing.cli.u5500.U5500SignTest;
import com.stericsson.sdk.signing.cli.u8500.U8500SignTest;

/**
 * @author xtomlju
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({
    A2CommandLineOptionsTest.class, SignerFactoryTest.class, A2CLISignerSettingsTest.class, SmokeTester.class,
    GenericCommandLineOptionsTest.class, A2SignTest.class, U5500SignTest.class, U8500SignTest.class, SignTest.class,
    CommandLineSignerSettingsSetupTest.class, LoadModulesTest.class, LoadModulesSignerSettingsTest.class,
    LoadModulesConfigurationReaderTest.class})
public class AllTests {

}
