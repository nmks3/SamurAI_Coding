n=0
while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
    if [ -n "$COLM1" ] ; then
        m=$((n%2))
        if [ $m -eq 0 ]; then
            echo ${COLM1}
        fi
    fi
    n=$((n+1))
done < /Users/g-2017/SamuraiCoding/Python/train.txt