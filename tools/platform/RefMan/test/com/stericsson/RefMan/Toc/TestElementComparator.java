/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Collections;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 *
 * @author Thomas Palmqvist
 *
 */
public class TestElementComparator {

    /**
     * @throws java.lang.Exception
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.ElementComparator#compare(com.stericsson.RefMan.Toc.Element, com.stericsson.RefMan.Toc.Element)}
     * .
     */
    @Test
    public void testComparator() {
        ArrayList<Element> elements = new ArrayList<Element>();
        Element e = new TopicElement();
        e.setLabel("C");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("D");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("a");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("A");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("c");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("E");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("b");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("A");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("E");
        elements.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elements.add(e);
        Collections.sort(elements, new ElementComparator());
        ArrayList<Element> elementsCorrect = new ArrayList<Element>();
        e = new TopicElement();
        e.setLabel("A");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("A");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("a");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("B");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("b");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("C");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("c");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("D");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("E");
        elementsCorrect.add(e);
        e = new TopicElement();
        e.setLabel("E");
        elementsCorrect.add(e);
        boolean passed = false;
        compare: if (elements.size() == elementsCorrect.size()) {
            for (int i = 0; i < elements.size(); i++) {
                if (!elements.get(i).getLabel().equals(
                        elementsCorrect.get(i).getLabel())) {
                    break compare;
                }
            }
            passed = true;
        }
        assertTrue(passed);
    }

}
