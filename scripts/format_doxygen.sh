#!/bin/bash
echo "- uncrustify"

for F in `find Hesiod/include/. -type f -iname \*.hpp`; do
    echo ${F}
    uncrustify -c scripts/uncrustify_config.cfg --replace ${F} --no-backup -q
done
