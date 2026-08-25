cmake_minimum_required(VERSION 3.16)

# Project version: vMAJOR.MINOR.BUILD from version.txt (build number is
# auto-bumped by spiffs_src/build_web.py, which also stamps the web assets'
# webui-version.json — same scheme as hub_core). PROJECT_VER lands in
# esp_app_desc_t.version.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/version.txt" PROJECT_VER LIMIT_COUNT 1)
string(STRIP "${PROJECT_VER}" PROJECT_VER)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/version.txt")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(rvc_tools)

spiffs_create_partition_image(storage spiffs FLASH_IN_PROJECT)
