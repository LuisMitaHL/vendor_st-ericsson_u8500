/*
 * Copyright (C) ST-Ericsson SA 2010
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

package com.stericsson.modemtracecontrol.test.internal;

import java.util.ArrayList;
import java.util.List;

import android.content.Intent;
import android.test.mock.MockContext;

public class TestContext extends MockContext {
    private List<Intent> mReceivedIntents = new ArrayList<Intent>();

    @Override
    public String getPackageName()
    {
        return "com.stericsson.modemtracecontrol.test.internal";
    }

    @Override
    public void startActivity(Intent xiIntent)
    {
        mReceivedIntents.add(xiIntent);
    }

    public List<Intent> getReceivedIntents()
    {
        return mReceivedIntents;
    }
}
