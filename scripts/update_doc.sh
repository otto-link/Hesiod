#!/bin/bash

cd build
./bin/hesiod --inventory
cd ..

cp -u build/*_settings.png docs/images/nodes/.
cp build/node_documentation_stub.json Hesiod/data/node_documentation.json

python3 scripts/generate_node_reference.py
