#!/bin/bash

CLS=${1}
SZ=${2}
OBJF=${3}
FN1=$(readlink -e ${4})
FN2=$(readlink -e ${5})

instfile=${CLS}_1_${SZ}_1_${SZ}_${SZ}_${SZ}_$(basename ${FN1} .csv | sed "s/data${SZ}_//")$(basename ${FN2} .csv | sed "s/data${SZ}_//").txt

printf "Gen instance ${instfile} ... "
echo "$(($SZ * $SZ)) $(($SZ * $SZ)) $OBJF" > $instfile && sed -e ':a;N;$!ba;s/\n/,/g' -e "s/,/ /g" $FN1 >> $instfile && sed -e ':a;N;$!ba;s/\n/,/g' -e "s/,/ /g" $FN2 >> $instfile && echo "OK" || echo "KO"
