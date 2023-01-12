all: cachesim virt2phys

virt2phys: virt2phys.c
	gcc -std=c99 -g -o $@ $<

cachesim: cachesim.c
	gcc -std=c99 -g -o $@ $< memory.c

clean:
	rm -f cachesim virt2phys
