cmake_minimum_required(VERSION 3.7)

project( CPU_Emulator_6502 )

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sub-directories where more CMakeLists.txt exist
add_subdirectory(CPU_Emulator_6502/src)
add_subdirectory(CPU_Emulator_6502/test)