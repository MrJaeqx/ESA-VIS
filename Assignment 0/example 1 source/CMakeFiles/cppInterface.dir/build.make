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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source"

# Include any dependencies generated for this target.
include CMakeFiles/cppInterface.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cppInterface.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cppInterface.dir/flags.make

CMakeFiles/cppInterface.dir/main.cpp.o: CMakeFiles/cppInterface.dir/flags.make
CMakeFiles/cppInterface.dir/main.cpp.o: main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source/CMakeFiles" $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/cppInterface.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/cppInterface.dir/main.cpp.o -c "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source/main.cpp"

CMakeFiles/cppInterface.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cppInterface.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source/main.cpp" > CMakeFiles/cppInterface.dir/main.cpp.i

CMakeFiles/cppInterface.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cppInterface.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source/main.cpp" -o CMakeFiles/cppInterface.dir/main.cpp.s

CMakeFiles/cppInterface.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/cppInterface.dir/main.cpp.o.requires

CMakeFiles/cppInterface.dir/main.cpp.o.provides: CMakeFiles/cppInterface.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/cppInterface.dir/build.make CMakeFiles/cppInterface.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/cppInterface.dir/main.cpp.o.provides

CMakeFiles/cppInterface.dir/main.cpp.o.provides.build: CMakeFiles/cppInterface.dir/main.cpp.o

# Object files for target cppInterface
cppInterface_OBJECTS = \
"CMakeFiles/cppInterface.dir/main.cpp.o"

# External object files for target cppInterface
cppInterface_EXTERNAL_OBJECTS =

cppInterface: CMakeFiles/cppInterface.dir/main.cpp.o
cppInterface: CMakeFiles/cppInterface.dir/build.make
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.8
cppInterface: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.8
cppInterface: CMakeFiles/cppInterface.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable cppInterface"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cppInterface.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cppInterface.dir/build: cppInterface
.PHONY : CMakeFiles/cppInterface.dir/build

CMakeFiles/cppInterface.dir/requires: CMakeFiles/cppInterface.dir/main.cpp.o.requires
.PHONY : CMakeFiles/cppInterface.dir/requires

CMakeFiles/cppInterface.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cppInterface.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cppInterface.dir/clean

CMakeFiles/cppInterface.dir/depend:
	cd "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source" "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source" "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source" "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source" "/home/viki/Documents/git/ESA-VIS/Assignment 0/example 1 source/CMakeFiles/cppInterface.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/cppInterface.dir/depend
