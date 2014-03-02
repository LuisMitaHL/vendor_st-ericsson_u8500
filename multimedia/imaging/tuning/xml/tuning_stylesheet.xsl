<!-- /*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ -->
 
<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <title>
      <xsl:value-of select="/IQSETTINGS/ID/OP_MODE"/> IQsettings for <xsl:value-of select="/IQSETTINGS/ID/SENSOR/NAME"/> Sensor
    </title>
  </head>
  <body> 
  <h1><xsl:value-of select="/IQSETTINGS/ID/OP_MODE"/> IQsettings for <xsl:value-of select="/IQSETTINGS/ID/SENSOR/NAME"/> Sensor</h1>
  <p><b>(C) STEricsson</b></p>
  <h2>Revision:</h2>
  <p>
    <b><a href="#History">File Release: </a> </b> 
    <xsl:value-of select="/IQSETTINGS/ID/VERSION/STRUCT"/>
    .<xsl:value-of select="/IQSETTINGS/ID/VERSION/ENTRIES"/>
    .<xsl:value-of select="/IQSETTINGS/ID/VERSION/PARAMS"/>
    .<xsl:value-of select="/IQSETTINGS/ID/VERSION/VALUES"/>
  </p>
  <p>
    <b>Sensor Name:</b> <xsl:value-of select="/IQSETTINGS/ID/SENSOR/NAME"/>,
    <b>Manuf ID:</b> <xsl:value-of select="/IQSETTINGS/ID/SENSOR/MANUFACTURER"/>
    <b>Model ID:</b> <xsl:value-of select="/IQSETTINGS/ID/SENSOR/MODEL"/>
    <b>Revision:</b> <xsl:value-of select="/IQSETTINGS/ID/SENSOR/REVISION"/>
  </p>
  <p><b>ISP Firmware Release:</b> 
    <xsl:value-of select="/IQSETTINGS/ID/FIRMWARE/MAJOR"/>
    .<xsl:value-of select="/IQSETTINGS/ID/FIRMWARE/MINOR"/>
    .<xsl:value-of select="/IQSETTINGS/ID/FIRMWARE/PATCH"/>
  </p>
  <p><b>SW3A Release:</b> 
    <xsl:value-of select="/IQSETTINGS/ID/SW3A/MAJOR"/>
    .<xsl:value-of select="/IQSETTINGS/ID/SW3A/MINOR"/>
    .<xsl:value-of select="/IQSETTINGS/ID/SW3A/BUILD"/>
  </p>
  <hr/>
  <h2>Summary list:</h2><a name="Summary"></a>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th align="center">ID</th>
      <th align="center">Target</th>
    </tr>
    <xsl:for-each select="/IQSETTINGS/CONFIGURATION/IQSET">
    <tr>         
        <td>
            <a> 
                <xsl:attribute name="href">
                #<xsl:value-of select="NAME"/>
                </xsl:attribute>
                <xsl:value-of select="NAME"/>
            </a>        
        </td>   
        <td>
            <table>
                <xsl:if test="ISP">
                    <tr><td bgcolor="yellow">ISP</td></tr>
                </xsl:if>    
                <xsl:if test="DRIVER">
                    <tr><td bgcolor="orange">DRIVER</td></tr>
                </xsl:if>
                <xsl:if test="NORCOS">
                    <tr><td bgcolor="green">NORCOS</td></tr>
                </xsl:if>
                <xsl:if test="SW3A_AEC">
                    <tr><td bgcolor="magenta">SW3A_AEC</td></tr>
                </xsl:if>
                <xsl:if test="SW3A_AWB">
                    <tr><td bgcolor="Aquamarine">SW3A_AWB</td></tr>
                </xsl:if>
                <xsl:if test="SW3A_AFC">
                    <tr><td bgcolor="cyan">SW3A_AFC</td></tr>
                </xsl:if>
                <xsl:if test="SW3A_ART">
                    <tr><td bgcolor="Cornflowerblue">SW3A_ART</td></tr>
                </xsl:if>
                <xsl:if test="DAMPER">
                    <tr><td bgcolor="cyan">DAMPER</td></tr>
                </xsl:if>
             </table>
        </td>
    </tr>
    </xsl:for-each>
  </table>
  
  <hr/>
  <h2>Detailed list</h2>
  <table border="1" >
    <xsl:for-each select="/IQSETTINGS/CONFIGURATION/IQSET">
        <tr>
            <td valign = "top">
                <b> <xsl:value-of select="NAME"/> </b>
                <a> <xsl:attribute name="name"> <xsl:value-of select="NAME"/> </xsl:attribute> </a>
                <a href="#Summary">(back to summary list)</a>
            </td>
            <td>

                <xsl:if test="ISP">
                    <table>
                        <tr >
                            <td bgcolor="yellow"><b>ISP</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="ISP/PARAM">
                                    <tr>
                                        <td><table>
                                          <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                          </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="DRIVER">
                    <table>
                        <tr >
                            <td bgcolor="orange"><b>DRIVER</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="DRIVER/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="NORCOS">
                    <table>
                        <tr >
                            <td bgcolor="green"><b>NORCOS</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="NORCOS/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="SW3A_AEC">
                    <table>
                        <tr >
                            <td bgcolor="magenta"><b>SW3A_AEC</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="SW3A_AEC/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="SW3A_AWB">
                    <table>
                        <tr >
                            <td bgcolor="Aquamarine"><b>SW3A_AWB</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="SW3A_AWB/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="SW3A_AFC">
                    <table>
                        <tr >
                            <td bgcolor="cyan"><b>SW3A_AFC</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="SW3A_AFC/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="SW3A_ART">
                    <table>
                        <tr >
                            <td bgcolor="Cornflowerblue"><b>SW3A_ART</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="SW3A_ART/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <xsl:if test="DAMPER">
                    <table>
                        <tr >
                            <td bgcolor="cyan"><b>DAMPER</b></td>
                            <td>
                                <table>
                                    <xsl:for-each select="DAMPER/PARAM">
                                    <tr>
                                        <td><table>
                                            <tr>
                                            <td><xsl:value-of select="ADDR"/></td>
                                            <td><xsl:value-of select="NAME"/> = <xsl:value-of select="VALUE"/></td>
                                            </tr>
                                        </table></td>
                                    </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </table>
                </xsl:if>

            </td>
        </tr>
    </xsl:for-each>
    </table>
  <hr />
  <h2>Revisions history</h2><a name="History"></a>
  <xsl:for-each select="/IQSETTINGS/HISTORY/VERSION">
    <p><b>Release:</b> 
  <xsl:value-of select="STRUCT"/>
  .<xsl:value-of select="ENTRIES"/>
  .<xsl:value-of select="PARAMS"/>
  .<xsl:value-of select="VALUES"/>: <xsl:value-of select="DESC"/>
  </p>

  </xsl:for-each>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>
