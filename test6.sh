if [ $# -eq 3 ] ; then 
	find "$2" -name "*.$3" > "$1"
	sort "$1"
else
	echo "Wrong number of parameters!" > &2
	echo "Command format: (out file name) (search dir) (extension)" > &2
fi

