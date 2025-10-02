# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "6.4.91")
project(KNightTime VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_DEP_VERSION "6.4.90")
set(QT_MIN_VERSION "6.9.0")
set(KF6_MIN_VERSION "6.18.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)
include(FeatureSummary)
include(GenerateExportHeader)

include(ECMConfiguredInstall)
include(ECMGenerateDBusServiceFile)
include(ECMGenerateHeaders)
include(ECMGenerateQDoc)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    DBus
    Positioning
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
    Holidays
    I18n
)

ecm_set_disabled_deprecation_versions(QT 6.9.0
    KF 6.16.0
)

check_cxx_compiler_flag(-Wno-unused-parameter COMPILER_UNUSED_PARAMETER_SUPPORTED)
if (COMPILER_UNUSED_PARAMETER_SUPPORTED)
    add_compile_options(-Wno-unused-parameter)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(examples)
endif()

add_subdirectory(src)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KNightTime")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/KNightTimeConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KNightTimeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KNightTimeConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KNightTimeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KNightTimeConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}" COMPONENT Devel
)

install(EXPORT KNightTimeTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KNightTimeTargets.cmake)

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
