// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <memory>
#include <string>

namespace ocudu {

class task_executor;

namespace vendor {

/// Per-cell configuration of the vendor PHY-FAPI adaptor.
// TODO: Add here any other cell parameter needed before the FAPI CONFIG.request is received.
struct vendor_adaptor_cell_config {
  /// Common subcarrier spacing.
  subcarrier_spacing scs_common;
};

/// \brief Creates the vendor PHY-FAPI adaptor.
///
/// \param[in] cells_cfg   Per-cell configuration, one sector adaptor is created for each cell.
/// \param[in] device_name Identifier of the device or channel used to communicate with the PHY.
/// \param[in] logger      Plugin logger.
/// \param[in] executor    Executor used to poll the messages coming from the PHY.
std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
create_phy_fapi_vendor_adaptor(span<const vendor_adaptor_cell_config> cells_cfg,
                               const std::string&                     device_name,
                               ocudulog::basic_logger&                logger,
                               task_executor&                         executor);

} // namespace vendor
} // namespace ocudu
