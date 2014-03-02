/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

/**
 *
 * @author Thomas Palmqvist
 *
 */
public interface NodeVisitor {
    void visit(Element node);
}
