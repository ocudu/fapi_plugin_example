// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "crc_indication_translator.h"
#include "ocudu/fapi/p7/builders/crc_indication_builder.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

void vendor::translate_crc_indication(fapi::crc_indication&   indication,
                                      slot_point              slot,
                                      const ::vendor_crc_pdu& pdu,
                                      const pusch_context&    context)
{
  // TODO: Convert the measurements to the units expected by FAPI. In this example the SINR is reported in 0.01 dB
  // steps and the timing advance follows TS38.213 Section 4.2, so it must be subtracted by 31. Report the RSSI and RSRP
  // as fapi::fapi_power_unit if the PHY measures them.
  float sinr_dB = static_cast<float>(pdu.sinr) * 0.01F;
  int   ta      = static_cast<int>(pdu.timing_advance) - 31;

  fapi::crc_indication_builder builder(indication);
  builder.set_slot(slot);
  builder.set_pdu(to_rnti(pdu.rnti),
                  context.harq_process_id,
                  pdu.crc_ok != 0,
                  sinr_dB,
                  phy_time_unit::from_timing_advance(ta, slot.scs()),
                  std::nullopt,
                  std::nullopt);
}
