# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

include(ECMMarkAsTest)

find_package(Qt6Test CONFIG REQUIRED)

add_executable(transition-test transition_test.cpp)
add_test(NAME transition-test COMMAND transition-test)
ecm_mark_as_test(transition-test)
target_link_libraries(transition-test PRIVATE KNightTime Qt6::Test)

add_executable(cycle-test cycle_test.cpp)
add_test(NAME cycle-test COMMAND cycle-test)
ecm_mark_as_test(cycle-test)
target_link_libraries(cycle-test PRIVATE KNightTime Qt6::Test)

add_executable(schedule-test schedule_test.cpp)
add_test(NAME schedule-test COMMAND schedule-test)
ecm_mark_as_test(schedule-test)
target_link_libraries(schedule-test PRIVATE KNightTime Qt6::Test)
