IMAGE := `docker images -a | grep greenfx-db | tr -s ' ' | cut -f 3 -d " "`

all: .image-stamp

.image-stamp: Dockerfile bootstrap.sh
	docker build -t="green/greenfx-base" .
	touch .image-stamp

clean:
	rm -rf *~ .image-stamp
