# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lewis/projects/javac-compiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lewis/projects/javac-compiler/build

# Include any dependencies generated for this target.
include CMakeFiles/javac.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/javac.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/javac.dir/flags.make

CMakeFiles/javac.dir/main.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/main.c.o: ../main.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/main.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/main.c.o   -c /home/lewis/projects/javac-compiler/main.c

CMakeFiles/javac.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/main.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/main.c > CMakeFiles/javac.dir/main.c.i

CMakeFiles/javac.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/main.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/main.c -o CMakeFiles/javac.dir/main.c.s

CMakeFiles/javac.dir/main.c.o.requires:
.PHONY : CMakeFiles/javac.dir/main.c.o.requires

CMakeFiles/javac.dir/main.c.o.provides: CMakeFiles/javac.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/main.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/main.c.o.provides

CMakeFiles/javac.dir/main.c.o.provides.build: CMakeFiles/javac.dir/main.c.o

CMakeFiles/javac.dir/hashtab.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/hashtab.c.o: ../hashtab.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/hashtab.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/hashtab.c.o   -c /home/lewis/projects/javac-compiler/hashtab.c

CMakeFiles/javac.dir/hashtab.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/hashtab.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/hashtab.c > CMakeFiles/javac.dir/hashtab.c.i

CMakeFiles/javac.dir/hashtab.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/hashtab.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/hashtab.c -o CMakeFiles/javac.dir/hashtab.c.s

CMakeFiles/javac.dir/hashtab.c.o.requires:
.PHONY : CMakeFiles/javac.dir/hashtab.c.o.requires

CMakeFiles/javac.dir/hashtab.c.o.provides: CMakeFiles/javac.dir/hashtab.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/hashtab.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/hashtab.c.o.provides

CMakeFiles/javac.dir/hashtab.c.o.provides.build: CMakeFiles/javac.dir/hashtab.c.o

CMakeFiles/javac.dir/lex.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/lex.c.o: ../lex.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/lex.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/lex.c.o   -c /home/lewis/projects/javac-compiler/lex.c

CMakeFiles/javac.dir/lex.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/lex.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/lex.c > CMakeFiles/javac.dir/lex.c.i

CMakeFiles/javac.dir/lex.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/lex.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/lex.c -o CMakeFiles/javac.dir/lex.c.s

CMakeFiles/javac.dir/lex.c.o.requires:
.PHONY : CMakeFiles/javac.dir/lex.c.o.requires

CMakeFiles/javac.dir/lex.c.o.provides: CMakeFiles/javac.dir/lex.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/lex.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/lex.c.o.provides

CMakeFiles/javac.dir/lex.c.o.provides.build: CMakeFiles/javac.dir/lex.c.o

CMakeFiles/javac.dir/parser.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/parser.c.o: ../parser.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/parser.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/parser.c.o   -c /home/lewis/projects/javac-compiler/parser.c

CMakeFiles/javac.dir/parser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/parser.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/parser.c > CMakeFiles/javac.dir/parser.c.i

CMakeFiles/javac.dir/parser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/parser.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/parser.c -o CMakeFiles/javac.dir/parser.c.s

CMakeFiles/javac.dir/parser.c.o.requires:
.PHONY : CMakeFiles/javac.dir/parser.c.o.requires

CMakeFiles/javac.dir/parser.c.o.provides: CMakeFiles/javac.dir/parser.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/parser.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/parser.c.o.provides

CMakeFiles/javac.dir/parser.c.o.provides.build: CMakeFiles/javac.dir/parser.c.o

CMakeFiles/javac.dir/slist.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/slist.c.o: ../slist.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/slist.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/slist.c.o   -c /home/lewis/projects/javac-compiler/slist.c

CMakeFiles/javac.dir/slist.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/slist.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/slist.c > CMakeFiles/javac.dir/slist.c.i

CMakeFiles/javac.dir/slist.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/slist.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/slist.c -o CMakeFiles/javac.dir/slist.c.s

CMakeFiles/javac.dir/slist.c.o.requires:
.PHONY : CMakeFiles/javac.dir/slist.c.o.requires

CMakeFiles/javac.dir/slist.c.o.provides: CMakeFiles/javac.dir/slist.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/slist.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/slist.c.o.provides

CMakeFiles/javac.dir/slist.c.o.provides.build: CMakeFiles/javac.dir/slist.c.o

CMakeFiles/javac.dir/xmem.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/xmem.c.o: ../xmem.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/xmem.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/xmem.c.o   -c /home/lewis/projects/javac-compiler/xmem.c

CMakeFiles/javac.dir/xmem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/xmem.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/xmem.c > CMakeFiles/javac.dir/xmem.c.i

CMakeFiles/javac.dir/xmem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/xmem.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/xmem.c -o CMakeFiles/javac.dir/xmem.c.s

