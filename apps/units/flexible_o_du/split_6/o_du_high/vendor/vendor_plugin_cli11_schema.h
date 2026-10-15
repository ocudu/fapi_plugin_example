// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {
namespace vendor {

struct vendor_plugin_config;

/// Configures the given CLI11 application with the vendor plugin configuration schema.
void configure_cli11_with_vendor_plugin_config_schema(CLI::App& app, vendor_plugin_config& config);

} // namespace vendor
} // namespace ocudu
