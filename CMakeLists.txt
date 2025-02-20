cmake_minimum_required(VERSION 3.5)

project(mould_system VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Multimedia MultimediaWidgets  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        label.cpp
        label.h
        label.ui
        ConfigManager.h
        ConfigManager.cpp
        global.h
        global.cpp
        collect.h
        collect.cpp
        collect.ui
        widgets/customListwidget.h
        widgets/customListwidget.cpp
        widgets/Abstract.h
        widgets/Abstract.cpp
        widgets/scalablelabel.h
        widgets/customvideo.h widgets/customvideo.cpp
        widgets/scalablelabel.cpp
        widgets/extendedcombobox.h widgets/extendedcombobox.cpp
        manager/annotation_manager.h
        manager/annotation_manager.cpp
        resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mould_system
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(mould_system SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(mould_system
            WIN32
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 包含RapidJSON头文件目录
#include_directories(${CMAKE_SOURCE_DIR}/include)


target_link_libraries(mould_system PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

set_target_properties(mould_system PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mould_system)
endif()
