clean:
	@echo "Cleaning cache..."
	@if [ -d cmake-build-release ]; then rm -rf cmake-build-release; fi
	@mkdir cmake-build-release

build: clean
	@echo "Building..."
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ..

compile: build
	@echo "Compiling..."
	cmake --build ./cmake-build-release --target Computer_Graphics_Cup

render:
	@echo "Rendering..."
	@./cmake-build-release/Computer_Graphics_Cup