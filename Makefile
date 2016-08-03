all: gps.so gps.exe

gps.so: gps.c
	gcc -O3 -shared -fPIC -Wl,-soname,$@ -o $@ $<

gps.exe: gps.c
	gcc -O3 -o $@ $<

.PHONY: clean

clean:
	-rm gps.exe
	-rm gps.so
