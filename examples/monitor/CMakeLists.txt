# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_executable(monitor)

target_sources(monitor PRIVATE
    main.cpp
)

target_link_libraries(monitor PRIVATE
    KNightTime
)
