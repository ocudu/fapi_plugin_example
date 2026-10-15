// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace vendor {

/// \brief Vendor P5 responses receiver.
///
/// Deserializes the P5 responses received from the PHY and forwards them through the corresponding FAPI notifiers.
class vendor_p5_responses_receiver
{
public:
  explicit vendor_p5_responses_receiver(ocudulog::basic_logger& logger_) : logger(logger_) {}

  /// Handles a P5 message received from the PHY.
  void handle_new_message(span<const uint8_t> msg);

  /// Sets the FAPI P5 responses notifier.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& notifier) { p5_notifier = &notifier; }

  /// Sets the FAPI error indication notifier.
  void set_error_indication_notifier(fapi::error_indication_notifier& notifier) { error_notifier = &notifier; }

private:
  ocudulog::basic_logger&          logger;
  fapi::p5_responses_notifier*     p5_notifier    = nullptr;
  fapi::error_indication_notifier* error_notifier = nullptr;
};

} // namespace vendor
} // namespace ocudu
