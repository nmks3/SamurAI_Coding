CPPFLAGS= -std=c++11 -g
CPP= g++

COMMONOBJECTS= players.o
RANDOMOBJECTS= ${COMMONOBJECTS} randomPlayer.o
TIMEOUTOBJECTS=  ${COMMONOBJECTS} timeoutPlayer.o
TOOACTIVEOBJECTS= ${COMMONOBJECTS} tooActivePlayer.o
NONCHECKINGOBJECTS= ${COMMONOBJECTS} nonCheckingPlayer.o
ABORTINGOBJECTS= ${COMMONOBJECTS} abortingPlayer.o
PLANNINGOBJECTS= planning.o
GREEDYOBJECTS= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer.o
TARGET= randomPlayer greedyPlayer timeoutPlayer tooActivePlayer nonCheckingPlayer abortingPlayer

all: ${TARGET}

randomPlayer: ${RANDOMOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

timeoutPlayer: ${TIMEOUTOBJECTS}
	${CPP} ${CPPFLAGS} -pthread -o $@ $^

tooActivePlayer: ${TOOACTIVEOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

nonCheckingPlayer: ${NONCHECKINGOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

abortingPlayer: ${ABORTINGOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer: ${GREEDYOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
