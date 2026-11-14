cmake_minimum_required(VERSION 3.16)

project(Clipboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Quick Sql Gui)
find_package(LayerShellQt REQUIRED)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appClipboard
    src/main.cpp
)

qt_add_executable(ClipboardDaemon
    src/daemon/main.cpp
    src/daemon/ClipboardDaemon.h
    src/daemon/ClipboardDaemon.cpp

    src/database/ClipboardDB.h
    src/database/ClipboardDB.cpp

    src/model/ClipboardItem.h
)

target_link_libraries(ClipboardDaemon
    PRIVATE Qt6::Core Qt6::Sql Qt6::Gui
)

qt_add_qml_module(appClipboard
    URI Clipboard
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    SOURCES
        src/Clipboard.h
        src/Clipboard.cpp
        src/model/ClipboardItem.h
        src/model/ClipboardModel.h
        src/model/ClipboardModel.cpp
        src/database/ClipboardDB.h
        src/database/ClipboardDB.cpp
)

set_target_properties(appClipboard PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appClipboard 
    PRIVATE 
        Qt6::Quick Qt6::Sql
        LayerShellQt::Interface
)

include(GNUInstallDirs)
install(TARGETS appClipboard ClipboardDaemon
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
