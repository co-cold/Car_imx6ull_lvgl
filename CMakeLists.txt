#该cmakeLists.txt功能
#用于交叉编译LVGL项目移植到开发板IMX6ULL上显示

cmake_minimum_required(VERSION 3.10)# 声明此CMake配置文件的最低版本要求为3.10，低于此版本的CMake可能无法正确解析配置

set(CMAKE_SYSTEM_NAME Linux)# 设置目标系统名称为Linux，表明编译产物将运行在Linux系统上
set(CMAKE_SYSTEM_PROCESSOR arm)# 设置目标系统处理器架构为ARM，说明编译产物适用于ARM架构的设备（此处指IMX6ULL开发板）

# 指定C交叉编译器路径，使用arm-linux-gnueabihf-gcc为IMX6ULL开发板编译C代码，这需要针对不同开发板选择合适的交叉编译器
# 开发板的官方文档通常会明确指出推荐或适配的交叉编译工具链，下载合适的编译器后指定其路径
set(CMAKE_C_COMPILER /opt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
# 指定C++交叉编译器路径，使用arm-linux-gnueabihf-g++为IMX6ULL开发板编译C++代码，如果没有使用C++，此行以及下面的CXX规则可以去掉
set(CMAKE_CXX_COMPILER /opt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# 此处用于定义项目的基本信息，我仅定义项目名称为GUI_LVGL_IMX6ULL，标识当前是针对IMX6ULL的LVGL GUI项目
project(LVGL_DEMO)

set(CMAKE_C_STANDARD 99)# 设置C语言标准为C99，LVGL官方支持C99及以上标准
set(CMAKE_CXX_STANDARD 17)# 设置C++语言标准为C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)# 强制要求使用指定的C++标准，若编译器不支持则会报错

# 指定可执行文件的输出目录为项目根目录下的bin文件夹，这样，我们的编译好对于开发板的可执行文件则会存在当前目录下的bin目录下，
#${PROJECT_SOURCE_DIR}代表的是CMakeLists.txt所在的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLV_CONF_INCLUDE_SIMPLE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${PROJECT_SOURCE_DIR}")

################## 添加libyuv支持 ###################
set(LIBYUV_ROOT "/home/coco/arm-libyuv/libyuv" CACHE PATH "Path to libyuv")

# 验证libyuv路径
if(NOT EXISTS ${LIBYUV_ROOT}/libyuv.a)
    message(FATAL_ERROR "libyuv.a not found at ${LIBYUV_ROOT}")
else()
    message(STATUS "Using libyuv from: ${LIBYUV_ROOT}")
endif()

# 添加libyuv头文件路径
include_directories(${LIBYUV_ROOT}/include)

# 添加libyuv库
add_library(libyuv STATIC IMPORTED)
set_target_properties(libyuv PROPERTIES
    IMPORTED_LOCATION ${LIBYUV_ROOT}/libyuv.a
)

################## 添加 FreeType 支持 ###################
set(FREETYPE_ROOT "/home/coco/arm-freetype/_freetype_install" CACHE PATH "Path to FreeType")

# 验证 FreeType 路径
if(NOT EXISTS "${FREETYPE_ROOT}/include/freetype2/ft2build.h")
    message(FATAL_ERROR "FreeType not found at ${FREETYPE_ROOT}")
else()
    message(STATUS "Using FreeType from: ${FREETYPE_ROOT}")
endif()

# 添加 FreeType 头文件路径
include_directories(${FREETYPE_ROOT}/include)
include_directories(${FREETYPE_ROOT}/include/freetype2)
link_directories(${FREETYPE_ROOT}/lib)

###################递归处理 lvgl 内部的构建逻辑（编译 lvgl 源码、处理它的依赖等 ）###################################
# 添加lvgl子目录，CMake会自动查找该目录下的CMakeLists.txt并执行，完成LVGL库的编译和配置
add_subdirectory(lvgl) 
add_subdirectory(lv_drivers)

set(FFMPEG_LIB "/home/coco/arm-ffmpeg/_ffmpeg_install/lib")
set(X264_LIB   "/home/coco/arm-ffmpeg/_x264_install/lib")
set(ALSA_LIB   "/home/coco/arm-ffmpeg/alsa_install/lib")

link_directories(${FFMPEG_LIB} ${X264_LIB} ${ALSA_LIB})

add_subdirectory(multimedia)

#########################################头文件处理#################################################
# custom 和 generated 中的代码需要引用彼此的头文件以及 LVGL 的头文件（如 lv_conf.h、lvgl/lvgl.h 等）。通过include_directories 暴露这些头文件的路径，全局可用（对所有源文件类型，即在调用头文件的名称即可，无需查找相对位置，如#include "../../lvgl.h",添加后可以直接#include "lvgl.h",编译器会在你所添加的搜索路径下去找该头文件，就不会因为粗心导致编译错误找不到头文件了）
#添加头文件搜索路径，确保编译器能找到以下目录中的头文件
include_directories(
    ${PROJECT_SOURCE_DIR}          # 项目根目录，如上所说（包含lv_conf.h等全局配置头文件）
    ${PROJECT_SOURCE_DIR}/ui/custom   # custom目录（GUI guider生成的用户自定义代码的头文件）
    ${PROJECT_SOURCE_DIR}/ui/generated# generated目录（GUI Guider自动生成的UI头文件）
    ${PROJECT_SOURCE_DIR}/ui/generated/guider_customer_fonts # 自定义字体相关头文件
    ${PROJECT_SOURCE_DIR}/ui/generated/guider_fonts # 自动生成的字体相关头文件
    ${PROJECT_SOURCE_DIR}/lvgl     # LVGL库的头文件目录
    ${PROJECT_SOURCE_DIR}/lv_drivers     # LVGL驱动文件目录
    ${PROJECT_SOURCE_DIR}/ui/Edclock  # 添加gui guider dclock组件目录
    ${PROJECT_SOURCE_DIR}/ui/Eanalogclock  # 添加gui guider analogclock组件目录
    ${PROJECT_SOURCE_DIR}/ui/Ekeyboard  # 添加gui guider keyboard组件目录
    ${PROJECT_SOURCE_DIR}/dev  # 添加设备代码
)

#########################################源文件处理##################################################
# GUI Guider 生成的代码分散在 custom（用户逻辑）和 generated（自动 UI 代码）中，需递归收集所有 .c 文件
# 递归查找custom目录下所有.c文件，并存入CUSTOM_SOURCES变量
file(GLOB_RECURSE CUSTOM_SOURCES "${PROJECT_SOURCE_DIR}/ui/custom/*.c")
# 递归查找generated目录下所有.c文件，并存入GENERATED_SOURCES变量
file(GLOB_RECURSE GENERATED_SOURCES "${PROJECT_SOURCE_DIR}/ui/generated/*.c")
file(GLOB_RECURSE DCLOCK_SOURCES "${PROJECT_SOURCE_DIR}/ui/Edclock/*.c")
file(GLOB_RECURSE ANALOGDCLOCK_SOURCES "${PROJECT_SOURCE_DIR}/ui/Eanalogclock/*.c")
file(GLOB_RECURSE KEYBOARD_SOURCES "${PROJECT_SOURCE_DIR}/ui/Ekeyboard/*.c")
# 设备
file(GLOB_RECURSE DEV_SOURCES "${PROJECT_SOURCE_DIR}/dev/*.c")

# 定义驱动源文件列表，包含LVGL的帧缓冲显示驱动和输入设备驱动
set(DIRVER_SOURCES
    lv_drivers/display/fbdev.c  # Linux帧缓冲显示驱动（用于屏幕输出）
    lv_drivers/indev/evdev.c             # 输入设备驱动（用于处理触摸、按键等输入）
    )

# 创建名为my_gui_app的可执行文件，并指定其依赖的源文件
add_executable(lvgl_demo 
    main.c           # 程序入口主文件
    ${CUSTOM_SOURCES}# 包含custom目录下的所有用户自定义逻辑代码
    ${GENERATED_SOURCES}# 包含generated目录下的所有自动生成UI代码
    ${DIRVER_SOURCES}# 包含LVGL的显示和输入驱动代码
    ${DCLOCK_SOURCES}# gui guider dclock组件
    ${ANALOGDCLOCK_SOURCES}# gui guider analogclock组件
    ${KEYBOARD_SOURCES}# gui guider keyboard组件
    ${DEV_SOURCES} # 外设
)

# 为my_gui_app可执行文件链接所需的库
target_link_libraries(lvgl_demo
                        PRIVATE 
                        freetype           # 链接FreeType库
                        lvgl               # 链接LVGL库
                        lvgl::demos
                        lv_drivers         # 链接lvgl驱动库
                        #lvgl::thorvg       # 链接LVGL依赖的thorvg矢量图形库
                        m                  # 链接数学库（部分LVGL功能依赖）
                        dl                 # 链接动态加载库（用于运行时加载动态库）
                        pthread            # 链接线程库（LVGL的多线程功能依赖）
                        libyuv
                        multimedia
                     ) 
