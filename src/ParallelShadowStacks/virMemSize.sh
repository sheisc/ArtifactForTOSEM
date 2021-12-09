#!/bin/bash
# ./vmsize.sh ./ | tee vmsize.txt






TESTCPU2006PATH=$1 
#exes=`ls  ${TESTCPU2006PATH}/instr`
preName="NONE"
max=0
while((1 != 0))
do 		
	#running=1
		
	name=firefox
	pids=`ps -AT | grep ${name} | grep -v "grep" | awk '{print $1}' | tr "\n" " "`	
	if [ "$preName" != "$name" ]	
	then
		echo "${name} pids=${pids} max=${max} ..."
		preName=${name}
	fi
	#echo "${name} pids=${pids} max=${max} ..."
	#while((${running} != 0))
	#do
		#echo "${name} pid = ${pid}"
		for pid in $pids
		do		
			vmsize=`cat /proc/$pid/statm 2>/dev/null | awk '{print $1}'`
			if [ "$vmsize" != "" ]
			then
				#echo $vmsize
				if [ $vmsize -gt $max ]
				then
					max=$vmsize
					#echo "${e} pid = ${pid}, max=${max} vmsize = ${vmsize}"
					echo "UNSW_EDU_AU_STAT ${name} ${pid} ${max} ${vmsize}"
				fi
			fi
			# if [ $vmsize == 0 ]
			# then
			# 	max=0
			# 	echo "max = 0"
			# fi
		done
	#done				
done

