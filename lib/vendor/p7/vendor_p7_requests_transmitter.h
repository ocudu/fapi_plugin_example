// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ul_context_repository.h"
#include "ocudu/fapi/p7/p7_last_request_notifier.h"
#include "ocudu/fapi/p7/p7_requests_gateway.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace vendor {

namespace hal {
class hardware_abstraction_layer;
} // namespace hal

/// \brief Vendor P7 requests transmitter.
///
/// Translates the FAPI P7 requests of a slot into vendor messages and sends them to the PHY in a single transaction,
/// which starts with the first request of the slot and ends when the last request is notified.
class vendor_p7_requests_transmitter : public fapi::p7_requests_gateway, public fapi::p7_last_request_notifier
{
public:
  vendor_p7_requests_transmitter(hal::hardware_abstraction_layer& hal_,
                                 pusch_context_repository&        pusch_repo_,
                                 pucch_context_repository&        pucch_repo_,
                                 ocudulog::basic_logger&          logger_) :
    hal(hal_), pusch_repo(pusch_repo_), pucch_repo(pucch_repo_), logger(logger_)
  {
  }

  // See interface for documentation.
  void send_dl_tti_request(const fapi::dl_tti_request& msg) override;

  // See interface for documentation.
  void send_ul_tti_request(const fapi::ul_tti_request& msg) override;

  // See interface for documentation.
  void send_ul_dci_request(const fapi::ul_dci_request& msg) override;

  // See interface for documentation.
  void send_tx_data_request(const fapi::tx_data_request& msg) override;

  // See interface for documentation.
  void on_last_message(slot_point slot) override;

private:
  /// \brief Queues a message in the transaction of the current slot.
  ///
  /// \param[in] msg_id Message type identifier.
  /// \param[in] fill   Callable that writes the message in the given buffer and returns its size, or zero on failure.
  template <typename Func>
  void send_message(unsigned msg_id, Func&& fill);

  hal::hardware_abstraction_layer& hal;
  pusch_context_repository&        pusch_repo;
  pucch_context_repository&        pucch_repo;
  ocudulog::basic_logger&          logger;
  /// Set to true when the transaction of the current slot has been started.
  bool message_in_slot = false;
};

} // namespace vendor
} // namespace ocudu
