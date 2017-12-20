all: buildrooms adventure

C = karczj.buildrooms.c
C = karczj.adventure.c

buildrooms: karczj.buildrooms.c
	gcc -g karczj.buildrooms.c -o buildrooms

adventure: karczj.adventure.c
	gcc -g karczj.adventure.c -o adventure -lpthread

clean:
	rm buildrooms adventure
