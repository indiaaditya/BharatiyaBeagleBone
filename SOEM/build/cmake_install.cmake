# Install script for directory: E:/Proff/Clients/Tekno Valves/Endurance2/SOEM

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SOEM")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/soem.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake"
         "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/CMakeFiles/Export/39806c66e6e7fd9076eb39407f12ee6f/soemConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/soem/cmake" TYPE FILE FILES "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/CMakeFiles/Export/39806c66e6e7fd9076eb39407f12ee6f/soemConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/soem/cmake" TYPE FILE FILES "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/CMakeFiles/Export/39806c66e6e7fd9076eb39407f12ee6f/soemConfig-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/soem" TYPE FILE FILES
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercat.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatbase.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatcoe.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatconfig.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatconfiglist.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatdc.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercateoe.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatfoe.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatmain.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatprint.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercatsoe.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/soem/ethercattype.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/osal/osal.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/osal/win32/inttypes.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/osal/win32/osal_defs.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/osal/win32/osal_win32.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/osal/win32/stdint.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/oshw/win32/nicdrv.h"
    "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/oshw/win32/oshw.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/test/simple_ng/cmake_install.cmake")
  include("E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/test/linux/slaveinfo/cmake_install.cmake")
  include("E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/test/linux/eepromtool/cmake_install.cmake")
  include("E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/test/linux/simple_test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "E:/Proff/Clients/Tekno Valves/Endurance2/SOEM/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
