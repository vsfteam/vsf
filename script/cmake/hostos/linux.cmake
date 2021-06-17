# pthread
target_link_libraries(${VSF_LIB_NAME} INTERFACE pthread)
# real time library, timer_create etc...
target_link_libraries(${VSF_LIB_NAME} INTERFACE rt)
