FLAGS = -Wall -std=gnu99 -g

all: copy greyscale gaussian_blur edge scale image_filter 

copy: copy.o bitmap.o
	gcc ${FLAGS} -o $@ $^ -lm

greyscale: greyscale.o bitmap.o
	gcc ${FLAGS} -o $@ $^ -lm

gaussian_blur: gaussian_blur.o bitmap.o
	gcc ${FLAGS} -o $@ $^ -lm

edge: edge.o bitmap.o
	gcc ${FLAGS} -o $@ $^ -lm

scale: scale.o bitmap.o
	gcc ${FLAGS} -o $@ $^ -lm

image_filter: image_filter.o
	gcc ${FLAGS} -o $@ $^ -lm

%.o: %.c bitmap.h
	gcc ${FLAGS} -c $<

clean:
	rm *.o image_filter copy greyscale gaussian_blur edge scale

test:
	mkdir images -p
	./copy < dog.bmp > images/dog_copy.bmp
	./greyscale < dog.bmp > images/dog_greyscale.bmp
	./gaussian_blur < dog.bmp > images/dog_gaussian_blur.bmp
	./edge < dog.bmp > images/dog_edge.bmp
	./scale < dog.bmp > images/dog_scale.bmp
