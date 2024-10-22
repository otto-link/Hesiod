#!/bin/bash
qtchooser -install qt6 $(which qmake6)
export QT_SELECT=qt6

rm -rf AppDir

wget -nc https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -P ${2}
wget -nc https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -P ${2}

chmod 0744 ${2}/linuxdeploy-x86_64.AppImage
chmod 0744 ${2}/linuxdeploy-plugin-qt-x86_64.AppImage

./linuxdeploy-x86_64.AppImage --appdir AppDir --executable ${3}/build/bin/hesiod --plugin qt --output appimage -d ${1}/hesiod.desktop -i ${1}/icon_hesiod.png

HASH=`git rev-parse HEAD`
HASH=${HASH:0:7}

DATE=$(date '+%Y-%m-%d')

cp hesiod-x86_64.AppImage ${3}/build/bin/hesiod.AppImage
