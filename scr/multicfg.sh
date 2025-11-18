#!/bin/bash

#
# ./scr/batch_multicfg.sh scr/batch.sh tprob tpinstsets/tpisetA P batchcfgs.csv runmulticfgs
#

SHSCR=`readlink -e ${1}`
EXEC=`readlink -e ${2}`
INSTD=`readlink -e ${3}`
ITP=${4}
CFGFL=`readlink -e ${5}`
DDIR=${6}
PRS=${7}

DDIR=$(mktemp -d -p $(pwd) cfg.XXXXXXXX)
cp -v $CFGFL $DDIR/

cd $DDIR
echo "Working directory $DDIR ..."
echo "Ready to run batching script ${SHSCR}"

echo "$INSTD" > info.txt
echo "Run ..."
while read l
do
    echo "$l" > mytmpopt.cfg
    $SHSCR $INSTD mytmpopt.cfg $EXEC $ITP $PRS
    rm mytmpopt.cfg
done < $CFGFL
echo "All done"

cd ..
