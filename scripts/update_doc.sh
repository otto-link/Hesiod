#!/bin/bash

cd build
./bin/hesiod --inventory
./bin/hesiod --snapshot
cd ..

cp -u build/*_settings.png docs/images/nodes/.
rm -f build/*_settings.png

cp -u build/*_hsd_example.png docs/images/nodes/.
rm -f build/*_hsd_example.png

cp -u Hesiod/data/examples/*.hsd docs/examples/.

cp build/node_documentation_stub.json Hesiod/data/node_documentation.json

python3 scripts/generate_node_reference.py
