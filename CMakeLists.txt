cmake_minimum_required(VERSION 3.5)
project(logmat CXX)

include(CheckIncludeFileCXX)


check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)



set(CMAKE_CXX_STANDARD 23)


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB MODULES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cpp)
file(GLOB MODELS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/models/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${MODULES_SRC} ${MODELS_SRC})


find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon OpenSSL::SSL)

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})


target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${OPENSSL_INCLUDE_DIR}
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

add_subdirectory(test)
