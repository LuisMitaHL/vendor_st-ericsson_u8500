package com.stericsson.sdk.signing.test;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {
    /** Logger instance */
    private static Logger logger = Logger.getLogger(Activator.class.getName());

    private String[] testCasesClassesNames =
        new String[] {
        "com.stericsson.sdk.signing.a2.A2AccessControlListTest",
        "com.stericsson.sdk.signing.a2.A2SignTest",
        "com.stericsson.sdk.signing.a2.A2SignerSettingsTest",
        "com.stericsson.sdk.signing.cops.COPSCertificateListTest",
        "com.stericsson.sdk.signing.cops.COPSControlFieldsTest",
        "com.stericsson.sdk.signing.cops.COPSSignedDataTest",
        "com.stericsson.sdk.signing.elf.FileHeaderTest",
        "com.stericsson.sdk.signing.elf.ProgramHeaderTest",
        "com.stericsson.sdk.signing.elf.SectionHeaderTest",
        "com.stericsson.sdk.signing.generic.GenericSignerPropertiesHelperTest",
        "com.stericsson.sdk.signing.generic.HashTypeExtendedTest",
        "com.stericsson.sdk.signing.mockup.FakeMessageTest",
        "com.stericsson.sdk.signing.mockup.FakeRTTServerTest",
        "com.stericsson.sdk.signing.mockup.FakeSignServerTest",
        "com.stericsson.sdk.signing.mockup.MockProtocolMessageTest",
        "com.stericsson.sdk.signing.signerservice.local.COPSRSAPrivateKeyTest",
        "com.stericsson.sdk.signing.signerservice.local.LocalSignerServiceTest",
        "com.stericsson.sdk.signing.signerservice.local.encryption.RSAPKCS1EncrypterTest",
        "com.stericsson.sdk.signing.signerservice.local.encryption.U5500EncrypterTest",
        "com.stericsson.sdk.signing.signerservice.local.encryption.U8500EncrypterTest",
        "com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketFactoryTest",
        "com.stericsson.sdk.signing.signerservice.protocol.SignerProtocolPacketTest",
        "com.stericsson.sdk.signing.signerservice.SignerServiceHelperTest",
        "com.stericsson.sdk.signing.signerservice.SignerServiceSocketFactoryTest",
        "com.stericsson.sdk.signing.signerservice.SignerServiceTest",
        "com.stericsson.sdk.signing.test.AbstractByteSequenceTest",
        "com.stericsson.sdk.signing.test.DigestDecoratorTest",
        "com.stericsson.sdk.signing.tvp.SignedTVPLicenseTest",
        "com.stericsson.sdk.signing.tvp.TVPSignPackageTest",
        "com.stericsson.sdk.signing.tvp.X509CertificateChainTest",
        "com.stericsson.sdk.signing.u5500.U5500AuthenticationCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500CustomerRootKeyCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500DTCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500ISSWCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedAuthenticationCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedCustomerRootKeyCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedDebugAndTestCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedFlashArchiveTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedHeaderTest",
        "com.stericsson.sdk.signing.u5500.UU5500SignedLoadModuleTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedPayloadTest",
        "com.stericsson.sdk.signing.u5500.U5500SignedTrustedApplicationTest",
        "com.stericsson.sdk.signing.u5500.U5500SignPackageTest",
        "com.stericsson.sdk.signing.u5500.U5500SignTest",
        "com.stericsson.sdk.signing.u5500.U5500TrustedApplicationCertificateTest",
        "com.stericsson.sdk.signing.u5500.U5500GenericRootKeyCertificateTest",
        "com.stericsson.sdk.signing.u8500.SectionIDTest",
        "com.stericsson.sdk.signing.u8500.U8500ISSWCertificateTest",
        "com.stericsson.sdk.signing.u8500.U8500ISSWCustomerPartTest",
        "com.stericsson.sdk.signing.u8500.U8500OEMCertificateTest",
        "com.stericsson.sdk.signing.u8500.U8500SignedFlashArchiveTest",
        "com.stericsson.sdk.signing.u8500.U8500SignedPayloadTest",
        "com.stericsson.sdk.signing.u8500.U8500SignPackageTest",
        "com.stericsson.sdk.signing.u8500.U8500SignTest",
        "com.stericsson.sdk.signing.u8500.U8500DTCertificateTest",
        "com.stericsson.sdk.signing.util.PemParserTest",
        "com.stericsson.sdk.signing.u7x00.M7X00ElfPayloadTest",
        "com.stericsson.sdk.signing.u7x00.M7X00SignTest",
        "com.stericsson.sdk.signing.test.SoftwareAlreadySignedExceptionTest",
        "com.stericsson.sdk.signing.creators.KeyPairCreatorTest",
        "com.stericsson.sdk.signing.creators.ISSWCertificateCreatorTest",
        "com.stericsson.sdk.signing.creators.RootSignPackageCreatorTest"
    };

    private static String resourcePath =
        "C:\\ksdRepo\\flash_tool_backend\\com.stericsson.sdk.signing.test\\test_files";

    private static final String RESOURCE_PATH = "/test_files";

    private static void setResourcesPath(String path) {
        Activator.resourcePath = path;
    }

    /**
     * @return return path to resources needed during testing
     */
    public static String getResourcesPath() {
        return resourcePath;
    }

    private void setResourcePath() {
        URL url = getClass().getResource(RESOURCE_PATH);

        URL find = null;

        try {

            find = FileLocator.toFileURL(url);

        } catch (IOException e) {
            e.printStackTrace();

        }
        try {
            File file = new File(find.getFile());

            setResourcesPath(file.getAbsolutePath());
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {

        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        setResourcePath();

        for (String actualClassName : testCasesClassesNames) {

            Class<?> actualClazz;
            try {
                actualClazz = Class.forName(actualClassName);
                testCaseService = (TestCase) actualClazz.newInstance();
                context.registerService(TestCase.class.getName(), testCaseService, null);
                testSuiteService.addTest(testCaseService);
            } catch (Throwable t) {
                logger.error(t.getMessage());
            }
        }

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.signing");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
