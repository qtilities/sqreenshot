cmake_minimum_required(VERSION 3.6)
project(Sqreenshot VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets X11Extras LinguistTools REQUIRED)
find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)

# Xfixes is needed to capture the mouse cursor image
pkg_check_modules(XFIXES REQUIRED xfixes)
pkg_search_module(GLIB REQUIRED glib-2.0)

set(TS_FILES resources/translations/sqreenshot_it_IT.ts)

set(SRC_FILES
    src/dialogmain.cpp
    src/dialogmain.hpp
    src/dialogscreenshot.hpp
    src/dialogscreenshot.cpp
    src/dialogselectionarea.hpp
    src/dialogselectionarea.cpp
    src/settings.cpp
    src/settings.hpp
    src/sqreenshot.cpp
    src/sqreenshot.hpp
    src/defs.hpp
    resources/resources.qrc
    ${TS_FILES}
)
set(UI_FILES
    src/dialogmain.ui
    src/dialogscreenshot.ui
)
source_group("UI Files" FILES ${UI_FILES})

add_executable(${CMAKE_PROJECT_NAME}
    ${SRC_FILES}
    ${UI_FILES}
)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${XFIXES_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIRS}
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::X11Extras
    ${X11_LIBRARIES}
    ${XFIXES_LDFLAGS}
)
target_link_libraries(${CMAKE_PROJECT_NAME} INTERFACE
    ${GLIB_LDFLAGS}
)
if (UNIX AND NOT APPLE)
    string(TOLOWER ${CMAKE_PROJECT_NAME} APP_NAME)
    set(APPSTREAM_BASENAME "org.azdrums.${APP_NAME}")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${APP_NAME}"
    )
    include (GNUInstallDirs)
    install (TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (FILES resources/icons/${APP_NAME}.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install (FILES resources/linux/${APPSTREAM_BASENAME}.appdata.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install (FILES resources/linux/${APPSTREAM_BASENAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
#   install (FILES ${QM_FILES}
#       DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}/translations)
endif()
