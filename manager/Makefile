all: managerDir playersDir

TAGS: manager/*.cpp manager/*.hpp players/*.cpp players/*.hpp
	etags $^

managerDir:
	cd manager; make

playersDir:
	cd players; make

tar: clean
	tar zcvf ../samurai.tgz .

clean:
	rm -f TAGS *~
	cd manager; make clean
	cd players; make clean
