all:
	(cd base; make all)
	(cd log; make all)
	(cd db; make all)
	(cd fuse; make all)
	(cd ticks-oanda; make all)
	(cd bot; make all)

clean:
	(cd base; make clean)
	(cd log; make clean)
	(cd db; make clean)
	(cd fuse; make clean)
	(cd ticks-oanda; make clean)
	(cd bot; make clean)
	rm -rf *~

