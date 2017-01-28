dir_name=/Users/g-2017/SamurAI_Coding
side=first
arm=2
cat ${dir_name}/Python/label1-1000.txt ${dir_name}/Python/label1001-2000.txt ${dir_name}/Python/label2001-3000.txt ${dir_name}/Python/label3001-4000.txt ${dir_name}/Python/label4001-5000.txt ${dir_name}/Python/label5001-6000.txt ${dir_name}/Python/label6001-7000.txt ${dir_name}/Python/label7001-8000.txt ${dir_name}/Python/label8001-9000.txt ${dir_name}/Python/label9001-10000.txt > ${dir_name}/manager/label.txt
sort ${dir_name}/manager/label.txt | uniq > ${dir_name}/manager/label_0_${arm}.txt

while read LINE; do
    COLM1=`echo ${LINE} | cut -d , -f 1`
    if [ -n "$COLM1" ] ; then
        mkdir ${dir_name}/traindata/${side}/${arm}/$COLM1
    fi
done < ${dir_name}/manager/label_0_${arm}.txt
