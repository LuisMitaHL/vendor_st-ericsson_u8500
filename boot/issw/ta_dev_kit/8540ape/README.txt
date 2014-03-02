The following steps are needed to be able to build and sign a TA:


1. Toolchain

STE currently uses the arm-eabi-4.4.0 compiler. The TA development environment
uses the Andoid default toolchain (android/platform/prebuilt.git). Download the
toolchain to your computer and create a link to the toolchain in the root of
the unpacked TA development package.

ln -s <path to prebuilt.git root>/linux-x86/toolchain .

Or passed in the make parameter CROSS_COMPILE to point out the toolchain.

2. Keys

The TA development kit do not contain any keys. The keys are stored separately.

The keys are needed by the Makefiles to automatically sign the TAs when
building. If the keys are not available the sign step will be excluded from
the build.

3. Signtool

The signtool from flashkit should be used when signing the TAs.

The path of signtool is passed in the make parameter SIGNTOOL. If none
is supplied the parameter will be assigned an internal path in TA dev kit
where a link can be provided instead.

4. Build

The makefiles for each TA is available in the build/<platform>/<TA> directory.


5. File structure

build
        Contains necessary make files to build a TA. It also contains example
        TA build files as reference.

delivery
        Contains the API and headers to access the APIs used by the TA.

user_ta
        Contains the user mode TA framework code and example TAs.

