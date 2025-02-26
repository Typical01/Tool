cmake_minimum_required(VERSION 3.10)
project(libTypical)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找源文件和头文件
file(GLOB_RECURSE SRC_Tool_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE libTypical_libHEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/libTypical/*.h")
file(GLOB_RECURSE libTypical_Tool_libHEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/libTypical/**/*.h")

file(GLOB_RECURSE Json_libHEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/Json/*.h")

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Json)

# 设置自定义配置类型: 多配置生成器
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Debug_Win;Release_Win" CACHE STRING "" FORCE)
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
  endif()
endif()

# 设置输出目录到 /bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# 为不同配置设置输出目录
foreach(CONFIGURATION IN ITEMS Debug Release Debug_Win Release_Win)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIGURATION} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CONFIGURATION}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIGURATION} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CONFIGURATION}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIGURATION} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CONFIGURATION}")
endforeach()

# 创建静态库
add_library(libTypical STATIC ${SRC_Tool_LIST} ${libTypical_libHEADER_LIST} ${libTypical_Tool_libHEADER_LIST})

# 使用设置链接库
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(libTypical
        PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Debug/libjsoncpp_static.a>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Release/libjsoncpp_static.a>
    )
elseif (MSVC)
    target_link_libraries(libTypical
        PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Debug/jsoncpp_static.lib>
        $<$<CONFIG:Debug_Win>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Debug/jsoncpp_static.lib>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Release/jsoncpp_static.lib>
        $<$<CONFIG:Release_Win>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Release/jsoncpp_static.lib>
    )
endif()


# 根据不同编译器设置编译选项
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(libTypical
    PRIVATE
    -Wall
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
    $<$<CONFIG:Debug>:-O0 -g>                     # Debug: 无优化，启用调试信息
    $<$<CONFIG:Release>:-O2>                     # Release: 启用中等优化
    $<$<CONFIG:Release>:-finput-charset=UTF-8>   # 输入文件编码
    $<$<CONFIG:Release>:-fexec-charset=UTF-8>    # 输出文件编码
  )
elseif (MSVC)
  target_compile_definitions(libTypical
      PRIVATE
      _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
      _CRT_SECURE_NO_WARNINGS
  )
  target_compile_options(libTypical
      PRIVATE
      /W3
      /FS  # 添加此行以启用 /FS 选项
      $<$<CONFIG:Debug>:/Od /MDd /Zi /utf-8 /U_WINDOWS>    # Debug: 禁用优化，启用调试信息
      $<$<CONFIG:Release>:/O2 /MD /utf-8 /U_WINDOWS>       # Release: 启用优化
      $<$<CONFIG:Debug_Win>:/Od /MDd /Zi /utf-8>                    
      $<$<CONFIG:Release_Win>:/O2 /MD /utf-8>
  )
endif()


# 分组源文件和头文件
source_group("HeaderFiles" FILES ${libTypical_libHEADER_LIST})
source_group("Tool_HeaderFiles" FILES ${libTypical_Tool_libHEADER_LIST})
source_group("SourceFiles" FILES ${SRC_Tool_LIST})
source_group("Json_libHeaderFiles" FILES ${Json_libHEADER_FILES})
# source_group("icu4c_libHeaderFiles" FILES ${icu4c_libHEADER_FILES})