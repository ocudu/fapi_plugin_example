// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ul_context_repository.h"
#include "vendor_p7_indications_receiver.h"
#include "vendor_p7_requests_transmitter.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_adaptor.h"

namespace ocudu {
namespace vendor {

/// Vendor PHY-FAPI P7 sector adaptor implementation.
class phy_fapi_p7_sector_adaptor_impl : public fapi_adaptor::phy_fapi_p7_sector_adaptor
{
public:
  phy_fapi_p7_sector_adaptor_impl(subcarrier_spacing               scs,
                                  hal::hardware_abstraction_layer& hal,
                                  ocudulog::basic_logger&          logger);

  // See interface for documentation.
  fapi::p7_requests_gateway& get_p7_requests_gateway() override { return msg_transmitter; }

  // See interface for documentation.
  fapi::p7_last_request_notifier& get_p7_last_request_notifier() override { return msg_transmitter; }

  // See interface for documentation.
  void set_p7_slot_indication_notifier(fapi::p7_slot_indication_notifier& notifier) override
  {
    msg_receiver.set_p7_slot_indication_notifier(notifier);
  }

  // See interface for documentation.
  void set_p7_indications_notifier(fapi::p7_indications_notifier& notifier) override
  {
    msg_receiver.set_p7_indications_notifier(notifier);
  }

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& notifier) override
  {
    msg_receiver.set_error_indication_notifier(notifier);
  }

  /// Returns the message receiver of this sector adaptor.
  vendor_p7_indications_receiver& get_message_receiver() { return msg_receiver; }

private:
  pusch_context_repository       pusch_repo;
  pucch_context_repository       pucch_repo;
  vendor_p7_indications_receiver msg_receiver;
  vendor_p7_requests_transmitter msg_transmitter;
};

} // namespace vendor
} // namespace ocudu
