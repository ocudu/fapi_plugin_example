// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "rach_indication_translator.h"
#include "ocudu/fapi/p7/builders/rach_indication_builder.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

void vendor::translate_rach_indication(fapi::rach_indication&        indication,
                                       slot_point                    slot,
                                       const ::vendor_rach_preamble& preamble)
{
  fapi::rach_indication_builder builder(indication);
  builder.set_slot(slot);

  // TODO: Report the RSSI, preamble power and SNR measurements. Power measurements are given as fapi::fapi_power_unit,
  // which takes the measured value in normalized dB and the dBFS/dBm conversion factors of the radio chain.
  fapi::rach_indication_pdu_builder pdu_builder =
      builder.set_pdu(preamble.symbol_index, preamble.slot_index, preamble.fd_ra_index, std::nullopt, std::nullopt);
  pdu_builder.add_preamble(preamble.preamble_index,
                           phy_time_unit::from_timing_advance(preamble.timing_advance, slot.scs()),
                           std::nullopt,
                           std::nullopt);
}
