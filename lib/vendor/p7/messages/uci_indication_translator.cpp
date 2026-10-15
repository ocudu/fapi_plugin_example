// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "uci_indication_translator.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/fapi/p7/builders/uci_indication_builder.h"
#include "ocudu/fapi/p7/builders/uci_pucch_pdu_format_0_1_builder.h"
#include "ocudu/fapi/p7/builders/uci_pucch_pdu_format_2_3_4_builder.h"
#include "ocudu/fapi/p7/builders/uci_pusch_pdu_builder.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

void vendor::translate_uci_indication(fapi::uci_indication&         indication,
                                      slot_point                    slot,
                                      const ::vendor_pucch_uci_pdu& pdu,
                                      const pucch_context&          context)
{
  fapi::uci_indication_builder builder(indication);
  builder.set_slot(slot);

  // TODO: Report the measurements (SINR, timing advance, RSSI, RSRP) with set_metrics_parameters() and
  // set_time_advance() on the PDU builders.

  if (context.format == pucch_format::FORMAT_0 || context.format == pucch_format::FORMAT_1) {
    fapi::uci_pucch_pdu_format_0_1_builder pdu_builder = builder.add_format_0_1_pucch_pdu();
    pdu_builder.set_ue_specific_parameters(to_rnti(pdu.rnti)).set_format(context.format);

    if (context.sr_bitlength.value() != 0) {
      pdu_builder.set_sr_parameters(pdu.sr_detected != 0);
    }

    // The number of HARQ-ACK bits must match the one expected by the scheduler.
    // TODO: Report DTX for the bits that the PHY did not detect.
    static_vector<uci_pucch_f0_or_f1_harq_values, 2> harq_values;
    for (unsigned i = 0, e = std::min(context.harq_bitlength.value(), 2U); i != e; ++i) {
      harq_values.push_back((pdu.harq_bits[i] != 0) ? uci_pucch_f0_or_f1_harq_values::ack
                                                    : uci_pucch_f0_or_f1_harq_values::nack);
    }
    if (!harq_values.empty()) {
      pdu_builder.set_harq_parameters(harq_values);
    }
    return;
  }

  fapi::uci_pucch_pdu_format_2_3_4_builder pdu_builder = builder.add_format_2_3_4_pucch_pdu();
  pdu_builder.set_ue_specific_parameters(to_rnti(pdu.rnti)).set_format(context.format);
  // TODO: Report the SR, HARQ-ACK and CSI part 1/2 payloads and their detection status, using the expected bit lengths
  // stored in the PUCCH context.
}

void vendor::translate_uci_indication(fapi::uci_indication&         indication,
                                      slot_point                    slot,
                                      const ::vendor_pusch_uci_pdu& pdu)
{
  fapi::uci_indication_builder builder(indication);
  builder.set_slot(slot);

  fapi::uci_pusch_pdu_builder pdu_builder = builder.add_pusch_pdu();
  pdu_builder.set_ue_specific_parameters(to_rnti(pdu.rnti));
  // TODO: Report the HARQ-ACK and CSI part 1/2 payloads multiplexed on PUSCH and their detection status, with
  // set_harq_parameters() and set_csi_part1_parameters().
}
