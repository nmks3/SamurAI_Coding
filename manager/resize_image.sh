dir_name=/Users/g-2017/SamuraiCoding
side=first
arm=2

while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
	if [ -n "$COLM1" ] ; then
		python ${dir_name}/Python/resize_image.py $COLM1
	fi
done < ${dir_name}/traindata/${side}/${arm}/train.txt