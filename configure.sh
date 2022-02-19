cmake -DGLFW_BUILD_DOCS=OFF    \
      -DASSIMP_BUILD_TESTS=OFF \
      -DASSIMP_INSTALL=OFF     \
      -DBUILD_SHARED_LIBS=OFF  \
      -S . -B ./build -G"Unix Makefiles"
