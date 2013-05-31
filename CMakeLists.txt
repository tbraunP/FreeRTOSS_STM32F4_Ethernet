cmake_minimum_required(VERSION 2.8)
project(freeRTOSDemo)
SET(TARGET "freeRTOSDemo")

#custom command to use objcopy to create .bin files out of ELF files
function(make_firmware INPUT)
              add_custom_command(TARGET ${INPUT}
                      COMMAND arm-none-eabi-objcopy -O binary ${INPUT} ${TARGET}.bin
                      COMMENT "objcopying to make mbed compatible firmware")
              set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${TARGET}.bin)
endfunction(make_firmware)


INCLUDE(CMakeForceCompiler)
 
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
 
find_program(LINKER arm-none-eabi-ld)
find_program(OBJCOPY arm-none-eabi-objcopy)
find_program(OBJDUMP arm-none-eabi-objdump)
SET(CMAKE_AR arm-none-eabi-ar)
SET(CMAKE_RANLIB arm-none-eabi-ranlib)

# Set compiler and linker options
SET(LIBS_FLAG "-DHSE_VALUE=8000000 -D__FPU_PRESENT=1 -D__FPU_USED=1 -DUSE_STDPERIPH_DRIVER -DUSE_STM32_DISCOVERY")
SET(COMMON_FLAGS "-O0 -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fsingle-precision-constant -Wall ${LIBS_FLAG} -Wunused-value")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS}  -fno-rtti  -fno-exceptions -std=gnu++0x")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS "-lm -Wl,-Map=${TARGET}.map,--cref -Wl,--gc-sections \"-T${CMAKE_SOURCE_DIR}/lib/STM32F4_LIB_CMSIS/stm32f4_flash.ld\" -static")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${TARGET}.map)

#remove -rdynamic to link static
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS) 
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

# Debug Options
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}  -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g")

# add libs in subdirectory using their CMakefile to build the libs
add_subdirectory (lib/STM32F4_LIB_CMSIS)
add_subdirectory (lib/STM32F4_LIB_PeriphLib)
add_subdirectory (lib/FreeRTOSV7.4.2)

# add -I compile headers for LIBS
include_directories("lib/STM32F4_LIB_CMSIS/inc")
include_directories("lib/STM32F4_LIB_PeriphLib/inc")

# freertos include files
include_directories("lib/FreeRTOSV7.4.2/FreeRTOS/Source/include")
include_directories("lib/FreeRTOSV7.4.2/FreeRTOS/Source/portable/GCC/ARM_CM4F")

SET(FREERTOS ${FREERTOS_SRCS} ${FREERTOS_PORT_SRCS})

# lwIP Stack 
## Includes
include_directories("lib/lwipCore/src/include")
include_directories("lib/lwipCore/src/include/ipv4")
include_directories("lib/lwipCore/src/include/ipv6")

# add files to compile
AUX_SOURCE_DIRECTORY("lib/lwipCore/src/api" lwip_API_SRC)
AUX_SOURCE_DIRECTORY("lib/lwipCore/src/core" lwip_CORE)
AUX_SOURCE_DIRECTORY("lib/lwipCore/src/core/ipv4" lwipCORE_SRC_IPV4)
AUX_SOURCE_DIRECTORY("lib/lwipCore/src/core/ipv6" lwipCORE_SRC_IPV6)
AUX_SOURCE_DIRECTORY("lib/lwipCore/src/netif" lwip_NETIF)

# lwip portage code for stm32f4_flash
include_directories("lib/lwip_STM32f4Port/port/STM32F4x7")
include_directories("lib/lwip_STM32f4Port/port/STM32F4x7/FreeRTOS")

AUX_SOURCE_DIRECTORY("lib/lwip_STM32f4Port/port/STM32F4x7/FreeRTOS" lwip_STM32F4x7)

SET(lwipSRC ${lwip_API_SRC} ${lwip_CORE} ${lwipCORE_SRC_IPV4} ${lwipCORE_SRC_IPV6} ${lwip_NETIF} ${lwip_STM32F4x7})

# STM32F4x7_ETH_Driver
include_directories("lib/STM32F4x7_ETH_Driver/inc/")

AUX_SOURCE_DIRECTORY("lib/STM32F4x7_ETH_Driver/src" STM32F4x7_ETH_DRIVER)


# default files
include_directories("inc")
include_directories("src")

# add source files from folder
AUX_SOURCE_DIRECTORY("src" SRCS)
#AUX_SOURCE_DIRECTORY("src/syscalls/" SRCSCalls)
#AUX_SOURCE_DIRECTORY("src/periph/" SRCSPeri)
#AUX_SOURCE_DIRECTORY("src/util/" SRCSUtil)


# define target
add_executable(${TARGET}.elf ${SRCS} ${lwipSRC} ${STM32F4x7_ETH_DRIVER})
target_link_libraries (${TARGET}.elf m c STM32F4_LIB_CMSIS STM32F4_LIB_PeriphLib FREERTOS_742)

make_firmware(${TARGET}.elf)
