/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.Collection;
import java.util.HashMap;
import java.util.Set;

/**
 * A container class used to get a synchronized hash map with limited
 * functionality.
 *
 * @param <K> the key
 * @param <V> the value
 */
public class AtHashMap<K, V> {

    /**
     * A non synchronous hash map.
     */
    private HashMap<K, V> hashMap = new HashMap<K, V>();

    /**
     * Associates the specified value with the specified key in this map. If the
     * map previously contained a mapping for this key, the old value is
     * replaced.
     *
     * @param key - key with which the specified value is to be associated.
     * @param value - value to be associated with the specified key.
     * @return previous value associated with specified key, or null if there
     *         was no mapping for key. A null return can also indicate that the
     *         HashMap previously associated null with the specified key.
     */
    public synchronized V put(K key, V value) {
        return hashMap.put(key, value);
    }

    /**
     * Removes the mapping for this key from this map if present.
     *
     * @param key - key whose mapping is to be removed from the map.
     * @return previous value associated with specified key, or null if there
     *         was no mapping for key. A null return can also indicate that the
     *         map previously associated null with the specified key.
     */
    public synchronized V remove(Object key) {
        return hashMap.remove(key);
    }

    /**
     * Removes all mappings from this map.
     */
    public synchronized void clear() {
        hashMap.clear();
    }

    /**
     * Returns a collection view of the values contained in this map. The
     * collection is backed by the map, so changes to the map are reflected in
     * the collection, and vice-versa. The collection supports element removal,
     * which removes the corresponding mapping from this map, via the
     * Iterator.remove, Collection.remove, removeAll, retainAll, and clear
     * operations. It does not support the add.
     *
     * @return a collection view of the values contained in this map.
     */
    public synchronized Collection<V> values() {
        return hashMap.values();
    }

}
