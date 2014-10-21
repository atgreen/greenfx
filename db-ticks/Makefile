all: .image-stamp

influxdb-latest-1.x86_64.rpm:
	wget http://s3.amazonaws.com/influxdb/influxdb-latest-1.x86_64.rpm

.image-stamp: Dockerfile influxdb-latest-1.x86_64.rpm
	docker build -t="green/greenfx-db-ticks" .
	touch .image-stamp

clean:
	rm -rf *~ .image-stamp

