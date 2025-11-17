#!/bin/bash

DB=${1}
SQL=$(readlink -e ${2})
DIR=$(readlink -e ${3})

TBNM=$(basename $DIR)
RESF=$TBNM.csv.raw

echo $RESF

find "${DIR}/" -type f -name "*.optres" -exec cat {} \;  > $RESF
sed -i -e "s/_/ /g" $RESF

psql -c "drop table if exists \"${TBNM}\"" $DB
psql -v tabname=\"${TBNM}\" -f ${SQL}/resultproc.sql $DB
psql -c "copy \"${TBNM}\" from '$(readlink -e $RESF)' with null as 'nan' delimiter ' ' csv;" $DB

psql -v tab=\"${TBNM}\" -f ${SQL}/query.sql $DB > $TBNM.csv

