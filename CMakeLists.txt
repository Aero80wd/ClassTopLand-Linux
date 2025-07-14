cmake_minimum_required(VERSION 3.0)
project(ClassTopLand_Linux)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)         # 自动调用 moc
set(CMAKE_AUTORCC ON)         # 自动调用 rcc
set(CMAKE_AUTOUIC ON)         # 自动调用 uic
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置 Qt 的查找路径（可选）
set(CMAKE_PREFIX_PATH "/home/jiahang/Qt/6.7.3/gcc_64/lib/cmake/")
set(CMAKE_LINK_LIBRARY_FLAG "-static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
# 查找 Qt6 组件
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        WebEngineWidgets
        Test
        REQUIRED
)

# 收集 src 及其子目录中的源文件
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.ui")
set(RESOURCE_FILES res.qrc)
include_directories(${Qt6Gui_PRIVATE_INCLUDE_DIRS})
# 添加主函数入口
set(MAIN_SOURCE main.cpp
        src/Utils/Utils.cpp
        src/Utils/Utils.h)

# 创建可执行文件（会自动处理 .ui/.qrc）
qt_add_executable(${PROJECT_NAME}
        ${MAIN_SOURCE}
        ${SOURCE_FILES}
        ${RESOURCE_FILES}
)
if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_include_directories(ClassTopLand_Linux PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(ClassTopLand_Linux PRIVATE ${X11_LIBRARIES})
endif()
# 链接需要的 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::WebEngineWidgets
        Qt::Test
)


# 如果你需要显式包含某些目录（比如头文件放在 include/ 中），可以加上：
# include_directories(include)