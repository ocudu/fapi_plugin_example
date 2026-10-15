// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "p7/ul_context_repository.h"
#include "ocudu/fapi/p7/messages/uci_indication.h"

extern "C" {
struct vendor_pucch_uci_pdu;
struct vendor_pusch_uci_pdu;
}

namespace ocudu {
namespace vendor {

/// \brief Translates a vendor PUCCH UCI PDU into a FAPI UCI.indication.
///
/// \param[out] indication FAPI UCI.indication.
/// \param[in]  slot       Slot of the indication.
/// \param[in]  pdu        Vendor PUCCH UCI PDU.
/// \param[in]  context    PUCCH context stored when the UL_TTI.request was sent.
void translate_uci_indication(fapi::uci_indication&         indication,
                              slot_point                    slot,
                              const ::vendor_pucch_uci_pdu& pdu,
                              const pucch_context&          context);

/// \brief Translates a vendor PUSCH UCI PDU into a FAPI UCI.indication.
///
/// \param[out] indication FAPI UCI.indication.
/// \param[in]  slot       Slot of the indication.
/// \param[in]  pdu        Vendor PUSCH UCI PDU.
void translate_uci_indication(fapi::uci_indication& indication, slot_point slot, const ::vendor_pusch_uci_pdu& pdu);

} // namespace vendor
} // namespace ocudu
