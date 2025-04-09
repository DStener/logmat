# Create symlink to HTML root dir
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/root
                    COMMAND ${CMAKE_COMMAND} -E create_symlink
                    ${CMAKE_SOURCE_DIR}/root ${CMAKE_CURRENT_BINARY_DIR}/root)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/migrations
                    COMMAND ${CMAKE_COMMAND} -E create_symlink
                    ${CMAKE_SOURCE_DIR}/migrations ${CMAKE_CURRENT_BINARY_DIR}/migrations)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/seeders
                    COMMAND ${CMAKE_COMMAND} -E create_symlink
                    ${CMAKE_SOURCE_DIR}/seeders ${CMAKE_CURRENT_BINARY_DIR}/seeders)


set(SYMLINK_TARGET ${SYMLINK_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/root)
set(SYMLINK_TARGET ${SYMLINK_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/migrations)
set(SYMLINK_TARGET ${SYMLINK_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/seeders)

