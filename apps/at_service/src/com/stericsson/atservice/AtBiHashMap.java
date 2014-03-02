/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class AtBiHashMap<K, V> {

    /**
     * A hash map.
     */
    private HashMap<K, V> hashMap = new HashMap<K, V>();

    /**
     * The inverse map of the one above.
     */
    private HashMap<V, K> inverseHashMap = new HashMap<V, K>();

    /**
     * Returns the number of key-value mappings in this map.
     *
     * @return the number of key-value mappings in this map
     */
    public int size() {
        return hashMap.size();
    }

    /**
     * Returns true if this map contains no key-value mappings.
     *
     * @return true if this map contains no key-value mappings
     */
    public boolean isEmpty() {
        return hashMap.isEmpty();
    }

    /**
     * Returns true if this map contains a mapping for the specified key.
     *
     * @param key The key whose presence in this map is to be tested
     * @return true if this map contains a mapping for the specified key
     */
    public boolean containsKey(Object key) {
        return hashMap.containsKey(key);
    }

    /**
     * Returns true if this map maps one or more keys to the specified value.
     *
     * @param value value whose presence in this map is to be tested
     * @return true if this map maps one or more keys to the specified value
     */

    public synchronized boolean containsValue(Object value) {
        return hashMap.containsValue(value);
    }

    /**
     * Returns the value to which the specified key is mapped, or null if this
     * map contains no mapping for the key. More formally, if this map contains
     * a mapping from a key k to a value v such that (key==null ? k==null :
     * key.equals(k)), then this method returns v; otherwise it returns null.
     * (There can be at most one such mapping.) A return value of null does not
     * necessarily indicate that the map contains no mapping for the key; it's
     * also possible that the map explicitly maps the key to null. The
     * containsKey operation may be used to distinguish these two cases.
     *
     * @param key the key whose associated value is to be returned
     * @return the value to which the specified key is mapped, or null if this
     *         map contains no mapping for the key
     */
    public V get(Object key) {
        return hashMap.get(key);
    }

    /**
     * Returns the key mapping to the specified value or if there is no key
     * mapping to the value null is returned. Only one key will map to the value
     * as all values are unique.
     *
     * @param value the value whose associated key is to be returned
     * @return the key to which the specified value is mapped, or null if this
     *         map contains no mapping for the value
     */
    public K getKey(V value) {
        return inverseHashMap.get(value);
    }

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
    public V put(K key, V value) {
        // v == null -> key is new
        // v != null -> key used before, removed old value
        V v = hashMap.put(key, value);
        // k == null -> value new
        // k != null -> value used fore, remove old key
        K k = inverseHashMap.put(value, key);

        if (v != null) {
            // the pair key, v is removed from hashMap
            // - remove v, key from inverseHashMap
            inverseHashMap.remove(v);
        }

        if (k != null) {
            // the pair value, k is remove from inverse
            // - remove k, value also from hashMap
            hashMap.remove(k);
        }

        return v;

    }

    /**
     * Removes the mapping for this key from this map if present.
     *
     * @param key - key whose mapping is to be removed from the map.
     * @return previous value associated with specified key, or null if there
     *         was no mapping for key. A null return can also indicate that the
     *         map previously associated null with the specified key.
     */
    public V remove(Object key) {
        V v = hashMap.remove(key);
        inverseHashMap.remove(v);
        return v;
    }

    /**
     * Returns a Set view of the keys contained in this map. Changes to the map
     * are NOT reflected in the set, and vice-versa.
     *
     * @return a set view of the keys contained in this map
     */
    public Set<K> keySet() {
        Set<K> keys = new HashSet<K>(hashMap.keySet());
        return keys;
    }

}
