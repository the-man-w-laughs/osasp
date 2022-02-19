if [ $# -eq 3 ] ; then 
	find "$2" -name "*.$3" -printf " %f\n" | sort -d >"$1"
else
	echo "Wrong number of parameters!
Command format: (out file name) (search dir) (extension)." >&2
fi

