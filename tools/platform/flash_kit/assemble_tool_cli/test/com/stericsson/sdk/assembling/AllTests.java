package com.stericsson.sdk.assembling;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import com.stericsson.sdk.assembling.cli.AssembleTest;
import com.stericsson.sdk.assembling.cli.AssemblerExceptionTest;
import com.stericsson.sdk.assembling.internal.u8500.U8500BinaryImageTest;

/**
 * @author xolabju
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({
    AssembleTest.class, AssemblerExceptionTest.class, U8500BinaryImageTest.class})
public class AllTests {

}
