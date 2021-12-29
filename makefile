##
# Ray Tracing FDS
#
#
all: raytracer output.ppm

raytracer: raytracer.c
	cc raytracer.c -o raytracer

.PHONY: run clean view

output.ppm: raytracer
	./raytracer > output.ppm

run:
	./raytracer

clean:
	rm -f raytracer output.ppm

view: output.ppm
	sxiv output.ppm

# end
