#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Error: This script requires exactly 2 arguments" >&2
    echo "Usage: $0 <arg1> <arg2>" >&2
    exit 1
fi

# executable file
EXE=${1}
IDIR=$(readlink -e ${2})

if [[ $(realpath --relative-base="$PWD" "$IDIR") == /* ]]; then
    echo "Error: $IDIR is not a subdirectory of $PWD"
    exit 1
fi

# Continue with your script here
echo "Safe to proceed"

# number of precesses
PRS=3

# prepare file of direcitives
echo "${IDIR}/MicroscopyImages;_ALG_ 18111 5 46 518400 0;_OF_" > dirfile.csv
echo "${IDIR}/Shapes;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/GRFrough;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/ClassicImages;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/GRFmoderate;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/GRFsmooth;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/LogitGRF;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/CauchyDensity;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/LogGRF;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv
echo "${IDIR}/WhiteNoise;_ALG_ 18111 5 46 518400 0;_OF_" >> dirfile.csv

# clear instance list files
[[ -n "$IDIR" ]] && [[ -d "$IDIR" ]] && find "${IDIR}/" -type f -regex ".*/list[0-9]+\.txt$" -exec rm -v {} \;

# generate file for list of sizes
echo  "32"  > listsizes.txt 
echo  "64" >> listsizes.txt 
echo "128" >> listsizes.txt 
echo "256" >> listsizes.txt 

# generate instance list file
./scr/gendotmarklists.sh ${IDIR} listsizes.txt

# run matryoshka experiments: OF 1
DDIR=$(mktemp -d -p $(pwd) kukly.of1.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/1/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 2
DDIR=$(mktemp -d -p $(pwd) kukly.of2.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/2/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 3
DDIR=$(mktemp -d -p $(pwd) kukly.of3.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/3/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 4
DDIR=$(mktemp -d -p $(pwd) kukly.of4.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/4/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

