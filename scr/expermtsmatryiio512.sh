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

# run matryoshka experiments: OF 1 Size 512
SIZE=512

DDIR=$(mktemp -d -p $(pwd) kukly512.of1.XXXXXXXX)

echo "rd 18111 5 46 518400 0" > $DDIR/k512of1.cfg

IFNM=$(./scr/gendotmarkinstV3.sh CauchyDensity $SIZE 1 ${IDIR}/CauchyDensity/data${SIZE}_1005.csv ${IDIR}/CauchyDensity/data${SIZE}_1007.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh CauchyDensity $SIZE 1 ${IDIR}/CauchyDensity/data${SIZE}_1010.csv ${IDIR}/CauchyDensity/data${SIZE}_1005.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh CauchyDensity $SIZE 1 ${IDIR}/CauchyDensity/data${SIZE}_1010.csv ${IDIR}/CauchyDensity/data${SIZE}_1007.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh ClassicImages $SIZE 1 ${IDIR}/ClassicImages/data${SIZE}_1005.csv ${IDIR}/ClassicImages/data${SIZE}_1001.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh ClassicImages $SIZE 1 ${IDIR}/ClassicImages/data${SIZE}_1005.csv ${IDIR}/ClassicImages/data${SIZE}_1006.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh ClassicImages $SIZE 1 ${IDIR}/ClassicImages/data${SIZE}_1006.csv ${IDIR}/ClassicImages/data${SIZE}_1001.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved")
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFmoderate $SIZE 1 ${IDIR}/GRFmoderate/data${SIZE}_1004.csv ${IDIR}/GRFmoderate/data${SIZE}_1006.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFmoderate $SIZE 1 ${IDIR}/GRFmoderate/data${SIZE}_1010.csv ${IDIR}/GRFmoderate/data${SIZE}_1004.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFmoderate $SIZE 1 ${IDIR}/GRFmoderate/data${SIZE}_1010.csv ${IDIR}/GRFmoderate/data${SIZE}_1006.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFrough $SIZE 1 ${IDIR}/GRFrough/data${SIZE}_1001.csv ${IDIR}/GRFrough/data${SIZE}_1006.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFrough $SIZE 1 ${IDIR}/GRFrough/data${SIZE}_1001.csv ${IDIR}/GRFrough/data${SIZE}_1009.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFrough $SIZE 1 ${IDIR}/GRFrough/data${SIZE}_1006.csv ${IDIR}/GRFrough/data${SIZE}_1009.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFsmooth $SIZE 1 ${IDIR}/GRFsmooth/data${SIZE}_1006.csv ${IDIR}/GRFsmooth/data${SIZE}_1002.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFsmooth $SIZE 1 ${IDIR}/GRFsmooth/data${SIZE}_1009.csv ${IDIR}/GRFsmooth/data${SIZE}_1002.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh GRFsmooth $SIZE 1 ${IDIR}/GRFsmooth/data${SIZE}_1009.csv ${IDIR}/GRFsmooth/data${SIZE}_1006.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogGRF $SIZE 1 ${IDIR}/LogGRF/data${SIZE}_1003.csv ${IDIR}/LogGRF/data${SIZE}_1004.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogGRF $SIZE 1 ${IDIR}/LogGRF/data${SIZE}_1010.csv ${IDIR}/LogGRF/data${SIZE}_1003.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogGRF $SIZE 1 ${IDIR}/LogGRF/data${SIZE}_1010.csv ${IDIR}/LogGRF/data${SIZE}_1004.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogitGRF $SIZE 1 ${IDIR}/LogitGRF/data${SIZE}_1005.csv ${IDIR}/LogitGRF/data${SIZE}_1002.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogitGRF $SIZE 1 ${IDIR}/LogitGRF/data${SIZE}_1005.csv ${IDIR}/LogitGRF/data${SIZE}_1009.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh LogitGRF $SIZE 1 ${IDIR}/LogitGRF/data${SIZE}_1009.csv ${IDIR}/LogitGRF/data${SIZE}_1002.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh MicroscopyImages $SIZE 1 ${IDIR}/MicroscopyImages/data${SIZE}_1005.csv ${IDIR}/MicroscopyImages/data${SIZE}_1007.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh MicroscopyImages $SIZE 1 ${IDIR}/MicroscopyImages/data${SIZE}_1005.csv ${IDIR}/MicroscopyImages/data${SIZE}_1009.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh MicroscopyImages $SIZE 1 ${IDIR}/MicroscopyImages/data${SIZE}_1007.csv ${IDIR}/MicroscopyImages/data${SIZE}_1009.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh Shapes $SIZE 1 ${IDIR}/Shapes/data${SIZE}_1004.csv ${IDIR}/Shapes/data${SIZE}_1010.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh Shapes $SIZE 1 ${IDIR}/Shapes/data${SIZE}_1007.csv ${IDIR}/Shapes/data${SIZE}_1004.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh Shapes $SIZE 1 ${IDIR}/Shapes/data${SIZE}_1007.csv ${IDIR}/Shapes/data${SIZE}_1010.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh WhiteNoise $SIZE 1 ${IDIR}/WhiteNoise/data${SIZE}_1002.csv ${IDIR}/WhiteNoise/data${SIZE}_1008.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh WhiteNoise $SIZE 1 ${IDIR}/WhiteNoise/data${SIZE}_1007.csv ${IDIR}/WhiteNoise/data${SIZE}_1002.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)
    
IFNM=$(./scr/gendotmarkinstV3.sh WhiteNoise $SIZE 1 ${IDIR}/WhiteNoise/data${SIZE}_1007.csv ${IDIR}/WhiteNoise/data${SIZE}_1008.csv) && \
    mv -v -t $DDIR/ $IFNM && \
    (cd $DDIR/ && ../$EXE $IFNM k512of1.cfg && echo "$IFNM solved" && rm -v $IFNM)


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
echo  "${SIZE}"  > listsizes.txt 

# generate instance list file
./scr/gendotmarklists.sh ${IDIR} listsizes.txt

# run matryoshka experiments: OF 2
DDIR=$(mktemp -d -p $(pwd) kukly512.of2.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/2/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 3
DDIR=$(mktemp -d -p $(pwd) kukly512.of3.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/3/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

# run matryoshka experiments: OF 4
DDIR=$(mktemp -d -p $(pwd) kukly512.of4.XXXXXXXX) && \
    sed -e "s/_ALG_/rd/" -e "s/_OF_/4/" dirfile.csv > matrydirfile.csv && \
    ./scr/multidir.sh scr/multicfg.sh scr/batchpl.sh $EXE matrydirfile.csv $DDIR $PRS

