add_executable(monitor)

target_sources(monitor PRIVATE
    main.cpp
)

target_link_libraries(monitor PRIVATE
    KNightTime
)
