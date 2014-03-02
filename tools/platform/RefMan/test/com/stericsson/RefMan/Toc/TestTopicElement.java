/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.util.Vector;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestTopicElement {

    private TopicElement child1;
    private TopicElement child2;
    private TopicElement elmCorrect1;
    private TopicElement elmCorrect2;
    private TopicElement elmIncorrect;
    private TopicElement elmNoTopics;

    @Before
    public void setUp() throws Exception {
        child1 = new TopicElement();
        child1.setLabel("child");
        Vector<Element> v1 = new Vector<Element>();
        v1.add(child1);
        elmCorrect1 = new TopicElement();
        elmCorrect1.setLabel("label_correct");
        elmCorrect1.setHref("href_correct");
        elmCorrect1.setTopics(v1);
        child2 = new TopicElement();
        child2.setLabel("child");
        Vector<Element> v2 = new Vector<Element>();
        v2.add(child2);
        elmCorrect2 = new TopicElement();
        elmCorrect2.setLabel("label_correct");
        elmCorrect2.setHref("href_correct");
        elmCorrect2.setTopics(v2);
        elmIncorrect = new TopicElement();
        elmIncorrect.setLabel("label_correct");
        elmNoTopics = new TopicElement();
        elmNoTopics.setLabel("label_correct");
        elmNoTopics.setHref("href_correct");
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.TopicElement#equals(Object)}.
     */
    @Test
    public void testEquals() {
        assertTrue(elmCorrect1.equals(elmCorrect2));
        assertTrue(!elmCorrect1.equals(elmIncorrect));
        assertTrue(!elmCorrect1.equals(elmNoTopics));

    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#hashCode()}
     * .
     */
    @Test
    public void testHashcode() {
        assertTrue(elmCorrect1.hashCode() == elmCorrect2.hashCode());
        assertTrue(elmCorrect1.hashCode() != elmIncorrect.hashCode());
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#toString()}
     * .
     */
    @Test
    public void testToString() {
        String correct = "type: TOPIC; label: label_correct; href: href_correct; topics: 1";
        String string = elmCorrect1.toString();

        assertTrue(string.equals(correct));
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#toString()}
     * .
     * 
     * No value for href and no topics added.
     */
    @Test
    public void testToStringNoHrefNoTopics() {
        String correct = "type: TOPIC; label: label_correct; href: ; topics: 0";
        String string = elmIncorrect.toString();

        assertTrue(string.equals(correct));
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.TopicElement#getType()}.
     * 
     * Test getType() to return 'TOPIC'.
     */
    @Test
    public void testGetType() {
        assertTrue(elmCorrect1.getType().equals("TOPIC"));
    }
}
