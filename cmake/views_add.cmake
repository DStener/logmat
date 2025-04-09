find_program(DROGON_CTL_COMMAND drogon_ctl REQUIRED)

FILE(GLOB HTML_LIST ${CMAKE_SOURCE_DIR}/views/*.html)
foreach(htmlFile ${HTML_LIST})
  cmake_path(GET htmlFile STEM classname)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${classname}.h ${CMAKE_CURRENT_BINARY_DIR}/${classname}.cc
                     COMMAND ${DROGON_CTL_COMMAND} 
                             ARGS 
                             create 
                             view 
                             ${htmlFile}
                             -o
                             ${CMAKE_CURRENT_BINARY_DIR}
                     DEPENDS ${htmlFile}
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                     VERBATIM)
  set(VIEWSRC ${VIEWSRC} ${CMAKE_CURRENT_BINARY_DIR}/${classname}.cc)
endforeach()
