package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * ADT class for model ID's in ISSW customer part
 * @author xdancho
 * 
 */
public class GenericISSWModelId {

    private List<Short> modelIdList = new ArrayList<Short>();

    /**
     * Get model IDs
     * 
     * @return A list of model IDs.
     */
    List<Short> getIds() {
        return modelIdList;
    }

    /**
     * Set model IDs.
     * 
     * @param idList
     *            A list of model IDs.
     */
    void setModelId(List<Short> idList) {
        Collections.sort(idList);
        modelIdList = idList;
    }

    private int getNumberOfModelIds() {
        return modelIdList.size();
    }

    /**
     * Get length of Model ID structure in bytes.
     * 
     * @return The length in bytes.
     */
    int getLength() {
        if ((modelIdList.size() + 1) % 2 != 0) {
            return (modelIdList.size() + 2) * 2;
        } else {
            return (modelIdList.size() + 1) * 2;
        }

    }

    /**
     * Set data.
     * 
     * @param data
     *            The data array.
     * @param pos
     *            The position of the model ID
     */
    void setData(byte[] data, int pos) {
        final byte[] modelIdData = new byte[2];
        System.arraycopy(data, pos, modelIdData, 0, modelIdData.length);
        pos += 2;
        final short numberOfModelIds = LittleEndianByteConverter.byteArrayToShort(modelIdData);

        for (int i = 0; i < numberOfModelIds; i++) {
            System.arraycopy(data, pos, modelIdData, 0, modelIdData.length);
            modelIdList.add(LittleEndianByteConverter.byteArrayToShort(modelIdData));
            pos += 2;
        }

    }

    /**
     * Get data from model ID
     * 
     * @return the model ID as data
     */
    byte[] getData() {
        final ByteBuffer idData = ByteBuffer.allocate(getLength());
        idData.put(LittleEndianByteConverter.valueToByteArray(getNumberOfModelIds(), 2));
        for (final Short id : modelIdList) {
            idData.put(LittleEndianByteConverter.valueToByteArray(id, 2));
        }
        // padding
        if ((getNumberOfModelIds() + 1) % 2 != 0) {
            idData.put(new byte[] {
                0, 0});
        }
        return idData.array();
    }

}
