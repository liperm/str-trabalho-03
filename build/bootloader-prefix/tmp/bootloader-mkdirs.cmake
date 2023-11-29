# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/felipe/esp/esp-idf/components/bootloader/subproject"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/tmp"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/src"
  "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/felipe/workspace/facul/str/trabalho03/sample_project/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
