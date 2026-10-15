// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "hal/hal.h"
#include "p5/phy_fapi_p5_vendor_sector_adaptor_impl.h"
#include "p7/phy_fapi_p7_vendor_sector_adaptor_impl.h"
#include "vendor_rx_message_handler.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "ocudu/vendor/phy_fapi_vendor_adaptor_factory.h"

namespace ocudu {
namespace vendor {

/// \brief Vendor PHY-FAPI sector adaptor implementation.
///
/// Owns the P5 and P7 adaptors of a cell and starts receiving messages from the PHY on construction.
class phy_fapi_sector_adaptor_impl : public fapi_adaptor::phy_fapi_sector_adaptor
{
public:
  phy_fapi_sector_adaptor_impl(const vendor_adaptor_cell_config&                cell_cfg,
                               std::unique_ptr<hal::hardware_abstraction_layer> hal_,
                               ocudulog::basic_logger&                          logger);

  ~phy_fapi_sector_adaptor_impl() override;

  // See interface for documentation.
  void start() override {}

  // See interface for documentation.
  void stop() override {}

  // See interface for documentation.
  fapi_adaptor::phy_fapi_p5_sector_adaptor& get_p5_sector_adaptor() override { return p5_adaptor; }

  // See interface for documentation.
  fapi_adaptor::phy_fapi_p7_sector_adaptor& get_p7_sector_adaptor() override { return p7_adaptor; }

private:
  std::unique_ptr<hal::hardware_abstraction_layer> hal;
  phy_fapi_p5_sector_adaptor_impl                  p5_adaptor;
  phy_fapi_p7_sector_adaptor_impl                  p7_adaptor;
  vendor_rx_message_handler                        msg_handler;
};

} // namespace vendor
} // namespace ocudu
