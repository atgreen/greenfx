all: .image-stamp

.image-stamp: Dockerfile ticks-oanda
	docker build -t="green/greenfx-ticks-oanda" .
	touch .image-stamp

clean:
	rm -rf *~ .image-stamp

ticks-oanda: main.cc config.h
	g++ -g3 -o ticks-oanda main.cc `pkg-config activemq-cpp --cflags` -lcurl `pkg-config activemq-cpp --libs`
