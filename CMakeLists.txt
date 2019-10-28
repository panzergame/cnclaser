cmake_minimum_required(VERSION 2.8)

project(cnclaser)

find_program(AVR_CC avr-gcc)
find_program(AVR_CXX avr-g++)
find_program(AVR_OBJCOPY avr-objcopy)
find_program(AVR_SIZE_TOOL avr-size)
find_program(AVR_OBJDUMP avr-objdump)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})

SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

# AVR target and upload settings.
set(AVR_MCU atmega328p)
set(AVR_H_FUSE 0xd9)
set(AVR_L_FUSE 0xe2)

set(AVR_PROGRAMMER arduino)
set(AVR_UPLOADTOOL avrdude)
set(AVR_UPLOADTOOL_PORT /dev/ttyUSB0)
set(AVR_UPLOADTOOL_BAUDRATE 115200)
set(AVR_UPLOADTOOL_OPTIONS -p ${AVR_MCU} -c ${AVR_PROGRAMMER} -b ${AVR_UPLOADTOOL_BAUDRATE})

set(MCU_SPEED "16000000UL")

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)


# /usr/lib/ccache/bin/avr-g++  -DF_CPU=16000000UL  -O2   -fpack-struct -fshort-enums -Wall -pedantic -pedantic-errors -funsigned-char -funsigned-bitfields -ffunction-sections -c -lprintf_flt -lscanf_flt -mmcu=atmega328p -o CMakeFiles/main-atmega328p.elf.dir/src/usart.cpp.obj -c /home/tristan/Compilation/cnclaser/src/usart.cpp

# /usr/lib/ccache/bin/avr-g++  -O2  -Wl,-u,vfprintf -lprintf_flt -Wl,-u,vfscanf -lscanf_flt -mmcu=atmega328p -Wl,--gc-sections -mrelax -Wl,-Map,main-atmega328p.map CMakeFiles/main-atmega328p.elf.dir/src/usart.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/stepper.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/laser.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/parser.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/rasterizer.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/timer.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/pwm.cpp.obj CMakeFiles/main-atmega328p.elf.dir/src/main.cpp.obj  -o main-atmega328p.elf

# /usr/bin/avr-objcopy -j .text -j .data -O ihex main-atmega328p.elf main-atmega328p.hex

# avrdude -p atmega328p -c arduino -b 115200 -U flash:w:main-atmega328p.hex -P /dev/ttyUSB0


if(CMAKE_BUILD_TYPE MATCHES Release)
   set(CMAKE_C_FLAGS_RELEASE "-O2")
   set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
   set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf")
endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

if(CMAKE_BUILD_TYPE MATCHES Debug)
   set(CMAKE_C_FLAGS_DEBUG "-O0 -save-temps -g -gdwarf-3 -gstrict-dwarf")
   set(CMAKE_CXX_FLAGS_DEBUG "-O0 -save-temps -g -gdwarf-3 -gstrict-dwarf")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(TARGET_NAME main)
set(ELF_FILE ${TARGET_NAME}.elf)
set(HEX_FILE ${TARGET_NAME}.hex)

add_executable(${ELF_FILE}
	src/usart.cpp
	src/stepper.cpp
	src/laser.cpp
	src/parser.cpp
	src/rasterizer.cpp
	src/timer.cpp
	src/pwm.cpp
	src/main.cpp
)

target_include_directories(${ELF_FILE} PRIVATE include)

target_compile_options(${ELF_FILE} PRIVATE
	-DF_CPU=${MCU_SPEED}
	-fpack-struct
	-fshort-enums
	-Wall
	-pedantic
	-pedantic-errors
	-funsigned-char
	-funsigned-bitfields
	-ffunction-sections
	-lprintf_flt
	-lscanf_flt
	-mmcu=${AVR_MCU}
)

target_link_options(${ELF_FILE} PRIVATE
	-Wl,-u,vfprintf
	-lprintf_flt
	-Wl,-u,vfscanf
	-lscanf_flt
	-mmcu=${AVR_MCU}
	-Wl,--gc-sections -mrelax
)

add_custom_command(OUTPUT ${HEX_FILE}
	COMMAND
		${AVR_OBJCOPY} -j .text -j .data -O ihex ${ELF_FILE} ${HEX_FILE}
	DEPENDS ${ELF_FILE}
)

add_custom_target(
	install
	${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_OPTIONS}
		-U flash:w:${HEX_FILE}
		-P ${AVR_UPLOADTOOL_PORT}
	DEPENDS ${HEX_FILE}
	COMMENT "Uploading ${HEX_FILE} to ${AVR_MCU} using ${AVR_PROGRAMMER}"
)
