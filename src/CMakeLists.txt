link_directories(
        ${GTKMM_LIBRARY_DIRS}  )

include_directories(
        ${GTKMM_INCLUDE_DIRS}  )

add_executable(cppraw
        main.cpp)

target_link_libraries(cppraw
        ${GTKMM_LIBRARIES}  )