dir_name=/Users/g-2017/SamurAI_Coding
side=first
arm=0
cat ${dir_name}/traindata/${side}/labels/label*.txt > ${dir_name}/manager/label.txt
sort ${dir_name}/manager/label.txt | uniq > ${dir_name}/manager/label_0_${arm}.txt

while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
    if [ -n "$COLM1" ] ; then
        mkdir ${dir_name}/traindata/${side}/${arm}/$COLM1
    fi
done < ${dir_name}/manager/label_0_${arm}.txt
