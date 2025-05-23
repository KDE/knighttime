add_library(KNightTime)

generate_export_header(KNightTime EXPORT_FILE_NAME knighttime_export.h)

set_target_properties(KNightTime PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME KNightTime
)

target_include_directories(KNightTime
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KNightTime/>"
)

target_sources(KNightTime PRIVATE
    knighttimeschedule.cpp
    knighttimescheduleprovider.cpp
    knighttimeschedulesubscription.cpp
)

target_link_libraries(KNightTime
    PUBLIC
        Qt6::Core
    PRIVATE
        Qt6::DBus
        KF6::Holidays
)

install(TARGETS KNightTime EXPORT KNightTimeTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_qdoc(KNightTime knighttime.qdocconf)

ecm_generate_headers(KNightTime_HEADERS
    HEADER_NAMES
        KNightTimeSchedule
        KNightTimeScheduleProvider
    REQUIRED_HEADERS KCoreAddons_HEADERS
)

install(
    FILES
        ${KNightTime_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/knighttime_export.h
        knighttimeschedule.h
        knighttimescheduleprovider.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KNightTime COMPONENT Devel
)

add_subdirectory(daemon)
