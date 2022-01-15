#!/bin/bash

red='\033[0;31m' &&
green='\033[0;32m' &&
yellow='\033[1;33m' &&
noColor='\033[0m' &&

echo "delete directory performance if exit" &&
sudo rm -r performance
echo "Create directory performance" &&
mkdir performance &&

echo "Install required packages:" &&
#install sysstat to collect CPU and RAM and I/O usage.
echo "installing sysstat:" &&
sudo apt-get install sysstat &&

#edit the /etc/defaulte/sysstat file and set ENABLED="false" to ENABLED="true"
echo "enable collecting data of system performance:" &&
sudo sed -i '9d' /etc/default/sysstat &&
sudo sed -i '8 a ENABLED="true"' /etc/default/sysstat &&
echo -e "${green}successful${noColor}" &&

#Know if the workload is a Docker container or a normal process?
workload=0 &&
echo "Is your workload a normal process? Then enter 1, or it is a docker container? Then enter 2" &&
read workloadType &&
if [ $workloadType -eq 1 ]
then
	echo "Enter the workload name:"
	read workload
	workoadpid=$(pgrep "$workload")
	while [ "$workoadpid" == "" ]
	do
		echo please start your workload!
		sleep 2s
		workoadpid=$(pgrep "$workload")
	done
else
	if [ $workloadType -eq 2 ]
	then
		echo "Enter the process ID of the container of your workload:"
		read workload
		sudo docker start $workload
	else
		echo "please enter just 1 or 2!"
		exit
	fi
fi

wait


#Get CPU characteristics:
#we should extract core Max Speed and core current speed, Core Count, Core Enabled, Thread Count from CPU_char.txt file.
echo "Get CPU characteristics:" &&
echo "Save CPU characteristics in the file CPU_char.txt" &&
sudo dmidecode -q > performance/1.CPU_char.txt &


#Get Network characteristics:
#we should extract capacity: 100Mbit/s, width: 64 bits of the Network in the phisical Cable.
#and size: 10Gbit/s available capacity in your pc. from Network_char.txt file.
echo "Get Network characteristics:" &&
echo "Save Network characteristics in the file Network_char.txt" &&
sudo lshw -C network > performance/5.Network_char.txt &


#Get Disk usage:
#we should extract Use% Disk_usage.txt file.
echo "Get Disk usage" &&
echo "Save Disk usage in the file Disk_usagetxt" &&
df -BM --total > performance/7.Disk_usage.txt &


#Get CPU  Usage:
#we should extract %idle and %iowait from CPU_usage.txt file.
echo "Get CPU Usage:" &&
echo "Save CPU usage in the file CPU_usage.txt" &&
sar -u 1 > performance/2.CPU_usage.txt &



#Get RAM  Usage:
#we should extract kbmemfree, kbmemused, kbavail, total RAM = kbmemfree + kbmemused + kbavail and kbcommit from RAM_usage.txt file.
echo "Get RAM Usage:" &&
echo "Save RAM usage in the file RAM_usage.txt" &&
sar -r 1 > performance/3.RAM_usage.txt &


#Get SWAP  Usage:
#we should extract kbswpfree, kbswpused and total = kbswpfree + kbswpused from SWAP_usage.txt file.
echo "Get SWAP Usage:" &&
echo "Save SWAP usage in the file SWAP_usage.txt" &&
sar -S 1 > performance/4.SWAP_usage.txt &


#Get Network Usage:
#we should extract rxkB/s, txkB/s, rxcmp/s, txcmp/s and rxmcst/s of each network from Network_usage.txt file.
echo "Get Network Usage:" &&
echo "Save Network usage in the file Network_usage.txt" &&
sar -n DEV 1 > performance/6.Network_usage.txt &

#wait until the monitored program is finished
if [ $workloadType -eq 1 ]
then
	workoadpids=$(pgrep $workload) &&
	while [ "$workoadpids" != "" ]
	do
		echo "running"
		sleep 2s
		workoadpids=$(pgrep $workload)

		if [ "$workoadpids" == "" ]
		then
			sarpid=$(pgrep sar)
			echo $sarpid
			sudo kill -INT $sarpid
		fi
	done
else
	dockerContainer=$(sudo docker ps | grep $workload)
	while [ "$dockerContainer" != "" ]
	do
		echo "running"
		sleep 2s

		dockerContainer=$(sudo docker ps | grep $workload)
		if [ "$dockerContainer" == "" ]
		then
			sarpid=$(pgrep sar)
			echo $sarpid
			sudo kill -INT $sarpid
		fi
	done
