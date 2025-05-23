add_executable(knighttimed)

kconfig_target_kcfg_file(knighttimed
    FILE knighttimesettings.kcfg
    CLASS_NAME KNightTimeSettings
    GENERATE_MOC
)

kconfig_target_kcfg_file(knighttimed
    FILE knighttimestate.kcfg
    CLASS_NAME KNightTimeState
    MUTATORS
)

target_sources(knighttimed PRIVATE
    knighttimemanager.cpp
    knighttimemanagerinterface.cpp
    knighttimescheduler.cpp
    ksolarnighttimescheduler.cpp
    ktimednighttimescheduler.cpp
    main.cpp
)

target_link_libraries(knighttimed PRIVATE
    KNightTime

    Qt6::DBus
    Qt6::Positioning

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::DBusAddons
)

install(FILES org.kde.NightTime.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(TARGETS knighttimed DESTINATION ${KDE_INSTALL_LIBEXECDIR})

ecm_generate_dbus_service_file(
    NAME org.kde.NightTime
    EXECUTABLE "${KDE_INSTALL_FULL_LIBEXECDIR}/knighttimed"
    SYSTEMD_SERVICE plasma-knighttimed.service
    DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
)

ecm_install_configured_files(
   INPUT plasma-knighttimed.service.in
   DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR}
)

ecm_install_configured_files(
    INPUT org.kde.knighttimed.desktop.in
    DESTINATION ${KDE_INSTALL_APPDIR}
)
