#!/bin/bash

if [ "$1" == "" ]; then
	echo I need a date in the form of YYYY_MM_DD
	exit
fi

logfiles=`ls ../log/*.log | grep -v $1`
prefixes=""

echo "Zipping files"
for logfile in $logfiles; do
	echo "Zipping $logfile"
	bzip2 $logfile
	thisprefix=`echo $logfile | cut -c-17` # 10 for date, 7 for ../log/
	found=FALSE
	for prefix in $prefixes; do
		if [ "$prefix" == "$thisprefix" ]; then
			found=TRUE
		fi
	done
	if [ $found == FALSE ]; then
		prefixes="$prefixes $thisprefix"
	fi
done

echo "Putting logfiles into folder"
for prefix in $prefixes; do
	echo Folder $prefix
	mkdir $prefix
	files=$prefix"*.bz2"
	for file in `ls $files`; do
		mv $file $prefix
	done
done
