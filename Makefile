all: hub push pull

hub: hub.c
	$(CC) -o hub hub.c

push: push.c
	$(CC) -o push push.c

pull: pull.c
	$(CC) -o pull pull.c

clean:
	rm -f hub push pull
