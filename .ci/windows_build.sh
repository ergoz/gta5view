#!/bin/bash

# Prepare environment variable
export GTA5VIEW_EXECUTABLE=gta5view-${EXECUTABLE_VERSION}${EXECUTABLE_ARCH}.exe && \

# Creating folders
cd ${PROJECT_DIR} && \
echo "gta5view build version is ${APPLICATION_VERSION}" && \
echo "gta5view executable is ${GTA5VIEW_EXECUTABLE}" && \
mkdir -p build && \
mkdir -p assets && \

# Starting build
cd build && \
qmake-static ${QMAKE_FLAGS_QT5} ${QMAKE_BUILD_TYPE} "DEFINES+=GTA5SYNC_BUILDCODE=\\\\\\\"${PACKAGE_CODE}\\\\\\\"" "DEFINES+=GTA5SYNC_APPVER=\\\\\\\"${APPLICATION_VERSION}\\\\\\\"" "DEFINES+=GTA5SYNC_COMMIT=\\\\\\\"${APPLICATION_COMMIT}\\\\\\\"" DEFINES+=GTA5SYNC_TELEMETRY "DEFINES+=GTA5SYNC_TELEMETRY_WEBURL=\\\\\\\"https://dev.syping.de/gta5view-userstats/\\\\\\\"" DEFINES+=GTA5SYNC_DONATION "DEFINES+=GTA5SYNC_DONATION_EMAIL=\\\\\\\"paypal/at/syping.de\\\\\\\"" ../gta5view.pro && \
make depend && \
make -j 4 && \
cp -Rf release/*.exe ${PROJECT_DIR}/assets/${GTA5VIEW_EXECUTABLE} && \
cd ${PROJECT_DIR}/assets
