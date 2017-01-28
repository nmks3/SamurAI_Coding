#!/bin/sh
case $1 in 
  # piyo vs sinryakudegeso
  0) 
    for n in `seq $2 $3`
    do
      echo "game$n"
	    #manager/gameManager -d ~/SamuraiCoding/matchlog/game$n/input -l ~/SamuraiCoding/matchlog/game$n/log -t\
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku  -p "" -u "" -n "sinryakudegeso" 
 
      TARGET=players
      for i in {1..6}
      do
        pid=(`ps | grep -v grep | grep ${TARGET} `)
        kill -9 ${pid}
      done
      rm ./Samurai*
    done
    ;;  
  # sinryakudegeso vs piyo
  1)
    for n in `seq $2 $3`
    do
      echo "game$n"
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku  -p "" -u "" -n "sinryakudegeso" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" 
 
      TARGET=players
      for i in {1..6}
      do
        pid=(`ps | grep -v grep | grep ${TARGET} `)
        kill -9 ${pid}
      done
      rm ./Samurai*
    done
    ;;  
  2)
    for n in `seq $2 $3`
    do
      echo "game$n"
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" 
 
      TARGET=players
      for i in {1..6}
      do
        pid=(`ps | grep -v grep | grep ${TARGET} `)
        kill -9 ${pid}
      done
      rm ./Samurai*
    done
    ;;  
  3)
    for n in `seq $2 $3`
    do
      echo "game$n"
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a players/prog -p "" -u "" -n "piyo" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" 
 
      TARGET=players
      for i in {1..6}
      do
        pid=(`ps | grep -v grep | grep ${TARGET} `)
        kill -9 ${pid}
      done
      rm ./Samurai*
    done
    ;;  
    4)
    for n in `seq $2 $3`
    do
      echo "game$n"
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" 
    done
    ;;
    5)
    for n in `seq $2 $3`
    do
      echo "game$n"
      manager2/gameManager -d ~/SamurAI_Coding/matchlog/game$4/game$n/input -l ~/SamurAI_Coding/matchlog/game$4/game$n/log -t > ~/SamurAI_Coding/matchlog/game$4/game$n/logfile\
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" \
      -a ~/SamurAI_Coding/arukuka_mztmr_nggts/jamurai/run.sh -p "" -u "" -n "arukuka" 
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" \
      -a players/sinryaku -p "" -u "" -n "sinryakudegeso" 
    done
    ;;
esac