CMakeFiles/javac.dir/xmem.c.o.requires:
.PHONY : CMakeFiles/javac.dir/xmem.c.o.requires

CMakeFiles/javac.dir/xmem.c.o.provides: CMakeFiles/javac.dir/xmem.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/xmem.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/xmem.c.o.provides

CMakeFiles/javac.dir/xmem.c.o.provides.build: CMakeFiles/javac.dir/xmem.c.o

CMakeFiles/javac.dir/print-tree.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/print-tree.c.o: ../print-tree.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/print-tree.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/print-tree.c.o   -c /home/lewis/projects/javac-compiler/print-tree.c

CMakeFiles/javac.dir/print-tree.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/print-tree.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/print-tree.c > CMakeFiles/javac.dir/print-tree.c.i

CMakeFiles/javac.dir/print-tree.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/print-tree.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/print-tree.c -o CMakeFiles/javac.dir/print-tree.c.s

CMakeFiles/javac.dir/print-tree.c.o.requires:
.PHONY : CMakeFiles/javac.dir/print-tree.c.o.requires

CMakeFiles/javac.dir/print-tree.c.o.provides: CMakeFiles/javac.dir/print-tree.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/print-tree.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/print-tree.c.o.provides

CMakeFiles/javac.dir/print-tree.c.o.provides.build: CMakeFiles/javac.dir/print-tree.c.o

CMakeFiles/javac.dir/check-tree.c.o: CMakeFiles/javac.dir/flags.make
CMakeFiles/javac.dir/check-tree.c.o: ../check-tree.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lewis/projects/javac-compiler/build/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/javac.dir/check-tree.c.o"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/javac.dir/check-tree.c.o   -c /home/lewis/projects/javac-compiler/check-tree.c

CMakeFiles/javac.dir/check-tree.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/javac.dir/check-tree.c.i"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/lewis/projects/javac-compiler/check-tree.c > CMakeFiles/javac.dir/check-tree.c.i

CMakeFiles/javac.dir/check-tree.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/javac.dir/check-tree.c.s"
	/usr/lib/ccache/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/lewis/projects/javac-compiler/check-tree.c -o CMakeFiles/javac.dir/check-tree.c.s

CMakeFiles/javac.dir/check-tree.c.o.requires:
.PHONY : CMakeFiles/javac.dir/check-tree.c.o.requires

CMakeFiles/javac.dir/check-tree.c.o.provides: CMakeFiles/javac.dir/check-tree.c.o.requires
	$(MAKE) -f CMakeFiles/javac.dir/build.make CMakeFiles/javac.dir/check-tree.c.o.provides.build
.PHONY : CMakeFiles/javac.dir/check-tree.c.o.provides

CMakeFiles/javac.dir/check-tree.c.o.provides.build: CMakeFiles/javac.dir/check-tree.c.o

# Object files for target javac
javac_OBJECTS = \
"CMakeFiles/javac.dir/main.c.o" \
"CMakeFiles/javac.dir/hashtab.c.o" \
"CMakeFiles/javac.dir/lex.c.o" \
"CMakeFiles/javac.dir/parser.c.o" \
"CMakeFiles/javac.dir/slist.c.o" \
"CMakeFiles/javac.dir/xmem.c.o" \
"CMakeFiles/javac.dir/print-tree.c.o" \
"CMakeFiles/javac.dir/check-tree.c.o"

# External object files for target javac
javac_EXTERNAL_OBJECTS =

javac: CMakeFiles/javac.dir/main.c.o
javac: CMakeFiles/javac.dir/hashtab.c.o
javac: CMakeFiles/javac.dir/lex.c.o
javac: CMakeFiles/javac.dir/parser.c.o
javac: CMakeFiles/javac.dir/slist.c.o
javac: CMakeFiles/javac.dir/xmem.c.o
javac: CMakeFiles/javac.dir/print-tree.c.o
javac: CMakeFiles/javac.dir/check-tree.c.o
javac: CMakeFiles/javac.dir/build.make
javac: CMakeFiles/javac.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable javac"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/javac.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/javac.dir/build: javac
.PHONY : CMakeFiles/javac.dir/build

CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/main.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/hashtab.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/lex.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/parser.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/slist.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/xmem.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/print-tree.c.o.requires
CMakeFiles/javac.dir/requires: CMakeFiles/javac.dir/check-tree.c.o.requires
.PHONY : CMakeFiles/javac.dir/requires

CMakeFiles/javac.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/javac.dir/cmake_clean.cmake
.PHONY : CMakeFiles/javac.dir/clean

CMakeFiles/javac.dir/depend:
	cd /home/lewis/projects/javac-compiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lewis/projects/javac-compiler /home/lewis/projects/javac-compiler /home/lewis/projects/javac-compiler/build /home/lewis/projects/javac-compiler/build /home/lewis/projects/javac-compiler/build/CMakeFiles/javac.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/javac.dir/depend

