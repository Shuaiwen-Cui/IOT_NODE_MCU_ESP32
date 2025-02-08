# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/cshwstem/esp/esp-idf/components/bootloader/subproject"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/tmp"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/src/bootloader-stamp"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/src"
  "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/cshwstem/CSW/DEV/IOT_NODE_MCU_ESP32/TUTORIALS/ALIENTEK/05_esp_timer/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
