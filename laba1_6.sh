if [ $# -eq 4 ] ; then 
	find $1 -type f -size +$2b -size -$3b -printf "%p, %f, %s\n" > $4
	printf "Number of viewed files:" ; find $1 -type f | wc -l
else
	echo "Wrong number of parameters!" > &2
	echo "Command format: directory) (min) (max) (out file)" > &2
fi
