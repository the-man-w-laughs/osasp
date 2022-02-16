if [ $# -ne 2 ] ; then
	echo "Wrong number of parameters!"
	echo "(infile) (outfile)"
else
gcc "$1" -o "$2"
if [ $? -eq 0 ] ; then 
	./"$2"
fi
fi

