# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/willsnew/cmake-3.7.0-rc3-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /home/willsnew/cmake-3.7.0-rc3-Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/willsnew/桌面/RISC-V-sim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/willsnew/桌面/RISC-V-sim

# Include any dependencies generated for this target.
include CMakeFiles/riscv_sim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/riscv_sim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/riscv_sim.dir/flags.make

CMakeFiles/riscv_sim.dir/main.cpp.o: CMakeFiles/riscv_sim.dir/flags.make
CMakeFiles/riscv_sim.dir/main.cpp.o: main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/willsnew/桌面/RISC-V-sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/riscv_sim.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/riscv_sim.dir/main.cpp.o -c /home/willsnew/桌面/RISC-V-sim/main.cpp

CMakeFiles/riscv_sim.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/riscv_sim.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/willsnew/桌面/RISC-V-sim/main.cpp > CMakeFiles/riscv_sim.dir/main.cpp.i

CMakeFiles/riscv_sim.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/riscv_sim.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/willsnew/桌面/RISC-V-sim/main.cpp -o CMakeFiles/riscv_sim.dir/main.cpp.s

CMakeFiles/riscv_sim.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/riscv_sim.dir/main.cpp.o.requires

CMakeFiles/riscv_sim.dir/main.cpp.o.provides: CMakeFiles/riscv_sim.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/riscv_sim.dir/build.make CMakeFiles/riscv_sim.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/riscv_sim.dir/main.cpp.o.provides

CMakeFiles/riscv_sim.dir/main.cpp.o.provides.build: CMakeFiles/riscv_sim.dir/main.cpp.o


CMakeFiles/riscv_sim.dir/elf.cpp.o: CMakeFiles/riscv_sim.dir/flags.make
CMakeFiles/riscv_sim.dir/elf.cpp.o: elf.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/willsnew/桌面/RISC-V-sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/riscv_sim.dir/elf.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/riscv_sim.dir/elf.cpp.o -c /home/willsnew/桌面/RISC-V-sim/elf.cpp

CMakeFiles/riscv_sim.dir/elf.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/riscv_sim.dir/elf.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/willsnew/桌面/RISC-V-sim/elf.cpp > CMakeFiles/riscv_sim.dir/elf.cpp.i

CMakeFiles/riscv_sim.dir/elf.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/riscv_sim.dir/elf.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/willsnew/桌面/RISC-V-sim/elf.cpp -o CMakeFiles/riscv_sim.dir/elf.cpp.s

CMakeFiles/riscv_sim.dir/elf.cpp.o.requires:

.PHONY : CMakeFiles/riscv_sim.dir/elf.cpp.o.requires

CMakeFiles/riscv_sim.dir/elf.cpp.o.provides: CMakeFiles/riscv_sim.dir/elf.cpp.o.requires
	$(MAKE) -f CMakeFiles/riscv_sim.dir/build.make CMakeFiles/riscv_sim.dir/elf.cpp.o.provides.build
.PHONY : CMakeFiles/riscv_sim.dir/elf.cpp.o.provides

CMakeFiles/riscv_sim.dir/elf.cpp.o.provides.build: CMakeFiles/riscv_sim.dir/elf.cpp.o


CMakeFiles/riscv_sim.dir/decode.cpp.o: CMakeFiles/riscv_sim.dir/flags.make
CMakeFiles/riscv_sim.dir/decode.cpp.o: decode.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/willsnew/桌面/RISC-V-sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/riscv_sim.dir/decode.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/riscv_sim.dir/decode.cpp.o -c /home/willsnew/桌面/RISC-V-sim/decode.cpp

CMakeFiles/riscv_sim.dir/decode.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/riscv_sim.dir/decode.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/willsnew/桌面/RISC-V-sim/decode.cpp > CMakeFiles/riscv_sim.dir/decode.cpp.i

CMakeFiles/riscv_sim.dir/decode.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/riscv_sim.dir/decode.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/willsnew/桌面/RISC-V-sim/decode.cpp -o CMakeFiles/riscv_sim.dir/decode.cpp.s

CMakeFiles/riscv_sim.dir/decode.cpp.o.requires:

.PHONY : CMakeFiles/riscv_sim.dir/decode.cpp.o.requires

