#!/bin/bash


DIR=`readlink -e ${1}`
CFG=`readlink -e ${2}`
EXE=`readlink -e ${3}`
FLG=${4}
PRS=${5}

echo "Instance dir: $DIR"
echo "Configuration file: $CFG"
cat $CFG
echo "Exec file: $EXE"
echo "Instance type $FLG"

DDIR=$(mktemp -d -p $(pwd) run.XXXXXXXX)
echo "Destination directory $DDIR ..."
cd $DDIR
cp -v $CFG .

start_time=$(date +%s.%3N)

if [ -z "${LISTTAG}" ]
then
    LISTTAG=""
else
    echo "List tag: ${LISTTAG}"
fi

find $DIR -type f -name "list*${LISTTAG}.txt" -print0 | while read -d $'\0' list_file
do
    i=0
    rep=""
    del=""
    dpath=$(dirname $(readlink -e $list_file))
    echo "Processing list file $(basename $list_file) of dir $dpath ..."
    FN=$(echo $line|cut -f 3 -d ';')
    start_time_C=$(date +%s.%3N)
  
    a=1
    while read line1
    do
        FN1=$dpath/$(echo $line1|cut -f 3 -d ';')
        
        b=1
        while read line2
        do
            SZ=$(echo $line2|cut -f 1 -d ';')
            #SZ=$(expr $SZ + 0)
            FN2=$dpath/$(echo $line2|cut -f 3 -d ';')
            
            instfile=OF${FLG}_$(basename $dpath)_1_${SZ}_1_${SZ}_${SZ}_${SZ}_$(basename ${FN1} .csv | \
                sed "s/data${SZ}_//")$(basename ${FN2} .csv | sed "s/data${SZ}_//").txt

            if ((b > a))
            then
                echo "$(($SZ * $SZ)) $(($SZ * $SZ)) $FLG" > $instfile
                sed -e ':a;N;$!ba;s/\n/,/g' -e "s/,/ /g" $FN1 >> $instfile
                sed -e ':a;N;$!ba;s/\n/,/g' -e "s/,/ /g" $FN2 >> $instfile
                                    
                if (($i > 0));
                then
                    rep+=" "
                    del+=" "
                fi
                
                rep+="$EXE $instfile $CFG"
                del+="$instfile"
                
                ((i++))                    
            fi
            
            ((b++))
        done < $list_file
        
        ((a++))
    done < $list_file
    
    parallel -N 3 --joblog - -j$PRS ::: $rep
    # remove created instance files
    rm $del
        
    end_time_C=$(date +%s.%3N)
    elapsed_C=$(echo "scale=3; $end_time_C - $start_time_C" | bc)
    echo "$i experiments, elapsed batch time $elapsed_C sec."
done
