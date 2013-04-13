cmake_minimum_required(VERSION 2.8)
project(blinieCPP)

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
SET(COMMON_FLAGS "-O0 -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fsingle-precision-constant -Wall")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS}  -fno-rtti  -fno-exceptions -std=gnu++0x")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS "-lm -Wl,-Map=Executable.map,--cref -Wl,--gc-sections \"-T${CMAKE_SOURCE_DIR}/STM32F4_LIB_CMSIS/stm32f4_flash.ld\" -static")

#remove -rdynamic to link static
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS) 
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

# Debug Options
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}  -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g")

# add libs
add_subdirectory (STM32F4_LIB_CMSIS)
add_subdirectory (STM32F4_LIB_PeriphLib)

# add -I Headers for LIBS
include_directories("STM32F4_LIB_CMSIS/inc")
include_directories("STM32F4_LIB_PeriphLib/inc")

include_directories("src")

# add source files from folder
AUX_SOURCE_DIRECTORY("src/" SRCS)
AUX_SOURCE_DIRECTORY("src/syscalls/" SRCSCalls)
AUX_SOURCE_DIRECTORY("src/periph/" SRCSPeri)
AUX_SOURCE_DIRECTORY("src/util/" SRCSUtil)

# define target
add_executable(blinieCPP ${SRCS} ${SRCSCalls} ${SRCSPeri} ${SRCSUtil})
target_link_libraries (blinieCPP STM32F4_LIB_CMSIS STM32F4_LIB_PeriphLib)
