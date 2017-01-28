CPPFLAGS= -pthread -std=c++11 -g
CPP= g++

OBJECTS= main.o battlefield.o game.o \
	commentedStream.o errors.o

TARGET= gameManager

all: ${TARGET}

gameManager: ${OBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
