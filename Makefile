VERSION = $(shell git rev-parse HEAD)
CFLAGS = -std=c99 -Wall -Werror -DVERSION='"$(VERSION)"'

gated: main.c | sdl
	$(CC) $(CFLAGS) $(shell sdl/bin/sdl2-config --cflags) $(shell sdl/bin/sdl2-config --libs) -o $@ $^ -lSDL2

SDL_INSTALL_DIR := "$(shell pwd)/sdl"
sdl:
	tar xzf SDL2-2.0.8.tar.gz
	cd SDL2-2.0.8 && \
		./autogen.sh && \
		./configure --prefix=$(SDL_INSTALL_DIR) && \
		make && \
		make install
