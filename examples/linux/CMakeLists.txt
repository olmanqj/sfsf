# Set the minimum version of CMake that can be used
# To find the cmake version run
cmake_minimum_required(VERSION 3.5)

# Project name
project (SFSF_Simple_App_Example)

# Create a sources variable with a link to all cpp files to compile
file(GLOB_RECURSE SFSF_SOURCES 
    #App
    app/*.c
    #SFSF Services
    ../../src/*.c
    #SFSF Port for Linux
    ../../ports/linux/*.c
    #CSP
    libcsp/src/*.c   
)


# Create a sources variable for the Ground Station
file(GLOB_RECURSE GS_SOURCES 
    #Ground Station
    ground_station.c
    #CSP
    libcsp/src/*.c   
)


# Add an executable for the sfsf app
add_executable(sfsf_example ${SFSF_SOURCES})

# Add an executable for the Ground Station
add_executable(ground_station ${GS_SOURCES})

# Linker options
target_link_libraries(sfsf_example PUBLIC -lpthread)
target_link_libraries(ground_station PUBLIC -lpthread)

# Set the direcoties that should be included in the build command for this target
target_include_directories(sfsf_example PRIVATE 
 	#App
	${PROJECT_SOURCE_DIR}/app
	#SFSF Services
	${PROJECT_SOURCE_DIR}/../../
	${PROJECT_SOURCE_DIR}/../../include
	#SFSF Port for Linux
	${PROJECT_SOURCE_DIR}/../../ports/linux
	#CSP
	${PROJECT_SOURCE_DIR}/libcsp/include
	${PROJECT_SOURCE_DIR}/libcsp/include/drivers
	${PROJECT_SOURCE_DIR}/libcsp/include/arch
	${PROJECT_SOURCE_DIR}/libcsp/include/arch/posix
	${PROJECT_SOURCE_DIR}/libcsp/include/interfaces
	${PROJECT_SOURCE_DIR}/libcsp/src
	${PROJECT_SOURCE_DIR}/libcsp/src/crypto
	${PROJECT_SOURCE_DIR}/libcsp/src/transport
)

# Includes for the ground station
target_include_directories(ground_station PRIVATE 
	#CSP
	${PROJECT_SOURCE_DIR}/libcsp/include
	${PROJECT_SOURCE_DIR}/libcsp/include/drivers
	${PROJECT_SOURCE_DIR}/libcsp/include/arch
	${PROJECT_SOURCE_DIR}/libcsp/include/arch/posix
	${PROJECT_SOURCE_DIR}/libcsp/include/interfaces
	${PROJECT_SOURCE_DIR}/libcsp/src
	${PROJECT_SOURCE_DIR}/libcsp/src/crypto
	${PROJECT_SOURCE_DIR}/libcsp/src/transport
)


