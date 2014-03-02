#!/bin/bash

./generatePEname.sh ${1} ${2} ${4} ${5}

for f in data ext split ext_ddr; do

    cp -f ${3}/Isp8500_${1}_fw_${f}.bin ${3}/Isp8500_${2}_fw_${f}.bin
    cp -f ${3}/Isp8500_${1}_fw_${f}.cde ${3}/Isp8500_${2}_fw_${f}.cde

    for t in ${1} ${2}; do
        ./bin2array.sh ${3}/Isp8500_${t}_fw_${f}.bin Isp8500_${t}_fw_${f} ${4} ${5}
        echo bin2array.sh ${3}/Isp8500_${t}_fw_${f}.bin Isp8500_${t}_fw_${f} ${4} ${5}
        echo
    done
done
