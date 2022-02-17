if [ $# -ne 2 ] ; then
	echo "Wrong number of parameters!"; echo "Command format: (file with code) (compiled file name)" > &2
else
	gcc "$1" -o "$2"
	if [ $? -eq 0 ] ; then 
		./"$2"
	fi
fi

