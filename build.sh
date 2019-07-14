COMMON_PATH=/home/tristan/Compilation/cnclaser
BUILD_PATH=${COMMON_PATH}/build
CACHE_PATH=${BUILD_PATH}/cache
SRC_PATH=${COMMON_PATH}/src/

SRCS="

${SRC_PATH}main.ino

"

mkdir -p build/cache

arduino-builder \
	-compile \
	-logger=machine \
	-hardware /usr/share/arduino/hardware \
	-hardware /home/tristan/.arduino15/packages \
	-tools /usr/share/arduino/hardware/tools/avr \
	-tools /home/tristan/.arduino15/packages \
	-libraries /home/tristan/Arduino/libraries \
	-fqbn=arduino:avr:nano:cpu=atmega328old \
	-build-path ${BUILD_PATH} \
	-build-cache ${CACHE_PATH} \
	-prefs=build.warn_data_percentage=75 \
	-verbose \
	${SRCS}
