cmake_minimum_required(VERSION 3.24)
project(logmat CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB MODULES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${MODULES_SRC})
configure_file(config.json config.json COPYONLY)

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


add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/views/temp.html  ${CMAKE_SOURCE_DIR}/views/temp.csp)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})


target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
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
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC})

# Create symlink to HTML root dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/root $<TARGET_FILE_DIR:${PROJECT_NAME}>/root)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/migrations $<TARGET_FILE_DIR:${PROJECT_NAME}>/migrations)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/seeders $<TARGET_FILE_DIR:${PROJECT_NAME}>/seeders)

add_subdirectory(test)
