// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "vendor_p5_requests_transmitter.h"
#include "vendor_p5_responses_receiver.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"

namespace ocudu {
namespace vendor {

/// Vendor PHY-FAPI P5 sector adaptor implementation.
class phy_fapi_p5_sector_adaptor_impl : public fapi_adaptor::phy_fapi_p5_sector_adaptor
{
public:
  phy_fapi_p5_sector_adaptor_impl(hal::hardware_abstraction_layer& hal, ocudulog::basic_logger& logger) :
    msg_receiver(logger), msg_transmitter(hal, logger)
  {
  }

  // See interface for documentation.
  fapi::p5_requests_gateway& get_p5_requests_gateway() override { return msg_transmitter; }

  // See interface for documentation.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& notifier) override;

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& notifier) override;

  /// Returns the message receiver of this sector adaptor.
  vendor_p5_responses_receiver& get_message_receiver() { return msg_receiver; }

private:
  vendor_p5_responses_receiver   msg_receiver;
  vendor_p5_requests_transmitter msg_transmitter;
};

} // namespace vendor
} // namespace ocudu
