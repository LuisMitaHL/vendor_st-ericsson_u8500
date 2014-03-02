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

package com.stericsson.modemtracecontrol.ui;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.Utility;

/**
 * Starting point for the application.
 * Sets up the ViewPager with the different fragments ("pages")
 * and handles the swiping between the pages.
 */
public class ModemTraceControlActivity extends FragmentActivity {

    private static final int NUMBER_OF_PAGES = 4;

    private ViewPager mViewPager;

    private TraceControlFragmentPagerAdapter mFragmentPagerAdapter;

    private static Context mContext;

    /**
     * Callback being called when the activity is created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(Utility.APP_NAME, "Starting modem trace control");
        setContentView(R.layout.main);
        mViewPager = (ViewPager) findViewById(R.id.viewpager);
        mFragmentPagerAdapter = new TraceControlFragmentPagerAdapter(getSupportFragmentManager());
        mViewPager.setAdapter(mFragmentPagerAdapter);
        mViewPager.setOnPageChangeListener(new MyPageChangeListener());
        mViewPager.setOffscreenPageLimit(NUMBER_OF_PAGES);
        mContext = this;
    }

    /**
     * The following two classes handle swipe actions from the user and update the ui accordingly.
     */
    private class MyPageChangeListener extends ViewPager.SimpleOnPageChangeListener {
        @Override
        public void onPageSelected(int position) {

            if (position == 0) {
                TraceMainFragment fragment =
                    (TraceMainFragment) getSupportFragmentManager().findFragmentByTag(
                        "android:switcher:" + R.id.viewpager + ":0");

                if (fragment != null) {
                    if (fragment.getView() != null) {
                        fragment.updateDisplay();
                    }
                }
            }
        }
    }

    private static class TraceControlFragmentPagerAdapter extends FragmentPagerAdapter {

        public TraceControlFragmentPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int index) {
            Fragment pageToShow = null;
            switch (index) {
                case 0:
                    pageToShow = TraceMainFragment.newInstance("traceMainFragment", mContext);
                    break;
                case 1:
                    pageToShow = TraceAutoConfFragment.newInstance("traceAutoConfFragment",
                            mContext);
                    break;
                case 2:
                    pageToShow = TraceAdvancedFragment.newInstance("traceAdvancedFragment", mContext);
                    break;
                case 3:
                    pageToShow = SettingsFragment.newInstance("settingsFragment", mContext);
                    break;
            }
            return pageToShow;
        }

        @Override
        public int getCount() {
            return NUMBER_OF_PAGES;
        }
    }
}
