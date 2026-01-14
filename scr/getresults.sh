#!/bin/bash
DB=${1}
SQL=$(readlink -e ${2})
DIR=$(readlink -e ${3})

TBNM=$(basename $DIR)
RESF=$TBNM.csv.raw

TBNM=$(echo "$TBNM" | tr '[:upper:]' '[:lower:]')
TBNM=$(echo "$TBNM" | tr '.' '_')

find "${DIR}/" -type f -name "*.optres" -exec cat {} \;  > $RESF
sed -i -e "s/_/ /g" $RESF

psql -c "drop table if exists ${TBNM}" $DB
psql -v tabname=${TBNM} -f ${SQL}/resultproc.sql $DB

psql -d "$DB" -v ON_ERROR_STOP=1 <<EOF
\copy "${TBNM}" FROM '$(readlink -e $RESF)' WITH NULL AS 'nan' DELIMITER ' ' CSV;
EOF

psql -v tab=${TBNM} -f ${SQL}/query.sql $DB > $TBNM.csv

echo "Results in file $TBNM.csv"
