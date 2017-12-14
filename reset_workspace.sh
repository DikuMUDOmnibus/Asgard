#!/bin/bash

# Set to test port.
port=4242

# Set to the account the production mud runs on.
prod_username="grim"

# Other variables.
username=`whoami`
date=`date +%Y_%m_%d`
data_folders="area data gods"

# Check username - if production username then we will NOT run.
if [ "$username" == "$prod_username" ]; then
	echo "Detected that this is running in production. Exiting."
	exit
fi

echo "This script reverts changes to this folder. Ctrl-C to exit."
echo "It will also kill the currently running Asgard on port $port."

# Check whether the user wants to revert the src folder.
echo "Do you want to revert changes to src too?"
echo "Please type Y to confirm or anything else for no:"
read revert_src
if [ "$revert_src" == "Y" -o "$revert_src" == "y" ]; then
	data_folders="$data_folders src"
	echo "WILL revert src folder."
else
	echo "Will NOT revert src folder."
fi

# Clean src folder.
echo "Cleaning src folder..."
cd src
make clean
cd ../

# Organise logs.
echo "Organising logs..."
cd area
./organise_logs.sh $date
cd ../

# Kill the current Asgard on the port if running.
asgard_pid=`ps aux | grep asgard | grep $port | sed "s/^$username[ \t]*\([0-9]*\)[ \t]*.*$/\1/"`
if [ "$asgard_pid" != "" ]; then
	echo "Killing Asgard on port $port."
	kill -9 $asgard_pid
fi

# Revert changes to data files (but not src).
echo "Reverting changes..."
svn revert -R $data_folders

# Update changes to everything.
echo "Updating to HEAD..."
svn update -r HEAD

echo "Done!"
# Notify if there is the possibility of conflicts.
if [ "$revert_src" != "Y" -a "$revert_src" != "y" ]; then
	echo "Please make sure you check for any conflicts in the src folder."
fi

