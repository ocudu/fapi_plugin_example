// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "hal/hal.h"
#include "p7/ul_context_repository.h"
#include "ocudu/fapi/p7/messages/rx_data_indication.h"

extern "C" {
struct vendor_rx_data_pdu;
}

namespace ocudu {
namespace vendor {

/// \brief Translates a vendor RX_Data PDU into a FAPI RX_Data.indication.
///
/// \param[out] indication FAPI RX_Data.indication.
/// \param[in]  slot       Slot of the indication.
/// \param[in]  pdu        Vendor RX_Data PDU.
/// \param[in]  context    PUSCH context stored when the UL_TTI.request was sent.
/// \param[in]  buffer     Received transport block, as read from the HAL. It must outlive the indication.
void translate_rx_data_indication(fapi::rx_data_indication&                           indication,
                                  slot_point                                          slot,
                                  const ::vendor_rx_data_pdu&                         pdu,
                                  const pusch_context&                                context,
                                  const hal::data_memory_reader::unique_memory_block& buffer);

} // namespace vendor
} // namespace ocudu
