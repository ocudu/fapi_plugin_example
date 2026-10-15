// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "p7/ul_context_repository.h"
#include "ocudu/fapi/p7/messages/crc_indication.h"

extern "C" {
struct vendor_crc_pdu;
}

namespace ocudu {
namespace vendor {

/// \brief Translates a vendor CRC PDU into a FAPI CRC.indication.
///
/// \param[out] indication FAPI CRC.indication.
/// \param[in]  slot       Slot of the indication.
/// \param[in]  pdu        Vendor CRC PDU.
/// \param[in]  context    PUSCH context stored when the UL_TTI.request was sent.
void translate_crc_indication(fapi::crc_indication&   indication,
                              slot_point              slot,
                              const ::vendor_crc_pdu& pdu,
                              const pusch_context&    context);

} // namespace vendor
} // namespace ocudu
