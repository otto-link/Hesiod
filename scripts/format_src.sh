#!/bin/bash

echo "- json"

jq . Hesiod/data/node_documentation.json --indent 4 > tmp.json
mv tmp.json Hesiod/data/node_documentation.json

# directories to be formatted (recursive search)
DIRS="Hesiod/include Hesiod/src Hesiod/app"
# FORMAT_CMD="clang-format --style=LLVM -i {}"
FORMAT_CMD="clang-format -style=file:scripts/clang_style -i"

echo "- clang-format"

for D in ${DIRS}; do
    for F in `find ${D}/. -type f \( -iname \*.hpp -o -iname \*.cpp \)`; do
	echo ${F}
	${FORMAT_CMD} ${F}
    done
done

echo "- cmake-format"

cmake-format -i CMakeLists.txt
cmake-format -i Hesiod/CMakeLists.txt
