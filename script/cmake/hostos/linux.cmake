# pthread
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC pthread)
# real time library, timer_create etc...
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC rt)
