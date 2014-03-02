package com.stericsson.sdk.common.junit.runner;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.LinkedHashSet;
import java.util.Set;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * Aim of this class is to retrieve from given class in constructor all methods related to junit3
 * and junit4 framework. After successful construction of this class will be provided methods for
 * fetching testing, fixture init and deinit methods of type {@link Method}.
 * 
 * @author rohacmic
 * 
 */
public class JUnitMethodsBrowser {

    private Set<Method> initFixtureMethod = new LinkedHashSet<Method>();

    private Set<Method> deinitFixtureMethod = new LinkedHashSet<Method>();

    private Set<Method> testMethods = new LinkedHashSet<Method>();

    /**
     * @param clazz
     *            Class to be scanned for junit methods.
     */
    public JUnitMethodsBrowser(Class<?> clazz) {
        for (Method m : clazz.getMethods()) {
            if (!(Modifier.toString(m.getModifiers()).equals("public") && m.getReturnType().equals(void.class) && (m
                .getParameterTypes().length == 0))) {
                continue;
            }

            if (m.isAnnotationPresent(Test.class) || m.getName().startsWith("test")) {
                testMethods.add(m);
            } else if (m.isAnnotationPresent(Before.class) || m.getName().equals("setUp")) {
                initFixtureMethod.add(m);
            } else if (m.isAnnotationPresent(After.class) || m.getName().equals("tearDown")) {
                deinitFixtureMethod.add(m);
            }
        }
    }

    /**
     * @return Set of {@link Method} which has to be called to initialize Fixture.
     */
    public Set<Method> getInitFixtureMethod() {
        return initFixtureMethod;
    }

    /**
     * @return Set of {@link Method} which has to be called to deinitialize Fixture.
     */
    public Set<Method> getDeinitFixtureMethod() {
        return deinitFixtureMethod;
    }

    /**
     * @return Set of {@link Method} to test.
     */
    public Set<Method> getTestMethods() {
        return testMethods;
    }

}
