dir_name=/Users/naohiro/SamurAI_Coding/traindata
side=first
arm=0

cd ${dir_name}/${side}/${arm}/
ls > ../filename_label${arm}.txt

n=0
while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
    if [ -n "$COLM1" ] ; then
        ls ./$COLM1 > ../filename_label${arm}_1.txt
        while read LINE; do
        	COLM2=`echo ${LINE} | cut -d , -f 1`
        	if [ -n "$COLM2" ] ; then
        		echo ${dir_name}/${side}/${arm}/${COLM1}/${COLM2} ${n} 
        	fi
        done < ../filename_label${arm}_1.txt
    fi
    n=$((n+1))
done < ../filename_label${arm}.txt