cmake_minimum_required(VERSION 3.10)
project(DoublePendulum)

set(CMAKE_CXX_STANDARD 17)


message("Using Eigen...")
SET( EIGEN3_INCLUDE_DIR "src/eigen3" )
IF( NOT EIGEN3_INCLUDE_DIR )
    MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
ENDIF()
INCLUDE_DIRECTORIES ( "${EIGEN3_INCLUDE_DIR}" )

add_definitions(-DEIGEN_RUNTIME_NO_MALLOC)

add_executable(DoublePendulum main.cpp)


TARGET_LINK_LIBRARIES(DoublePendulum ${ARPACKLIBS}  ${LAPACKLIBS} ${BLASLIBS}  )
