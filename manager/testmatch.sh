#!/bin/sh

#./testmatch-1.sh $1 $2 $3
while read LINE; do
    COLM2=`echo ${LINE} | cut -d , -f 1`

    case $COLM2 in
    Timed*)
      ./testmatch-1.sh $1 ${COLM1:4} ${COLM1:4} $2
     	COLM1=$COLM2
     	;;
    kill:*)
		  ;;
    ./testmatch-1.sh:*)
      ;;
	  *)
		COLM1=$COLM2
		;;
    esac
done < testmatch.txt