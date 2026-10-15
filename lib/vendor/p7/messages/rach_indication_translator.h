// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/rach_indication.h"

extern "C" {
struct vendor_rach_preamble;
}

namespace ocudu {
namespace vendor {

/// \brief Translates a vendor PRACH preamble into a FAPI RACH.indication.
///
/// \param[out] indication FAPI RACH.indication.
/// \param[in]  slot       Slot of the indication.
/// \param[in]  preamble   Vendor PRACH preamble.
void translate_rach_indication(fapi::rach_indication&        indication,
                               slot_point                    slot,
                               const ::vendor_rach_preamble& preamble);

} // namespace vendor
} // namespace ocudu
