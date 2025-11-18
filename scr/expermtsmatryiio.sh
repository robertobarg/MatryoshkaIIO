#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Error: This script requires exactly 4 arguments" >&2
    echo "Usage: $0 <arg1> <arg2> <arg3> <arg4>" >&2
    exit 1
fi

# executable file
EXE=$(readlink -e ${1})
IDIR=$(readlink -e ${2})
CFG=$(readlink -e ${3})
# number of precesses
if [[ "${4}" =~ ^[0-9]+$ ]]; then
    PRS="${4}"
else
    PRS=1
fi

echo "$PRS parallel experiments"

# tests
if [ $? -ne 0 ] || [ -z "$EXE" ]; then
    echo "Error: File '$1' does not exist or is broken link" >&2
    exit 1
fi
if [ $? -ne 0 ] || [ -z "$IDIR" ]; then
    echo "Error: File '$2' does not exist or is broken link" >&2
    exit 1
fi
if [ $? -ne 0 ] || [ -z "$CFG" ]; then
    echo "Error: File '$3' does not exist or is broken link" >&2
    exit 1
fi


# test that data dir is a subdir
if [[ $(realpath --relative-base="$PWD" "$IDIR") == /* ]]; then
    echo "Error: $IDIR is not a subdirectory of $PWD"
    exit 1
fi

# Continue with your script here
echo "Safe to proceed"

CFGSTR=$(cat $CFG | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')

# prepare file of direcitives
echo "${IDIR}/MicroscopyImages;$CFGSTR;_OF_" > dirfile.csv
echo "${IDIR}/Shapes;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/GRFrough;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/ClassicImages;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/GRFmoderate;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/GRFsmooth;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/LogitGRF;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/CauchyDensity;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/LogGRF;$CFGSTR;_OF_" >> dirfile.csv
echo "${IDIR}/WhiteNoise;$CFGSTR;_OF_" >> dirfile.csv

# clear instance list files
[[ -n "$IDIR" ]] && [[ -d "$IDIR" ]] && find "${IDIR}/" -type f -regex ".*/list[0-9]+\.txt$" -exec rm -v {} \;

# generate file for list of sizes
echo  "32"  > listsizes.txt 
#echo  "64" >> listsizes.txt 
#echo "128" >> listsizes.txt 
#echo "256" >> listsizes.txt 

# generate instance list file
./scr/gendotmarklists.sh ${IDIR} listsizes.txt

#create dir
DDIR=$(mktemp -d -p $(pwd) matryoskhaiio4d.XXXXXXXX)
if [ $? -ne 0 ] || [ ! -d "$DDIR" ]; then
    echo "Error: Failed to create temporary directory" >&2
    exit 1
fi

# run matryoshka experiments: OF 1
sed -e "s/_OF_/1/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 2
sed -e "s/_OF_/2/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 3
sed -e "s/_OF_/3/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 4
sed -e "s/_OF_/4/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS






























