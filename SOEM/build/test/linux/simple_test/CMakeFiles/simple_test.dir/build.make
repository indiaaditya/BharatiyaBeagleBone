﻿# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

# Include any dependencies generated for this target.
include test\linux\simple_test\CMakeFiles\simple_test.dir\depend.make
# Include any dependencies generated by the compiler for this target.
include test\linux\simple_test\CMakeFiles\simple_test.dir\compiler_depend.make

# Include the progress variables for this target.
include test\linux\simple_test\CMakeFiles\simple_test.dir\progress.make

# Include the compile flags for this target's objects.
include test\linux\simple_test\CMakeFiles\simple_test.dir\flags.make

test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.obj: test\linux\simple_test\CMakeFiles\simple_test.dir\flags.make
test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.obj: "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\linux\simple_test\simple_test.c"
test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.obj: test\linux\simple_test\CMakeFiles\simple_test.dir\compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/linux/simple_test/CMakeFiles/simple_test.dir/simple_test.c.obj"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test"
	$(CMAKE_COMMAND) -E cmake_cl_compile_depends --dep-file=CMakeFiles\simple_test.dir\simple_test.c.obj.d --working-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test" --filter-prefix="Note: including file: " -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /showIncludes /FoCMakeFiles\simple_test.dir\simple_test.c.obj /FdCMakeFiles\simple_test.dir\ /FS -c "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\linux\simple_test\simple_test.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple_test.dir/simple_test.c.i"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe > CMakeFiles\simple_test.dir\simple_test.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\linux\simple_test\simple_test.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple_test.dir/simple_test.c.s"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\simple_test.dir\simple_test.c.s /c "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\linux\simple_test\simple_test.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

# Object files for target simple_test
simple_test_OBJECTS = \
"CMakeFiles\simple_test.dir\simple_test.c.obj"

# External object files for target simple_test
simple_test_EXTERNAL_OBJECTS =

test\linux\simple_test\simple_test.exe: test\linux\simple_test\CMakeFiles\simple_test.dir\simple_test.c.obj
test\linux\simple_test\simple_test.exe: test\linux\simple_test\CMakeFiles\simple_test.dir\build.make
test\linux\simple_test\simple_test.exe: soem.lib
test\linux\simple_test\simple_test.exe: test\linux\simple_test\CMakeFiles\simple_test.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable simple_test.exe"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test"
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\simple_test.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\mt.exe --manifests -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\simple_test.dir\objects1.rsp @<<
 /out:simple_test.exe /implib:simple_test.lib /pdb:"E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test\simple_test.pdb" /version:0.0 /machine:X86 /debug /INCREMENTAL /subsystem:console   -LIBPATH:"E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\oshw\win32\wpcap\Lib"  ..\..\..\soem.lib wpcap.lib Packet.lib Ws2_32.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

# Rule to build all files generated by this target.
test\linux\simple_test\CMakeFiles\simple_test.dir\build: test\linux\simple_test\simple_test.exe
.PHONY : test\linux\simple_test\CMakeFiles\simple_test.dir\build

test\linux\simple_test\CMakeFiles\simple_test.dir\clean:
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test"
	$(CMAKE_COMMAND) -P CMakeFiles\simple_test.dir\cmake_clean.cmake
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"
.PHONY : test\linux\simple_test\CMakeFiles\simple_test.dir\clean

test\linux\simple_test\CMakeFiles\simple_test.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\linux\simple_test" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\linux\simple_test\CMakeFiles\simple_test.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : test\linux\simple_test\CMakeFiles\simple_test.dir\depend

