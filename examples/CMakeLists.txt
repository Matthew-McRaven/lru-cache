###########################################################
## BINARIES
###########################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples)

########################################
# TARGETS
########################################

add_executable(fibonacci-basic fibonacci-basic.cpp)
add_executable(statistics statistics.cpp)
add_executable(callbacks callbacks.cpp)
add_executable(wrap wrap.cpp)
