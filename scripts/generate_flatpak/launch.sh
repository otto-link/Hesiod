#!/bin/sh

# Due to not being able to change the data dir, we have to get a bit creative
export HESIOD_TMP_DIR=$(mktemp -d)
cd $HESIOD_TMP_DIR

ln -s /app/hesiod/data data
ln -s /app/hesiod/hesiod hesiod

export QTWEBENGINEPROCESS_PATH=/app/bin/QtWebEngineProcess

LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libOpenCL.so.1 ./hesiod
