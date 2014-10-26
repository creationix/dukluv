dukluv: build/Makefile
	cmake --build build --config Debug

build/Makefile: CMakeLists.txt uv.cmake
	cmake -H. -Bbuild

link: build/Makefile dukluv
	ln -sf build/dukluv /usr/local/bin/dukluv

test: dukluv
	build/dukluv unit-tests.js

clean:
	rm -rf build
