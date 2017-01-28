while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
	if [ -n "$COLM1" ] ; then
		python /Users/g-2017/SamuraiCoding/Python/resize_image.py $COLM1
	fi
done < /Users/g-2017/SamuraiCoding/traindata/first/2/train.txt