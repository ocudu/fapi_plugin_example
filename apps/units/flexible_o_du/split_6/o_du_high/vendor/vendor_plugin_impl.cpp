// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_plugin_impl.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/o_du_unit.h"
#include "apps/units/flexible_o_du/split_6/o_du_high/split6_o_du_low_fapi_adaptor_configuration.h"
#include "vendor_plugin_cli11_schema.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/vendor/phy_fapi_vendor_adaptor_factory.h"

using namespace ocudu;
using namespace vendor;

/// Name of the logger used by the plugin.
static constexpr std::string_view VENDOR_LOGGER_NAME = "VENDOR";

void vendor_plugin_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_vendor_plugin_config_schema(app, plugin_cfg);
}

bool vendor_plugin_impl::on_configuration_validation() const
{
  // TODO: Validate here the plugin specific configuration.
  if (plugin_cfg.transport_cfg.device_name.empty()) {
    fmt::println("The vendor.device_name configuration parameter must be provided.");
    return false;
  }

  return true;
}

bool vendor_plugin_impl::is_ran_config_supported(const odu::du_high_ran_config& configuration) const
{
  // TODO: Reject here the RAN configurations that are not supported by the underlying PHY. Some examples of
  // restrictions are the number of cells, the PDSCH/PUSCH mapping types, DMRS positions, CSI-RS and PDSCH multiplexing,
  // etc.
  if (configuration.cells.size() != 1) {
    fmt::println("The vendor PHY-FAPI plugin does not support multiple cells.");
    return false;
  }

  return true;
}

void vendor_plugin_impl::on_loggers_registration()
{
  auto& logger = ocudulog::fetch_basic_logger(std::string(VENDOR_LOGGER_NAME));
  logger.set_level(plugin_cfg.log_level);
}

std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
vendor_plugin_impl::create_fapi_adaptor(const fapi_adaptor::split6_o_du_low_fapi_adaptor_configuration& fapi_cfg,
                                        const o_du_unit_dependencies&                                   dependencies)
{
  auto& logger = ocudulog::fetch_basic_logger(std::string(VENDOR_LOGGER_NAME));

  // TODO: Initialize here any process-wide resource required by the PHY transport before creating the adaptor (e.g. a
  // DPDK EAL, a connection to the PHY process, etc).

  std::vector<vendor_adaptor_cell_config> cells_cfg;
  for (const auto& cell : fapi_cfg.cells) {
    cells_cfg.push_back({.scs_common = cell.scs_common});
  }

  return create_phy_fapi_vendor_adaptor(
      cells_cfg, plugin_cfg.transport_cfg.device_name, logger, *dependencies.workers->split6_exec);
}

void vendor_plugin_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // Request the split 6 executor, used by the HAL to poll the messages coming from the PHY.
  config.is_split6_enabled = true;
}
