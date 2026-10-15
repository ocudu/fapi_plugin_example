// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "apps/units/flexible_o_du/split_6/o_du_high/split6_plugin.h"
#include "vendor_plugin_config.h"

namespace ocudu {

struct o_du_unit_dependencies;
struct worker_manager_config;

namespace fapi_adaptor {
struct split6_o_du_low_fapi_adaptor_configuration;
}

namespace vendor {

/// Vendor plugin implementation for DU split 6.
class vendor_plugin_impl : public split6_plugin
{
public:
  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation() const override;

  // See interface for documentation.
  bool is_ran_config_supported(const odu::du_high_ran_config& configuration) const override;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
  create_fapi_adaptor(const fapi_adaptor::split6_o_du_low_fapi_adaptor_configuration& fapi_cfg,
                      const o_du_unit_dependencies&                                   dependencies) override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

private:
  vendor_plugin_config plugin_cfg;
};

} // namespace vendor
} // namespace ocudu
