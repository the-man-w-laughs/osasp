if [ $# -eq 3 ] ; then 
	find "$2" -name "*.$3" > "$1" 2>errorcontent
	sort "$1"
else
	echo "Wrong number of parameters!"
	echo "(out file name) (search dir) (extension)"
fi

