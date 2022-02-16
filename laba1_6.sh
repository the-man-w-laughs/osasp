#(directory) (min) (max) (out file)
find $1 -type f -size +$2b -size -$3b -printf "%p, %f, %s\n" > $4
printf "Number of viewed files:" ; find $1 -type f | wc -l
