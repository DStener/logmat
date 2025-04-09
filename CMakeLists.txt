cmake_minimum_required(VERSION 3.24)
project(logmat CXX)

# Include file/module for convert .html view (like .csp) to c++ code
include(${CMAKE_SOURCE_DIR}/cmake/views_add.cmake)
# Include file/module for create symlink for mains directories
include(${CMAKE_SOURCE_DIR}/cmake/main_symlink_create.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB MODULES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${MODULES_SRC})
configure_file(config.json config.json COPYONLY)
configure_file(config.h.in config.h @ONLY)

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)
find_package(JPEG REQUIRED)

find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY qrencode)
 
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon OpenSSL::SSL
                      ${Boost_LIBRARY_DIRS} ${QRENCODE_LIBRARY} JPEG::JPEG)


aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_BINARY_DIR}
                                   ${OPENSSL_INCLUDE_DIR}
                                   ${Boost_INCLUDE_DIR}
                                   ${JPEG_INCLUDE_DIR}
                                   ${QRENCODE_INCLUDE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/modules
                                   ${CMAKE_CURRENT_SOURCE_DIR}/controllers
                                   ${CMAKE_CURRENT_SOURCE_DIR}/plugins)

target_sources(${PROJECT_NAME}
               PRIVATE
               ${SYMLINK_TARGET}
               ${VIEWSRC}
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC})

add_subdirectory(test)
