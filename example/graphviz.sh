#!/bin/bash
usage() {
    echo "$(basename ${0}) converts every dot file to a svg file using dot command

Usage:
        $(basename ${0}) dots_dir   
    "
}

if [ $# -eq 0 ] || [ "$1" = "-h" ]; then
    usage
    exit 0
fi

dot_files=`find $1 -type f -name "*.dot"`

for file in $dot_files;
do
    filename=`echo $file | cut -d '.' -f 1`
    svg="${filename}.svg"
    dot -Tsvg $file > ${svg}
done