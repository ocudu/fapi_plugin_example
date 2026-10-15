# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set(SOURCES
  vendor_plugin_cli11_schema.cpp
  vendor_plugin_impl.cpp
  vendor_split6_du_application_unit_factory.cpp)

add_library(ocudu_split6_plugin_vendor OBJECT ${SOURCES})
target_include_directories(ocudu_split6_plugin_vendor PRIVATE ${CMAKE_SOURCE_DIR} ${OCUDU_PLUGIN_SRC_DIR}/include)

set(OCUDU_TARGET_LIB ocudu_split6_plugin)
# Link to the Split 6 plugin.
# Note: the OCUDU library is linked against the object library to add its requirements, and against its objects to
# add the objects themselves. They provide the ocudu::create_split6_plugin() symbol used by the split 6 applications.
target_link_libraries(${OCUDU_TARGET_LIB} PRIVATE ocudu_split6_plugin_vendor $<TARGET_OBJECTS:ocudu_split6_plugin_vendor>)
# Compile the OCUDU target library with this definition so that its dummy ocudu::create_split6_plugin() is left out
# in favour of the one provided by this plugin.
target_compile_definitions(${OCUDU_TARGET_LIB} PRIVATE -DOCUDU_HAS_SPLIT6_PHY_PLUGIN)

target_link_libraries(ocudu_split6_plugin_vendor PRIVATE ocudu_app_helpers fapi_vendor_adaptor)
