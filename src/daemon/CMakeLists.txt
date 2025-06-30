# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_executable(knighttimed)

kconfig_target_kcfg_file(knighttimed
    FILE kdarklightsettings.kcfg
    CLASS_NAME KDarkLightSettings
    GENERATE_MOC
    GENERATE_PROPERTIES
)

kconfig_target_kcfg_file(knighttimed
    FILE kdarklightstate.kcfg
    CLASS_NAME KDarkLightState
    GENERATE_MOC
    GENERATE_PROPERTIES
    MUTATORS
)

target_compile_definitions(knighttimed PRIVATE
    -DTRANSLATION_DOMAIN=\"knighttimed\"
)

target_sources(knighttimed PRIVATE
    kdarklightmanager.cpp
    kdarklightmanagerinterface.cpp
    kdarklightscheduler.cpp
    ksolardarklightscheduler.cpp
    ktimeddarklightscheduler.cpp
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
