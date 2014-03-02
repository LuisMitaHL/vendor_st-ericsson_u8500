/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import com.stericsson.RefMan.Commons.TestIO;
import com.stericsson.RefMan.Commons.TestMustConvertToHtmlFilenameFilter;
import com.stericsson.RefMan.TemplateComparator.TestXMLVerifier;
import com.stericsson.RefMan.Toc.TestDocumentFactory;
import com.stericsson.RefMan.Toc.TestElementComparator;
import com.stericsson.RefMan.Toc.TestElementFactory;
import com.stericsson.RefMan.Toc.TestIncludeDoc;
import com.stericsson.RefMan.Toc.TestStdAPIElement;
import com.stericsson.RefMan.Toc.TestTocElement;
import com.stericsson.RefMan.Toc.TestTopicElement;
import com.stericsson.RefMan.USDoc.TestDoxygenRunner;
import com.stericsson.RefMan.USDoc.TestFindTocLocationsFromFile;
import com.stericsson.RefMan.USDoc.TestFindTocLocationsFromRepo;
import com.stericsson.RefMan.USDoc.TestUserSpaceDocumentation;
import com.stericsson.RefMan.USDoc.TestXmltoRunner;
import com.stericsson.RefMan.UmlExportFilter.TestUmlExport;
import com.stericsson.RefMan.UmlExportFilter.TestUmlExportExampleFilesAndSchema;
import com.stericsson.RefMan.UmlExportFilter.TestUmlExportInterface;
import com.stericsson.RefMan.UmlExportFilter.TestUmlExportPlatform;
import com.stericsson.RefMan.Validator.TestXSDValidator;
import com.stericsson.RefMan.kernelDoc.TestCopyHtml;

/**
 *
 * @author Thomas Palmqvist
 *
 */
@RunWith(Suite.class)
@Suite.SuiteClasses( { TestElementComparator.class, TestDocumentFactory.class,
        TestXSDValidator.class, TestElementFactory.class, TestTocElement.class,
        TestTopicElement.class, TestStdAPIElement.class, TestIncludeDoc.class,
        TestIO.class, TestCopyHtml.class, TestDoxygenRunner.class,
        TestXmltoRunner.class, TestMustConvertToHtmlFilenameFilter.class,
        TestUmlExportExampleFilesAndSchema.class,
        com.stericsson.RefMan.UmlExportFilter.TestElementFactory.class,
        TestUmlExportPlatform.class, TestUmlExportInterface.class,
        TestFindTocLocationsFromFile.class, TestFindTocLocationsFromRepo.class,
        TestUserSpaceDocumentation.class, TestCreateEclipsePlugin.class,
        TestXMLVerifier.class, TestUmlExport.class, TestCreateHtmlManual.class })
public class AllRefManTests {
}
