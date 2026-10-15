// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace vendor {

namespace hal {
class hardware_abstraction_layer;
} // namespace hal

/// \brief Vendor P5 requests transmitter.
///
/// Translates the FAPI P5 requests into vendor messages and sends each of them to the PHY in its own transaction.
class vendor_p5_requests_transmitter : public fapi::p5_requests_gateway
{
public:
  vendor_p5_requests_transmitter(hal::hardware_abstraction_layer& hal_, ocudulog::basic_logger& logger_) :
    hal(hal_), logger(logger_)
  {
  }

  // See interface for documentation.
  void send_param_request(const fapi::param_request& msg) override;

  // See interface for documentation.
  void send_config_request(const fapi::config_request& msg) override;

  // See interface for documentation.
  void send_start_request(const fapi::start_request& msg) override;

  // See interface for documentation.
  void send_stop_request(const fapi::stop_request& msg) override;

  /// Sets the FAPI P5 responses notifier, used to answer the PARAM.request locally.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& notifier) { p5_notifier = &notifier; }

private:
  /// \brief Sends a single message to the PHY.
  ///
  /// \param[in] msg_id Message type identifier.
  /// \param[in] fill   Callable that writes the message in the given buffer and returns its size in bytes.
  template <typename Func>
  void send_message(unsigned msg_id, Func&& fill);

  hal::hardware_abstraction_layer& hal;
  ocudulog::basic_logger&          logger;
  fapi::p5_responses_notifier*     p5_notifier = nullptr;
};

} // namespace vendor
} // namespace ocudu
