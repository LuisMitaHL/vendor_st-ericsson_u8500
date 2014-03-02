package com.stericsson.sdk.signing.creators;

import java.io.File;

import com.stericsson.sdk.signing.data.RootSignPackageData;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.util.SignPackageWriter;

/**
 * Creates root sign packages.
 * 
 * @author TSIKOR01
 * 
 */
public class RootSignPackageCreator {

    private static final String ROOT_PACKAGE_SUFFIX = "_ROOT";

    private static final String PACKAGE_EXTENSION = ".pkg";

    /**
     * Class constructor.
     */
    public RootSignPackageCreator() {

    }

    /**
     * Creates root sign package based on provided root sign package data.
     * 
     * @param rootSignPackageData
     *            Root sign package data.
     * @throws Exception
     *             Exception.
     */
    public void create(RootSignPackageData rootSignPackageData) throws Exception {
        if (rootSignPackageData == null) {
            throw new NullPointerException();
        }

        File file =
            new File(rootSignPackageData.getPath(), rootSignPackageData.getAlias() + ROOT_PACKAGE_SUFFIX
                + PACKAGE_EXTENSION);
        String path = file.getAbsolutePath();
        SignPackageWriter.writeSignPackage(path, rootSignPackageData.getAlias(), ISignPackage.PACKAGE_TYPE_U5500_ROOT,
            null, rootSignPackageData.getRevision(), rootSignPackageData.getDescription(), rootSignPackageData
                .getIsswCertificate());
    }
}
