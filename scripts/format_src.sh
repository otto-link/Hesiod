#!/bin/bash

# directories to be formatted (recursive search)
DIRS="Hesiod/include Hesiod/src Hesiod/generate_node_snapshot"
# FORMAT_CMD="clang-format --style=LLVM -i {}"
FORMAT_CMD="clang-format -style=file:scripts/clang_style -i {}"

for D in ${DIRS}; do
    find ${D}/. -type f \( -iname \*.hpp -o -iname \*.cpp \) -exec ${FORMAT_CMD} \;
done

cmake-format -i CMakeLists.txt
cmake-format -i doc/CMakeLists.txt
