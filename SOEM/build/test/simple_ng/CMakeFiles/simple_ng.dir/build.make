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
include test\simple_ng\CMakeFiles\simple_ng.dir\depend.make
# Include any dependencies generated by the compiler for this target.
include test\simple_ng\CMakeFiles\simple_ng.dir\compiler_depend.make

# Include the progress variables for this target.
include test\simple_ng\CMakeFiles\simple_ng.dir\progress.make

# Include the compile flags for this target's objects.
include test\simple_ng\CMakeFiles\simple_ng.dir\flags.make

test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.obj: test\simple_ng\CMakeFiles\simple_ng.dir\flags.make
test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.obj: "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\simple_ng\simple_ng.c"
test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.obj: test\simple_ng\CMakeFiles\simple_ng.dir\compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/simple_ng/CMakeFiles/simple_ng.dir/simple_ng.c.obj"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng"
	$(CMAKE_COMMAND) -E cmake_cl_compile_depends --dep-file=CMakeFiles\simple_ng.dir\simple_ng.c.obj.d --working-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng" --filter-prefix="Note: including file: " -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /showIncludes /FoCMakeFiles\simple_ng.dir\simple_ng.c.obj /FdCMakeFiles\simple_ng.dir\ /FS -c "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\simple_ng\simple_ng.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple_ng.dir/simple_ng.c.i"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe > CMakeFiles\simple_ng.dir\simple_ng.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\simple_ng\simple_ng.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple_ng.dir/simple_ng.c.s"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\simple_ng.dir\simple_ng.c.s /c "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\simple_ng\simple_ng.c"
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

# Object files for target simple_ng
simple_ng_OBJECTS = \
"CMakeFiles\simple_ng.dir\simple_ng.c.obj"

# External object files for target simple_ng
simple_ng_EXTERNAL_OBJECTS =

test\simple_ng\simple_ng.exe: test\simple_ng\CMakeFiles\simple_ng.dir\simple_ng.c.obj
test\simple_ng\simple_ng.exe: test\simple_ng\CMakeFiles\simple_ng.dir\build.make
test\simple_ng\simple_ng.exe: soem.lib
test\simple_ng\simple_ng.exe: test\simple_ng\CMakeFiles\simple_ng.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable simple_ng.exe"
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng"
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\simple_ng.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x86\mt.exe --manifests -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1429~1.301\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\simple_ng.dir\objects1.rsp @<<
 /out:simple_ng.exe /implib:simple_ng.lib /pdb:"E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng\simple_ng.pdb" /version:0.0 /machine:X86 /debug /INCREMENTAL /subsystem:console   -LIBPATH:"E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\oshw\win32\wpcap\Lib"  ..\..\soem.lib wpcap.lib Packet.lib Ws2_32.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"

# Rule to build all files generated by this target.
test\simple_ng\CMakeFiles\simple_ng.dir\build: test\simple_ng\simple_ng.exe
.PHONY : test\simple_ng\CMakeFiles\simple_ng.dir\build

test\simple_ng\CMakeFiles\simple_ng.dir\clean:
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng"
	$(CMAKE_COMMAND) -P CMakeFiles\simple_ng.dir\cmake_clean.cmake
	cd "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build"
.PHONY : test\simple_ng\CMakeFiles\simple_ng.dir\clean

test\simple_ng\CMakeFiles\simple_ng.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\test\simple_ng" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng" "E:\Proff\Clients\Tekno Valves\Endurance2\SOEM\build\test\simple_ng\CMakeFiles\simple_ng.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : test\simple_ng\CMakeFiles\simple_ng.dir\depend

