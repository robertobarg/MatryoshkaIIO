#!/bin/bash

#
# ./scr/batch_multicfg.sh scr/batch.sh tprob tpinstsets/tpisetA P batchcfgs.csv runmulticfgs
# ./scr/batch_multidir.sh scr/batch_multicfg.sh scr/batch.sh tprob inputcsvs/opt230118_NWC.csv expts230118NWC scr/rtraspalgs.R
# ./scr/batch_multidir.sh scr/batch_multicfg.sh scr/batch.sh tprobtrace2 inputcsvs/opt230118trace2.csv expts230118trace2 scr/rtraspalgs.R
#

SHSCRMC=`readlink -e ${1}`
SHSCR=`readlink -e ${2}`
EXEC=`readlink -e ${3}`
DIRFL=`readlink -e ${4}`
DDIR=${5}
PRS=${6}

LGFN=$(echo $(basename $DDIR) | cut -f 1 -d ".")
LGFN="expr_${LGFN}.log"

mkdir -vp $DDIR
cp -v $DIRFL $DDIR/
touch $DDIR/$LGFN
cat /dev/null > $DDIR/$LGFN

cd $DDIR
echo "Working directory $DDIR ..."
echo "Ready to run batching scripts ${SHSCRMC} ..."
echo "Log file $LGFN"

while read l
do
    CD=$(readlink -e $(echo $l | cut -f 1 -d ';'))
    CFGS=$(echo $l | cut -f 2 -d ';') && (echo $CFGS | sed -e 's/:/\n/g' > mycfg.csv)
    ITP=$(echo $l | cut -f 3 -d ';')
    
    $SHSCRMC $SHSCR $EXEC $CD $ITP mycfg.csv $DDIR $PRS
    
done < $DIRFL
echo "All done"

cd ..