fi &&


#extract needed lines for CPU characteristics:
#Count the lines in the file 1.CPU_char.txt
echo "count the size of 1.CPU_char.txt file" &&
lineNumber=$(wc -l performance/1.CPU_char.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines:
echo -e "${red}delete unneeded line in 1.CPU_char.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne 109 -a $line -ne 110 -a $line -ne 116 -a $line -ne 117 -a $line -ne 118 -a $line -ne 128 -a $line -ne 132 -a $line -ne 133 -a $line -ne 143 -a $line -ne 147 -a $line -ne 148 -a $line -ne 158 -a $line -ne 162 -a $line -ne 163 ]
	then
		sed -i "${line}d" performance/1.CPU_char.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/1.CPU_char.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 14 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for CPU_usage:
#Count the lines in the file 2.CPU_usage.txt
echo "count the size of 2.CPU_usage.txt file" &&
lineNumber=$(wc -l performance/2.CPU_usage.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines:
echo -e "${red}delete unneeded line in 2.CPU_usage.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne $lineNumber ] && [ $line -ne 3 ]
	then
		sed -i "${line}d" performance/2.CPU_usage.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/2.CPU_usage.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 2 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for RAM_usage:
#Count the lines in the file 3.RAM_usage.txt
echo "count the size of 3.RAM_usage.txt file" &&
lineNumber=$(wc -l performance/3.RAM_usage.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines:
echo -e "${red}delete unneeded line in 3.RAM_usage.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne $lineNumber ] && [ $line -ne 3 ]
	then
		sed -i "${line}d" performance/3.RAM_usage.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/3.RAM_usage.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 2 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for SWAP_usage:
#Count the lines in the file 4.SWAP_usage.txt
echo "count the size of 4.SWAP_usage.txt file" &&
lineNumber=$(wc -l performance/4.SWAP_usage.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines:
echo -e "${red}delete unneeded line in 4.SWAP_usage.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne $lineNumber ] && [ $line -ne 3 ]
	then
		sed -i "${line}d" performance/4.SWAP_usage.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/4.SWAP_usage.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 2 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for Network_char:
#Count the lines in the file 5.Network_char.txt
echo "count the size of 5.Network_char.txt file" &&
lineNumber=$(wc -l performance/5.Network_char.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines:
echo -e "${red}delete unneeded line in 5.Network_char.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne 10 ] && [ $line -ne 11 ] && [ $line -ne 35 ]
	then
		sed -i "${line}d" performance/5.Network_char.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/5.Network_char.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 3 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for Network_usage:
#Count the lines in the file 6.Network_usage.txt
echo "count the size of 6.Network_usage.txt file" &&
lineNumber=$(wc -l performance/6.Network_usage.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines (let the last 7 lines):
echo -e "${red}delete unneeded line in 6.Network_usage.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne $lineNumber ] && [ $line -ne $(($lineNumber - 1 )) ] && [ $line -ne $(($lineNumber - 2 )) ] && 
	   [ $line -ne $(($lineNumber - 3 )) ] && [ $line -ne $(($lineNumber - 4 )) ] && [ $line -ne $(($lineNumber - 5 )) ]
	then
		sed -i "${line}d" performance/6.Network_usage.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/6.Network_usage.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 6 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&


#extract needed lines for Disk_usage.
#Count the lines in the file 7.Disk_usage.txt
echo "count the size of 7.Disk_usage.txt file" &&
lineNumber=$(wc -l performance/7.Disk_usage.txt) &&
#delete all characters after the first space.
lineNumber=${lineNumber% *} &&
#delete unneeded lines (let the last 7 lines):
echo -e "${red}delete unneeded line in 7.Disk_usage.txt${noColor}" &&
for index in $(seq 1 $lineNumber)
do
	line=$(($lineNumber - $index + 1))
	if [ $line -ne 1 ] && [ $line -ne $lineNumber ]
	then
		sed -i "${line}d" performance/7.Disk_usage.txt
	fi
done &&

#verify if unneeded lines are deleted.
lineNumber=$(wc -l performance/7.Disk_usage.txt) &&
lineNumber=${lineNumber% *} &&
if [ $lineNumber -eq 2 ]
then
	echo -e "${green}successful${noColor}"
else
	echo -e "${red}unsuccessful${noColor}"
fi &&

echo "Give read and write premission" &&
sudo chmod +w performance &&
echo "Given"