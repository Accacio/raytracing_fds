##
# Ray Tracing FDS
#
#
all: raytracer output.ppm


-include raytracer.d
raytracer: raytracer.c
	cc raytracer.c -lm -o raytracer -MMD

.PHONY: run clean view

output.ppm: raytracer
	time ./raytracer > output.ppm

run:
	./raytracer

clean:
	rm -f raytracer output.ppm

view: output.ppm
	sxiv output.ppm

# end
