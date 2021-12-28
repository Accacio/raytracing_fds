##
# Ray Tracing FDS
#

all: raytracer run view

raytracer: raytracer.c
	cc raytracer.c -o raytracer

.PHONY: run clean view

run:
	./raytracer > output.ppm

clean:
	rm raytracer output.ppm

view:
	sxiv output.ppm

# end
