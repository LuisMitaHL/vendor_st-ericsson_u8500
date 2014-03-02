/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.stericsson.ril.oem.service.test;


import android.test.suitebuilder.annotation.Suppress;


/**
 * Functional tests for STErilOemService and STErilOemHook
 * Mostly identical to STErilOemServiceModuleTest except
 * that it runs directly towards STErilOemService instead
 * of IsolatedSTErilOemService.
 *
 * Note: It is important that IsolatedSTErilOemService is not
 *       Installed when this test is run since it will interfere
 *       with STErilOemService.
 */
public class STErilOemServiceFuncTest
            extends STErilOemServiceModuleTest {


    @Override
    protected void setUp() throws Exception {
        super.setUp();

    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /*
     * Re-use all the Module test except this one.
     */

    @Suppress
    @Override
    public void testUnsolRilOemResponse() {
        // Currently only possible test this in ModuleTest
    }
}