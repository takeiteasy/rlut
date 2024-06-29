default: all

rlut-tui-lirary:
	$(CXX) -shared -fpic \
			-x objective-c++ \
			src/rlut.cpp \
			deps/imtui/imtui-impl-ncurses.cpp \
			deps/imtui/imtui-impl-text.cpp \
			deps/imgui/*.cpp \
			-Ideps/ \
			-fobjc-arc \
			-lncurses \
			-o build/librlut-tui.dylib

rlut-tui-test: rlut-tui-lirary
	$(CC) -Isrc aux/test.c -Lbuild -lrlut-tui -o build/rlut-tui

all: rlut-tui-lirary rlut-tui-test
