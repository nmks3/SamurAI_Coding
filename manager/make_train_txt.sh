cd /Users/g-2017/SamuraiCoding/traindata/first/2/

ls > /Users/g-2017/SamuraiCoding/manager/filename_label.txt

n=0
while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
    if [ -n "$COLM1" ] ; then
        ls ./$COLM1 > /Users/g-2017/SamuraiCoding/manager/filename_label1.txt
        while read LINE; do
        	COLM2=`echo ${LINE} | cut -d , -f 1`
        	if [ -n "$COLM2" ] ; then
        		echo /Users/g-2017/SamuraiCoding/traindata/first/2/${COLM1}/${COLM2} ${n} 
        	fi
        done < /Users/g-2017/SamuraiCoding/manager/filename_label1.txt
    fi
    n=$((n+1))
done < /Users/g-2017/SamuraiCoding/manager/filename_label.txt