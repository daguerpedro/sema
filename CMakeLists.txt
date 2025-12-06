cmake_minimum_required(VERSION 3.19)
project(Sema LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts Mqtt Sql)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Sema
    WIN32 MACOSX_BUNDLE
    code/main.cpp
    code/data/database.h
    code/data/database.cpp
    code/winmanager.h
    code/winmanager.cpp
    code/settings/settingswin.h
    code/settings/settingswin.cpp
    code/dashboard/dashboardwin.h
    code/dashboard/dashboardwin.cpp
    resources.qrc
    code/widgets/customchart.h
    code/widgets/customlabel.h
    code/widgets/custombutton.h
    code/networking/netmanager.h code/networking/netmanager.cpp


)

target_link_libraries(Sema
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Charts
        Qt::Mqtt
        Qt::Sql
)
target_link_libraries(Sema PRIVATE Qt6::Widgets)
target_link_libraries(Sema PRIVATE Qt6::Core)
target_link_libraries(Sema PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS Sema
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Sema
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
