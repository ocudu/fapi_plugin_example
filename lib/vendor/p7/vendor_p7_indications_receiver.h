// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ul_context_repository.h"
#include "ocudu/adt/span.h"
#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p7/p7_indications_notifier.h"
#include "ocudu/fapi/p7/p7_slot_indication_notifier.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace vendor {

namespace hal {
class data_memory_reader;
} // namespace hal

/// \brief Vendor P7 indications receiver.
///
/// Deserializes the P7 indications received from the PHY and forwards them through the corresponding FAPI notifiers.
class vendor_p7_indications_receiver
{
public:
  vendor_p7_indications_receiver(subcarrier_spacing        scs_,
                                 hal::data_memory_reader&  data_mem_reader_,
                                 pusch_context_repository& pusch_repo_,
                                 pucch_context_repository& pucch_repo_,
                                 ocudulog::basic_logger&   logger_) :
    scs(scs_), data_mem_reader(data_mem_reader_), pusch_repo(pusch_repo_), pucch_repo(pucch_repo_), logger(logger_)
  {
  }

  /// Handles a P7 message received from the PHY.
  void handle_new_message(span<const uint8_t> msg);

  /// Sets the P7 slot indication notifier.
  void set_p7_slot_indication_notifier(fapi::p7_slot_indication_notifier& notifier) { slot_notifier = &notifier; }

  /// Sets the P7 indications notifier.
  void set_p7_indications_notifier(fapi::p7_indications_notifier& notifier) { p7_notifier = &notifier; }

  /// Sets the error indication notifier.
  void set_error_indication_notifier(fapi::error_indication_notifier& notifier) { error_notifier = &notifier; }

private:
  const subcarrier_spacing           scs;
  hal::data_memory_reader&           data_mem_reader;
  pusch_context_repository&          pusch_repo;
  pucch_context_repository&          pucch_repo;
  ocudulog::basic_logger&            logger;
  fapi::p7_slot_indication_notifier* slot_notifier  = nullptr;
  fapi::p7_indications_notifier*     p7_notifier    = nullptr;
  fapi::error_indication_notifier*   error_notifier = nullptr;
};

} // namespace vendor
} // namespace ocudu
