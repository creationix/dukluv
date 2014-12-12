ifneq ('x'$(shell command -v ninja),'x')
	GENERATOR_FLAGS='-GNinja'
endif

dukluv:
	cmake -H. -Bbuild $(GENERATOR_FLAGS)
	cmake --build build --config Debug

test: dukluv
	build/dukluv unit-tests.js

install: test
	install build/dukluv /usr/local/bin

uninstall:
	rm -f /usr/local/bin/dukluv

clean:
	rm -rf build
