package com.stericsson.sdk.common.junit.runner;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import junit.framework.Test;
import junit.framework.TestSuite;

import org.junit.Test.None;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

/**
 * @author xtomlju
 */
public class Runner {

    private static final String PROPERTY_IGNORE_LONGRUNNING = "backend.ignorelongrunning";

    private static final String[] LONGRUNNING_TESTS =
        new String[] {
            "com.stericsson.sdk.common.memory.test", 
            "LocalSignerServiceTest#testSignA2ECLoader",
            "com.stericsson.sdk.signing"};

    private boolean ignoreLongRunning;

    private BundleContext bundleContext;

    private int totalTestCount;

    private int sucessfulTestCount;

    private String failedTests = "";;

    /**
     * @param context
     *            TBD
     */
    public Runner(BundleContext context) {
        updateIgnoreLongRunning();
        bundleContext = context;
    }

    /**
     * @param obj
     *            TBD
     */
    public Runner(Object obj) {
        updateIgnoreLongRunning();
        runTestCases(obj);
    }

    private void updateIgnoreLongRunning() {
        String property = System.getProperty(PROPERTY_IGNORE_LONGRUNNING);
        ignoreLongRunning = property != null && "true".equalsIgnoreCase(property);
    }

    /**
     * 
     */
    public void execute() {
        try {
            ServiceReference[] allServiceReferences =
                bundleContext.getAllServiceReferences(TestSuite.class.getName(), null);
            if (allServiceReferences == null) {
                System.out.println("No Junit services found");
                return;
            }

            for (int i = 0; i < allServiceReferences.length; i++) {
                System.out.println(allServiceReferences[i].getProperty("bundle.name"));
            }
            System.out.println("Total JUnit services found = " + allServiceReferences.length);

            for (int i = 0; i < allServiceReferences.length; i++) {
                // should be JUNIT classe only
                Object service = bundleContext.getService(allServiceReferences[i]);
                runTestCases(service);
                bundleContext.ungetService(allServiceReferences[i]);
            }
            int failedTestCount = totalTestCount - sucessfulTestCount;
            System.out.println("\nTotal tests executed: " + totalTestCount + "\nFailures:             "
                + failedTestCount + "\n");
            if (failedTestCount != 0) {
                System.err.println("Failed tests (" + failedTestCount + "):");
                System.err.println(failedTests);
                System.err.println("Aborting build operation due to failed tests");
                System.exit(1);
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        System.exit(0);
    }

    private void runTestCases(Object obj) {
        List<Object> tests = new ArrayList<Object>();
        if ((obj != null) && (obj instanceof TestSuite)) {
            Enumeration<Test> testSuiteTests = ((TestSuite) obj).tests();
            while (testSuiteTests.hasMoreElements()) {
                tests.add(testSuiteTests.nextElement());
            }
        } else {
            tests.add(obj);
        }

        runTestCases(tests.toArray(new Object[tests.size()]));
    }

    private void runTestCases(Object... tests) {
        for (Object test : tests) {
            JUnitMethodsBrowser methodBrowser = new JUnitMethodsBrowser(test.getClass());
            Object testInstance = null;
            Method lastFixtureMethod = null;
            try {
                for (Method testMethod : methodBrowser.getTestMethods()) {
                    if (ignore(testMethod.getDeclaringClass().getName() + "#" + testMethod.getName())) {
                        continue;
                    }
                    testInstance = test.getClass().newInstance();
                    for (Method initFixture : methodBrowser.getInitFixtureMethod()) {
                        lastFixtureMethod = initFixture;
                        runMethod(testInstance, initFixture);
                    }

                    runTestMethod(testInstance, testMethod);

                    for (Method deinitFixture : methodBrowser.getDeinitFixtureMethod()) {
                        lastFixtureMethod = deinitFixture;
                        runMethod(testInstance, deinitFixture);
                    }
                }
            } catch (Throwable t) {
                String errMsg =
                    "[FAILURE] " + "Unable to run fixture method " + test.getClass().getSimpleName() + "#"
                        + lastFixtureMethod.getName();
                failedTests += errMsg + "\n";
                System.err.println(errMsg);
                if (t.getCause() != null) {
                    t.getCause().printStackTrace();
                } else {
                    t.printStackTrace();
                }
            }
        }
    }

    private boolean ignore(String string) {
        if (!ignoreLongRunning) {
            return false;
        }
        for (String testcase : LONGRUNNING_TESTS) {
            if (string.contains(testcase)) {
                System.out.println("Ignoring test case: " + string + " (matches " + testcase + ")");
                return true;
            }
        }
        return false;
    }

    private void runTestMethod(final Object o, final Method method) {
        System.out.println("Running test case:" + method.getDeclaringClass().getName() + "#" + method.getName());
        Class<? extends Throwable> expected = null;
        // long timeout = 0L; TODO - implementation is needed

        org.junit.Test testAnnotation = method.getAnnotation(org.junit.Test.class);
        if (testAnnotation != null) {
            expected = testAnnotation.expected();
            // timeout = testAnnotation.timeout();
        }

        boolean isPassed = false;
        try {
            method.setAccessible(true);
            method.invoke(o, new Object[0]);
            if ((expected == null) || None.class.equals(expected)) {
                isPassed = true;
                System.out.println("[ OK ] " + method.getName());
            } else {
                System.err.println("[FAIL] " + o.getClass().getName() + "#" + method.getName()
                    + " - Expected exception of type " + expected.getName());
            }
        } catch (Throwable t) {
            if ((expected != null) && !None.class.equals(expected)) {
                if ((t.getCause() != null) && expected.isAssignableFrom(t.getCause().getClass())) {
                    isPassed = true;
                    System.out.println("[ OK ] " + method.getName());
                } else {
                    System.err.println("[FAIL] " + o.getClass().getName() + "#" + method.getName()
                        + " - Expected exception of type " + expected.getName());
                }
            } else if ((t.getCause() != null) && Error.class.isAssignableFrom(t.getCause().getClass())) {
                System.err.println("[FAIL] " + o.getClass().getName() + "#" + method.getName() + " - "
                    + t.getCause().getMessage());
            } else {
                System.err.println("[FAIL] " + o.getClass().getName() + "#" + method.getName());
                t.printStackTrace();
            }
        }

        totalTestCount++;
        if (!isPassed) {
            failedTests += o.getClass().getSimpleName() + "#" + method.getName() + "\n";
        } else {
            sucessfulTestCount++;
        }
    }

    private void runMethod(Object o, Method method) throws Throwable {
        method.setAccessible(true);
        method.invoke(o, new Object[0]);
    }
}
