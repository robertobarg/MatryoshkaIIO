#!/bin/bash

ROOTDIR=$(readlink -e ${1})
size_list=$(readlink -e ${2})
lstag=${3}

cd $ROOTDIR

#find . -type f -name "list${lstag}*.txt" -exec rm -v {} \;

while read size
do
    find . -mindepth 1 -maxdepth 1 -type d -print0 | while read -d $'\0' instdir
    do
        (cd $instdir
        LISTFN=list${size}${lstag}.txt
        printf "Create file $(pwd)/$LISTFN ... "
	#size=$(expr $size + 0)
        find . -type f -name "data${size}*.csv" -exec basename {} \; | sed -e "s|^|${size};${ROOTDIR}/c${size}${lstag}.txt;|" > $LISTFN && echo "OK" || echo "KO"        
        sort -t';' -k3,3 $LISTFN  > tmplist.txt
	    mv tmplist.txt $LISTFN
	)
    done
done < $size_list

