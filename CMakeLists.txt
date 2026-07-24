cmake_minimum_required(VERSION 3.16)
project(panelplayer VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBXFCE4PANEL REQUIRED libxfce4panel-2.0>=4.14.0)
pkg_check_modules(LIBXFCE4UI REQUIRED libxfce4ui-2>=4.14.0)
pkg_check_modules(PLAYERCTL REQUIRED playerctl>=2.0.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0>=3.22.0)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.50.0)

# Plugin sources
set(PLUGIN_SOURCES
    src/panelplayer-plugin.c
    src/panelplayer-player.c
)

# Create shared library
add_library(panelplayer MODULE ${PLUGIN_SOURCES})

# Include directories
target_include_directories(panelplayer PRIVATE
    ${LIBXFCE4PANEL_INCLUDE_DIRS}
    ${LIBXFCE4UI_INCLUDE_DIRS}
    ${PLAYERCTL_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(panelplayer PRIVATE
    ${LIBXFCE4PANEL_LIBRARIES}
    ${LIBXFCE4UI_LIBRARIES}
    ${PLAYERCTL_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${GLIB_LIBRARIES}
)

# Compiler flags
target_compile_options(panelplayer PRIVATE
    ${LIBXFCE4PANEL_CFLAGS_OTHER}
    ${LIBXFCE4UI_CFLAGS_OTHER}
    ${PLAYERCTL_CFLAGS_OTHER}
    ${GTK3_CFLAGS_OTHER}
    ${GLIB_CFLAGS_OTHER}
)

# Keep lib prefix - xfce4 panel expects lib<module>.so
set_target_properties(panelplayer PROPERTIES PREFIX "lib")

# Install plugin library
install(TARGETS panelplayer
    LIBRARY DESTINATION lib/xfce4/panel/plugins)

# Install desktop file
configure_file(
    ${CMAKE_SOURCE_DIR}/panel-plugin/panelplayer.desktop.in
    ${CMAKE_BINARY_DIR}/panelplayer.desktop
    @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/panelplayer.desktop
    DESTINATION share/xfce4/panel/plugins)

# Install icon
install(FILES icons/playerctl.svg
    DESTINATION share/icons/hicolor/scalable/apps)
