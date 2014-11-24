all: .image-stamp

.image-stamp: Dockerfile rec-ticks
	docker build -t="green/greenfx-rec-ticks" .
	touch .image-stamp

clean:
	rm -rf *~ .image-stamp

rec-ticks: main.cc config.h
	g++ -O2 -g -I influxdb-c/src -o rec-ticks main.cc `pkg-config activemq-cpp --cflags` influxdb-c/src/.libs/libinfluxdb.a `pkg-config activemq-cpp --libs` -lcurl -ljson-c
