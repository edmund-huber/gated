VERSION = $(shell git rev-parse HEAD)
CFLAGS = -std=c99 -D_XOPEN_SOURCE=600 -Wall -Werror -DVERSION='"$(VERSION)"'

SDL := "$(shell pwd)/sdl"
SDL_IMAGE := "$(shell pwd)/sdl_image"
gated: main.c | sdl sdl_image
	$(CC) \
		$(CFLAGS) $(shell sdl/bin/sdl2-config --cflags) \
		$(shell sdl/bin/sdl2-config --libs) -lSDL2 \
		-I$(SDL_IMAGE)/include/SDL2 -L$(SDL_IMAGE)/lib -Wl,-rpath,$(SDL_IMAGE)/lib -lSDL2_image \
		-o $@ $^

sdl:
	tar xzf SDL2-2.0.8.tar.gz
	cd SDL2-2.0.8 && \
		./autogen.sh && \
		./configure --prefix=$(SDL) && \
		make && \
		make install

sdl_image: sdl
	tar xzf SDL2_image-2.0.3.tar.gz
	cd SDL2_image-2.0.3 && \
		./autogen.sh && \
		PATH="$$PATH:$(SDL)/bin" ./configure --prefix=$(SDL_IMAGE) && \
		make && \
		make install
