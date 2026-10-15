# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# This directory is added automatically by the OCUDU build when it is placed under '<ocudu>/plugins/'.
set(OCUDU_PLUGIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: Find the headers and libraries of your PHY SDK and link them in lib/vendor/CMakeLists.txt. For example:
#
# find_path(VENDOR_SDK_INCLUDE_DIR NAMES vendor_l1_api.h HINTS $ENV{VENDOR_SDK_DIR} PATH_SUFFIXES include REQUIRED)
# find_library(VENDOR_SDK_LIBRARY NAMES vendor_l1 HINTS $ENV{VENDOR_SDK_DIR} PATH_SUFFIXES lib REQUIRED)
# include_directories(SYSTEM ${VENDOR_SDK_INCLUDE_DIR})

include_directories(${OCUDU_PLUGIN_SRC_DIR}/include)

# PHY-FAPI adaptor library.
add_subdirectory(lib/vendor)
# Split 6 plugin, hooked into the OCUDU split 6 applications.
add_subdirectory(apps/units/flexible_o_du/split_6/o_du_high/vendor)
