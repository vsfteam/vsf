# Install script for directory: C:/Users/yongxiang/work/vsfteam/vsf/example/template/demo

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/vsf_template")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/yongxiang/scoop/apps/gcc-arm-none-eabi/current/bin/arm-none-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/btstack_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/common/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/disp_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/freetype_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/hal_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/kernel_test/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/linux_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/lvgl_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/lua_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/lwip_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/sdl2_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/tgui_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/usbd_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/usbh_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/cpu_usage_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/scsi_demo/cmake_install.cmake")
  include("C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/aic8800/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/yongxiang/work/vsfteam/vsf/build-aic8800-demo-gcc/vsf_template_bin/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
