// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_plugin_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "vendor_plugin_config.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;
using namespace vendor;

static void configure_cli11_log_args(CLI::App& app, vendor_plugin_config& params)
{
  app_helpers::add_log_option(app, params.log_level, "--vendor_level", "Default log level for the vendor FAPI plugin");
}

static void configure_cli11_transport_args(CLI::App& app, vendor_transport_config& transport_cfg)
{
  add_option(app,
             "--device_name",
             transport_cfg.device_name,
             "Identifier of the device or channel used to communicate with the PHY");
}

void vendor::configure_cli11_with_vendor_plugin_config_schema(CLI::App& app, vendor_plugin_config& config)
{
  // Logging section. Adds the 'log.vendor_level' option.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, config);

  // Vendor section. Adds the 'vendor' section to the configuration file.
  // TODO: Add here any other parameter required by your plugin.
  CLI::App* vendor_subcmd = add_subcommand(app, "vendor", "Vendor FAPI plugin configuration")->configurable();
  configure_cli11_transport_args(*vendor_subcmd, config.transport_cfg);
}