CMakeFiles/riscv_sim.dir/decode.cpp.o.provides: CMakeFiles/riscv_sim.dir/decode.cpp.o.requires
	$(MAKE) -f CMakeFiles/riscv_sim.dir/build.make CMakeFiles/riscv_sim.dir/decode.cpp.o.provides.build
.PHONY : CMakeFiles/riscv_sim.dir/decode.cpp.o.provides

CMakeFiles/riscv_sim.dir/decode.cpp.o.provides.build: CMakeFiles/riscv_sim.dir/decode.cpp.o


CMakeFiles/riscv_sim.dir/vmm.cpp.o: CMakeFiles/riscv_sim.dir/flags.make
CMakeFiles/riscv_sim.dir/vmm.cpp.o: vmm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/willsnew/桌面/RISC-V-sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/riscv_sim.dir/vmm.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/riscv_sim.dir/vmm.cpp.o -c /home/willsnew/桌面/RISC-V-sim/vmm.cpp

CMakeFiles/riscv_sim.dir/vmm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/riscv_sim.dir/vmm.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/willsnew/桌面/RISC-V-sim/vmm.cpp > CMakeFiles/riscv_sim.dir/vmm.cpp.i

CMakeFiles/riscv_sim.dir/vmm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/riscv_sim.dir/vmm.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/willsnew/桌面/RISC-V-sim/vmm.cpp -o CMakeFiles/riscv_sim.dir/vmm.cpp.s

CMakeFiles/riscv_sim.dir/vmm.cpp.o.requires:

.PHONY : CMakeFiles/riscv_sim.dir/vmm.cpp.o.requires

CMakeFiles/riscv_sim.dir/vmm.cpp.o.provides: CMakeFiles/riscv_sim.dir/vmm.cpp.o.requires
	$(MAKE) -f CMakeFiles/riscv_sim.dir/build.make CMakeFiles/riscv_sim.dir/vmm.cpp.o.provides.build
.PHONY : CMakeFiles/riscv_sim.dir/vmm.cpp.o.provides

CMakeFiles/riscv_sim.dir/vmm.cpp.o.provides.build: CMakeFiles/riscv_sim.dir/vmm.cpp.o


# Object files for target riscv_sim
riscv_sim_OBJECTS = \
"CMakeFiles/riscv_sim.dir/main.cpp.o" \
"CMakeFiles/riscv_sim.dir/elf.cpp.o" \
"CMakeFiles/riscv_sim.dir/decode.cpp.o" \
"CMakeFiles/riscv_sim.dir/vmm.cpp.o"

# External object files for target riscv_sim
riscv_sim_EXTERNAL_OBJECTS =

riscv_sim: CMakeFiles/riscv_sim.dir/main.cpp.o
riscv_sim: CMakeFiles/riscv_sim.dir/elf.cpp.o
riscv_sim: CMakeFiles/riscv_sim.dir/decode.cpp.o
riscv_sim: CMakeFiles/riscv_sim.dir/vmm.cpp.o
riscv_sim: CMakeFiles/riscv_sim.dir/build.make
riscv_sim: CMakeFiles/riscv_sim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/willsnew/桌面/RISC-V-sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable riscv_sim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/riscv_sim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/riscv_sim.dir/build: riscv_sim

.PHONY : CMakeFiles/riscv_sim.dir/build

CMakeFiles/riscv_sim.dir/requires: CMakeFiles/riscv_sim.dir/main.cpp.o.requires
CMakeFiles/riscv_sim.dir/requires: CMakeFiles/riscv_sim.dir/elf.cpp.o.requires
CMakeFiles/riscv_sim.dir/requires: CMakeFiles/riscv_sim.dir/decode.cpp.o.requires
CMakeFiles/riscv_sim.dir/requires: CMakeFiles/riscv_sim.dir/vmm.cpp.o.requires

.PHONY : CMakeFiles/riscv_sim.dir/requires

CMakeFiles/riscv_sim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/riscv_sim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/riscv_sim.dir/clean

CMakeFiles/riscv_sim.dir/depend:
	cd /home/willsnew/桌面/RISC-V-sim && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/willsnew/桌面/RISC-V-sim /home/willsnew/桌面/RISC-V-sim /home/willsnew/桌面/RISC-V-sim /home/willsnew/桌面/RISC-V-sim /home/willsnew/桌面/RISC-V-sim/CMakeFiles/riscv_sim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/riscv_sim.dir/depend

