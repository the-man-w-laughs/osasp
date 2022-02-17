if [ $# -eq 4 ] ; then 
	find $1 -type f -size +$2b -size -$3b -printf "%p, %f, %s\n" > $4
	printf "Number of viewed files:" ; find $1 -type f | wc -l
else
	echo "Wrong number of parameters!" > &2
	echo "Command format: (serch directory) (min size) (max size) (out file name)" > &2
fi
