dir_name=/Users/install/ML_inoue/SamurAI_Coding
side=first
arm=0
n=0
while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
	if [ -n "$COLM1" ] ; then
		python ${dir_name}/Python/resize_image.py $COLM1
		m=$((n % 1000))
		if [ $m -eq 0 ] ; then
			echo $COLM1 | cut -c 57-60
		fi
		n=$((n+1))
　	fi
done < ${dir_name}/traindata/${side}/train${arm}.txt