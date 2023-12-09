# 这是一个CMakeList.txt文件

# EXECUTABLE_OUTPUT_PAT---宏表示可执行程序的输出路径
# CMAKE_CURRENT_SOURCE_DIR---宏表示当前访问的 CMakeLists.txt 文件所在的路径
# PROJECT_SOURCE_DIR---宏对应的值就是我们在使用cmake命令时，后面紧跟的目录，一般是工程的根目录

# 指定最低版本号
cmake_minimum_required(VERSION 3.0)

# 指定c++标准
set(DCMAKE_CXX_STANDARD 11)

# 项目名称
project(haibara)

# 指定可执行程序的输出路径---宏EXECUTABLE_OUTPUT_PATH (若路径不存在则自动生成)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# 搜索文件一---使用aux_source_directory 命令可以查找某个路径下的所有源文件
# 搜索 src 目录下的源文件---aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

# 包含头文件
include_directories(${PROJECT_SOURCE_DIR}/inc)

# 搜索文件二---file(GLOB (非递归)/GLOB_RECURSE (递归) 变量名 要搜索的文件路径和文件类型)
# file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# file(GLOB MAIN_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/src/*.c)

# 根据源文件指定生成的可执行程序的名字---字符串拼接
# string(REPLACE .c .out app ${SRC_LIST}) --- 错误

# 指定要链接的动态库的路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 遍历源文件列表，为每个文件生成一个对应的可执行程序
foreach(source_file ${SRC_LIST})
    # 获取源文件的基本文件名（不包括路径和扩展名）
    get_filename_component(app ${source_file} NAME_WE)
    # 添加可执行程序并关联源文件
    add_executable(${app}.out ${source_file})
    # 连接动态库
    target_link_libraries(${app}.out wrap pthread)
endforeach()

# get_filename_component(app ${SRC_LIST} NAME_WE)

# 关于要搜索的文件路径和类型可加双引号，也可不加---file(GLOB MAIN_HEAD "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# 将所有用到的源文件赋值给SRC_LIST---set(SRC_LIST main.cpp add.cpp)

# 生成可执行程序---引用变量SRC_LIST
# add_executable(${app}.out ${SRC_LIST})

# 连接动态库
# target_link_libraries(${app}.out wrap pthread)
