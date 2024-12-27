cmake_minimum_required(VERSION 3.10)
project(libTypical_Tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找源文件和头文件
file(GLOB_RECURSE SRC_Tool_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_Tool_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE Json_libHEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/Json/*.h")

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Json)

# 设置自定义配置类型: 多配置生成器(Visual Studio, Xcode)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Debug_Win;Release_Win" CACHE STRING "" FORCE)

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
add_library(libTypical_Tool STATIC ${SRC_Tool_LIST} ${HEADER_Tool_LIST})

# 使用设置链接库
target_link_libraries(libTypical_Tool
    $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Debug/jsoncpp_static.lib>
    $<$<CONFIG:Debug_Win>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Debug/jsoncpp_static.lib>
    $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Release/jsoncpp_static.lib>
    $<$<CONFIG:Release_Win>:${CMAKE_CURRENT_SOURCE_DIR}/lib/Json/Release/jsoncpp_static.lib>
)

# 设置不同配置的编译定义
target_compile_definitions(libTypical_Tool
  PRIVATE
  $<$<CONFIG:Debug_Win>:_DEBUG;_WINDOWS>
  $<$<CONFIG:Release_Win>:NDEBUG;_WINDOWS>
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
  #对于调试和发布配置，明确地删除_WINDOWS
  target_compile_options(libTypical_Tool
    PRIVATE
    $<$<CONFIG:Debug>:/U_WINDOWS>
    $<$<CONFIG:Release>:/U_WINDOWS>
  )

  # 对于所有配置设置 UTF-8 编码
  target_compile_options(libTypical_Tool
  PRIVATE
  $<$<CONFIG:Debug>:/utf-8>
  $<$<CONFIG:Release>:/utf-8>
  $<$<CONFIG:Debug_Win>:/utf-8>
  $<$<CONFIG:Release_Win>:/utf-8>
)
endif()

# 分组源文件和头文件
source_group("HeaderFiles" FILES ${HEADER_Tool_LIST})
source_group("SourceFiles" FILES ${SRC_Tool_LIST})
source_group("Json_libHeaderFiles" FILES ${Json_libHEADER_FILES})
