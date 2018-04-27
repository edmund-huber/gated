VERSION = $(shell git rev-parse HEAD)
CXXFLAGS = -std=c++11 -Wall -Werror -DVERSION='"$(VERSION)"'

SDL := "$(shell pwd)/sdl"
SDL_IMAGE := "$(shell pwd)/sdl_image"
gated: dummy.cc entity.cc layer.cc loop.cc main.cc realm.cc splash.cc ui.cc | sdl sdl_image
	$(CXX) \
		$(CXXFLAGS) $(shell sdl/bin/sdl2-config --cflags) \
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
