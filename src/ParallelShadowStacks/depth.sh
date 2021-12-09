#!/bin/bash
# ./depth.sh ./ | tee depth.txt






TESTCPU2006PATH=$1 
exes=`ls  ${TESTCPU2006PATH}/instr`
preName="NONE"
max=0
while((1 != 0))
do 		
	#running=1
		
	name=`cat /home/iron/src/cpu2006/curApp.txt`
	pids=`ps -Al | grep ${name} | awk '{print $4}' | tr "\n" " "`	
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
			depth=`sudo cat /proc/$pid/stack | wc -l`
			#echo $depth
			if [ $depth -gt $max ]
			then
				max=$depth
				#echo "${e} pid = ${pid}, max=${max} depth = ${depth}"
				echo "UNSW_EDU_AU_STAT ${name} ${pid} ${max} ${depth}"
			fi
			if [ $depth == 0 ]
			then
				max=0
				echo "max = 0"
			fi
		done
	#done				
done

