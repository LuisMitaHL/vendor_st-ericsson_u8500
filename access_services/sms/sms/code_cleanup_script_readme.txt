/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


The code in these directories has been run through the following script to ensure that its
formatting is consistent.


<start script>
#!/bin/sh
# 
# cleanup.sh
#
# find *.[ch] files and apply k&r indent, etc., and
# strip any unix new lines
#
#
 
FILES=`find . -name '*.[ch]'`;
for i in $FILES; do
    echo "+ fixing up $i"
    # No longer using dos2unix as it is not available on some (all?) Ubuntu 10.04 builds.
    # However it was probably simlinked to fromdos anyway, so use that as a direct replacement.
    #dos2unix $i
    fromdos $i
    indent -kr -nut -l200 -bfda $i
done

FILES=`find . -name '*~'`;
for i in $FILES; do
    echo "+ removing backup $i"
    rm -f $i
done
<end script>
