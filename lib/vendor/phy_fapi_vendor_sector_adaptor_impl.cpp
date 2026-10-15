// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "phy_fapi_vendor_sector_adaptor_impl.h"

using namespace ocudu;
using namespace vendor;

phy_fapi_sector_adaptor_impl::phy_fapi_sector_adaptor_impl(const vendor_adaptor_cell_config&                cell_cfg,
                                                           std::unique_ptr<hal::hardware_abstraction_layer> hal_,
                                                           ocudulog::basic_logger&                          logger) :
  hal(std::move(hal_)),
  p5_adaptor(*hal, logger),
  p7_adaptor(cell_cfg.scs_common, *hal, logger),
  msg_handler(p5_adaptor.get_message_receiver(), p7_adaptor.get_message_receiver(), logger)
{
  // Start receiving messages from the PHY.
  hal->get_message_dispatcher_operation_controller().start(msg_handler);
}

phy_fapi_sector_adaptor_impl::~phy_fapi_sector_adaptor_impl()
{
  hal->get_message_dispatcher_operation_controller().stop();
}
