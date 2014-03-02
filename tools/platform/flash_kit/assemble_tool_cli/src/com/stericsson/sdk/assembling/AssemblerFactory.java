package com.stericsson.sdk.assembling;

import java.util.Locale;

import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.assembling.cli.Assemble;
import com.stericsson.sdk.assembling.cli.ICommandLineProxy;
import com.stericsson.sdk.assembling.internal.u8500.U8500AssemblerSettings;
import com.stericsson.sdk.assembling.internal.u8500.U8500BinaryImageAssembler;
import com.stericsson.sdk.assembling.internal.u8500.U8500CommandLineOptions;
import com.stericsson.sdk.assembling.internal.u8500.U8500FlashArchiveAssembler;
import com.stericsson.sdk.assembling.internal.u8500.U8500PreflashImageAssembler;

/**
 * This factory class is used to create different types of assembler based on some properties.
 * 
 * @author xolabju
 */
public class AssemblerFactory implements IAssemblerFactory {
    /**
     * Constructor
     */
    public AssemblerFactory() {
    }

    /**
     * {@inheritDoc}
     */
    public IAssembler createAssembler(IAssemblerSettings settings) {
        if (Assemble.PLATFORM_U8500.equals(settings.get(IAssemblerSettings.KEY_PLATFORM_TYPE))) {
            return createU8500Assembler(settings);
        }
        return null;
    }

    private IAssembler createU8500Assembler(IAssemblerSettings settings) {
        String outputType = (String) settings.get(IAssemblerSettings.KEY_OUTPUT_TYPE);

        if (U8500CommandLineOptions.TYPE_FLASH_ARCHIVE.equals(outputType)) {
            return new U8500FlashArchiveAssembler();
        } else if (U8500CommandLineOptions.TYPE_RAM_IMAGE.equals(outputType)) {
            return new U8500BinaryImageAssembler();
        } else if (U8500CommandLineOptions.TYPE_PREFLASH_IMAGE.equals(outputType)) {
            return new U8500PreflashImageAssembler();
        }
        return null;
    }

    /**
     * Create an assembler settings object
     * 
     * @param outFile
     *            The output file
     * @param cmdLineProxy
     *            settings that was set by the commandline.
     * 
     * @return An IAssemblerSettings interface
     * 
     * @throws AssemblerSettingsException
     *             If an assembler settings related error occurred
     * @throws AssemblerException
     *             If an assembler related error occurred
     * @throws ParseException
     *             parseException
     */
    public IAssemblerSettings createAssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
        throws AssemblerSettingsException, AssemblerException, ParseException {
        if (Assemble.PLATFORM_U8500.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createU8500AssemblerSettings(outFile, cmdLineProxy);
        } else {
            return null;
        }
    }

    private IAssemblerSettings createU8500AssemblerSettings(String outFile, ICommandLineProxy cmdLineProxy)
        throws ParseException, AssemblerSettingsException, AssemblerException {
        String outputType =
            (String) cmdLineProxy.getOptionValue(U8500CommandLineOptions.Option.OUTPUT_TYPE.getShortCMD());
        if (!U8500CommandLineOptions.isOutputTypeSupported(outputType)) {
            throw new AssemblerException(outputType + " is not a valid output type");
        }

        String confFile = cmdLineProxy.getOptionValue(U8500CommandLineOptions.Option.CONFIGURATION.getShortCMD());
        if (confFile == null) {
            throw new AssemblerException("Configuration file is not valid");
        }
        String fileList = cmdLineProxy.getOptionValue(U8500CommandLineOptions.Option.FILE_LIST.getShortCMD());
        if (fileList == null) {
            throw new AssemblerException("File list is not valid");
        }

        U8500AssemblerSettings newSettings = new U8500AssemblerSettings(confFile, fileList, outFile);
        newSettings.put(IAssemblerSettings.KEY_OUTPUT_TYPE, outputType);
        newSettings.put(IAssemblerSettings.KEY_PLATFORM_TYPE, Assemble.PLATFORM_U8500);

        // Store important objects into assembler settings
        newSettings.put(IAssemblerSettings.KEY_PLATFORM_TYPE, cmdLineProxy.getArgs()[0]);

        getBufferSizeSetting(newSettings, cmdLineProxy);

        getAlignmentSizeSetting(newSettings, cmdLineProxy);

        return newSettings;
    }

    private void getBufferSizeSetting(U8500AssemblerSettings settings, ICommandLineProxy cmdLineProxy)
        throws ParseException, AssemblerSettingsException, AssemblerException {
        if (cmdLineProxy.hasOption(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD())) {
            int bufferSize = 0;
            String bufferSizeStr = cmdLineProxy.getOptionValue(U8500CommandLineOptions.Option.BUFFER_SIZE.getLongCMD());
            if (bufferSizeStr != null) {
                if (bufferSizeStr.toLowerCase(Locale.getDefault()).endsWith("k")) {
                    bufferSizeStr = bufferSizeStr.substring(0, bufferSizeStr.length() - 1);
                    bufferSize = Integer.decode(bufferSizeStr) * 1024;
                } else if (bufferSizeStr.toLowerCase(Locale.getDefault()).endsWith("m")) {
                    bufferSizeStr = bufferSizeStr.substring(0, bufferSizeStr.length() - 1);
                    bufferSize = Integer.decode(bufferSizeStr) * 1024 * 1024;
                } else {
                    bufferSize = Integer.decode(bufferSizeStr);
                }
            }
            if (bufferSize < 1) {
                throw new AssemblerException("Buffer size value is not valid");
            }
            settings.put(IAssemblerSettings.KEY_BUFFER_SIZE, bufferSize);
        }
    }

    private void getAlignmentSizeSetting(U8500AssemblerSettings settings, ICommandLineProxy cmdLineProxy)
        throws ParseException, AssemblerSettingsException, AssemblerException {
        if (cmdLineProxy.hasOption(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD())) {
            int alignmentSize = 0;
            String alignmentSizeStr =
                cmdLineProxy.getOptionValue(U8500CommandLineOptions.Option.ALIGNMENT_SIZE.getLongCMD());
            if (alignmentSizeStr != null) {
                alignmentSize = Integer.decode(alignmentSizeStr);
            }
            if (alignmentSize < 1 || alignmentSize > 65536) {
                throw new AssemblerException("Alignment size value is out of range 1..65536");
            }
            settings.put(IAssemblerSettings.KEY_ALIGNMENT_SIZE, alignmentSize);
        }
    }
}
