core.so: core.c
	gcc -Wall -shared -o core.so core.c

.PHONY: clean

clean:
	rm *.so
