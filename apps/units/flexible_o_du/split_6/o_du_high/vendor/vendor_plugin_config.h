// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <string>

namespace ocudu {
namespace vendor {

/// \brief Transport configuration options.
///
/// Parameters required to connect to the underlying PHY (e.g. a shared memory device name, a socket address, a PCIe
/// device identifier, etc).
// TODO: Replace/extend with the parameters needed by your PHY transport.
struct vendor_transport_config {
  /// Identifier of the device or channel used to communicate with the PHY.
  std::string device_name;
};

/// Vendor plugin configuration.
struct vendor_plugin_config {
  /// Plugin log level.
  ocudulog::basic_levels log_level = ocudulog::basic_levels::warning;
  /// Transport configuration.
  vendor_transport_config transport_cfg;
};

} // namespace vendor
} // namespace ocudu
