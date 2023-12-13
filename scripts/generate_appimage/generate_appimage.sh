#!/bin/bash

rm -rf AppDir

wget -nc https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -P ${2}
chmod 0744 ${2}/linuxdeploy-x86_64.AppImage

./linuxdeploy-x86_64.AppImage --appdir AppDir --executable ${3}/bin/hesiod --output appimage -d ${1}/hesiod.desktop -i ${1}/icon_hesiod.png

HASH=`git rev-parse HEAD`
HASH=${HASH:0:7}

DATE=$(date '+%Y-%m-%d')

cp hesiod-x86_64.AppImage ${3}/bin/hesiod-x86_64_${DATE}_${HASH}.AppImage
