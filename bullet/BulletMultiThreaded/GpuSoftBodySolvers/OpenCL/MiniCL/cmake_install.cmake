# Install script for directory: /Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/just3obad/Downloads/bullet-2.82-r2704/bullet-build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/libBulletSoftBodySolvers_OpenCL_Mini.2.82.dylib"
    "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/libBulletSoftBodySolvers_OpenCL_Mini.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libBulletSoftBodySolvers_OpenCL_Mini.2.82.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libBulletSoftBodySolvers_OpenCL_Mini.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND "/usr/bin/install_name_tool"
        -id "libBulletSoftBodySolvers_OpenCL_Mini.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletCollision/libBulletCollision.2.82.dylib" "libBulletCollision.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletDynamics/libBulletDynamics.2.82.dylib" "libBulletDynamics.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletMultiThreaded/libBulletMultiThreaded.2.82.dylib" "libBulletMultiThreaded.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/BulletSoftBody/libBulletSoftBody.2.82.dylib" "libBulletSoftBody.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/LinearMath/libLinearMath.2.82.dylib" "libLinearMath.2.82.dylib"
        -change "/Users/just3obad/Downloads/bullet-2.82-r2704/src/MiniCL/libMiniCL.2.82.dylib" "libMiniCL.2.82.dylib"
        "${file}")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "bullet-build/"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

