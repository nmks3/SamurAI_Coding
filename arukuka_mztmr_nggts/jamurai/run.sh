cd $(dirname $0)
FilePath=`pwd`
java -client -Xverify:none -XX:+TieredCompilation -XX:TieredStopAtLevel=1 -cp ${FilePath}/jamurai Main
