# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Users/ASUS/esp/esp-idf/components/bootloader/subproject"
  "D:/PROJECT/Project_group/station/build/bootloader"
  "D:/PROJECT/Project_group/station/build/bootloader-prefix"
  "D:/PROJECT/Project_group/station/build/bootloader-prefix/tmp"
  "D:/PROJECT/Project_group/station/build/bootloader-prefix/src/bootloader-stamp"
  "D:/PROJECT/Project_group/station/build/bootloader-prefix/src"
  "D:/PROJECT/Project_group/station/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/PROJECT/Project_group/station/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/PROJECT/Project_group/station/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
