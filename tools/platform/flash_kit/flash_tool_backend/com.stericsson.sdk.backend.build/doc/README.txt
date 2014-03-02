Flash Kit Tools
===============

ABSTRACT
--------
This README file describes the usage of the Flash Kit tools
developed by ST-Ericsson.

INTRODUCTION
~~~~~~~~~~~~
The command line interface tools and the flash tool backend service are
designed to: +
- Assemble and sign flash archives containing platform software +
- Download and execute a flash and customize loader to platform hardware +
- Process flash archive files in order to program flash +

Additional information can be found on http://wiki.lud.stericsson.com/wiki/index.php/Flash_Kit[*Flash Kit Wikipage*] +

Below you can find information about Fash Kit tools, their proper setup and  basic 
guideline about their usage. In each section you can find link to speciffic tool help +

1 Flashkit Tools basics
-----------------------

Basic Workflow Description
~~~~~~~~~~~~~~~~~~~~~~~~~~
Flash Kit tools are used to flash software, archives or loaders to the mobile equipment (ME). You need to assemble archive using assemble tool, sign it using sign tool, in the end you will flash created file to your ME. Flashing consists of two architectural parts, client and backend. Client is responsible for providing user input for backend, which will make command itself.
You can automate most difficult actions using flash-script engine. Following sections are in the order of usage when working with tools  + 

1.1 Assemble Tool CLI
~~~~~~~~~~~~~~~~~~~~~
Assemble tool CLI is a command line tool necessary for assembling the software in the correct format, which can be used for flashing to ME later on.
This is a requirement regardless of whether it is loaders, app or acc SW, or flash archives. +

file:assemble-tool-cli/doc/assemble-tool-cli.html[1.1.1 Information about usage of Assemble tool CLI] +

1.2 Sign Tool CLI
~~~~~~~~~~~~~~~~~
For security reasons it is necessary to sign the software to be able to flash software to the mobile equipment. This is a requirement regardless of whether it is loaders, app or acc SW.
There are unique clients used for each platform.  +

link:sign-tool-cli/doc/signing.html[1.2.1 Step by step information about setting up sing server] +
link:sign-tool-cli/doc/sign-tool-cli-a2.html[1.2.2 Information about usage of Sign tool CLI for A2 platform.] +
link:sign-tool-cli/doc/sign-tool-cli-m570-authenticate.html[1.2.3 Information about usage of Sign tool CLI for m570 platform.] +
link:sign-tool-cli/doc/sign-tool-cli-u5500-u8500-m7400.html[1.2.4 Information about usage of Sign tool  CLI for u8500/u5500/m7400 platform.] +


1.3 Flash Tool Backend
~~~~~~~~~~~~~~~~~~~~~~
The Flash tool backend provides the user with a defined, common protocol for communication with all supported ME platforms. so that the client is not restricted to a certain ME platform. The Flash tool backend handles the basic protocols so that the user does not need specific information regarding the platform to develop specific clients or test tools. +
Backend has to be running when user wants to perform any kind of ME related operations (flashing, erasing, ...), by calling speciffic commands using Flash Tool CLI.

link:flash-tool-backend/doc/flash-tool-backend.html[1.3.1 Information about usage of Flash tool backend] +


1.4 Flash Tool CLI
~~~~~~~~~~~~~~~~~~
Flash tool client is the  name for the application that use the remote message protocol to communicate with the backend. The purpose of the client is to provide the user with a simple interface to manipulate the mobile platform. Flash tool CLI is a simple command line interface that communicates with Backend by sending and receiving requests (commands) supported by remote message protocol. The backend remote protocol accepts requests from flash-tool backend. +
User can perform ME related operations if Flash Tool Backend is running, by calling speciffic comands (process_file for flashing etc. )

link:flash-tool-cli/doc/flash-tool-cli.html[1.4.1 Information about usage of Flash tool CLI.] +

Copyright
~~~~~~~~~
Copyright (C) ST-Ericsson SA 2012. All rights reserved.
This code is ST-Ericsson proprietary and confidential.
Any use of the code for whatever purpose is subject to
specific written permission of ST-Ericsson SA.
