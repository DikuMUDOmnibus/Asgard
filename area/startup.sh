#!/bin/bash

# Program specs.
area_fdr=~/mud/area
exe_file=asgard
port=4242

# Other variables
day="nothing at the moment"

# Ensure we're in the right directory.
cd $area_fdr

echo "Starting startup script for $exe_file attached on $port..."

if [ -e shutdown.txt ]; then
	rm -rf shutdown.txt
fi

while true; do
	# Starting the mud if its not up.
	proc=`ps ux | grep "$exe_file $port" | grep -v grep`
	if [ "$proc" == "" ]; then
		echo "Starting $exe_file attached on $port..."
		# Unlimited core dumps.
		ulimit -c unlimited
		./$exe_file $port &
	fi

	# Zip logfiles every day.
	current_day=`date +%j`
	if [ "$day" != "$current_day" ]; then
		day=$current_day;
		./organise_logs.sh `date +%Y_%m_%d`
	fi

	# Loop every 20 seconds.
	sleep 20

	# Break if the shutdown marker file is found.
	if [ -e shutdown.txt ]; then
		break
	fi
done

