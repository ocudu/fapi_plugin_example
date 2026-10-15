// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ul_tti_request_translator.h"
#include "ocudu/fapi/p7/messages/ul_tti_request.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

/// Translates the FAPI PRACH PDU. Returns the PDU size in bytes.
static unsigned translate_prach_pdu(::vendor_prach_pdu& pdu, const fapi::ul_prach_pdu& fapi_pdu)
{
  pdu.nof_prach_occasions = fapi_pdu.num_prach_ocas;
  pdu.prach_format        = static_cast<uint8_t>(fapi_pdu.prach_format);
  pdu.start_symbol        = fapi_pdu.prach_start_symbol;

  // TODO: Translate the rest of the PRACH parameters (frequency domain occasions, cyclic shifts, beamforming, etc).

  pdu.hdr.pdu_type = VENDOR_UL_PDU_TYPE_PRACH;
  pdu.hdr.pdu_size = sizeof(pdu);
  return pdu.hdr.pdu_size;
}

/// Translates the FAPI PUSCH PDU. Returns the PDU size in bytes.
static unsigned translate_pusch_pdu(::vendor_pusch_pdu& pdu, const fapi::ul_pusch_pdu& fapi_pdu, unsigned ue_id)
{
  pdu.rnti         = static_cast<uint16_t>(fapi_pdu.rnti);
  pdu.ue_id        = ue_id;
  pdu.bwp_start    = fapi_pdu.bwp.start();
  pdu.bwp_size     = fapi_pdu.bwp.length();
  pdu.rb_start     = fapi_pdu.resource_allocation_1.vrbs.start();
  pdu.rb_size      = fapi_pdu.resource_allocation_1.vrbs.length();
  pdu.start_symbol = fapi_pdu.symbols.start();
  pdu.nof_symbols  = fapi_pdu.symbols.length();
  pdu.nof_layers   = fapi_pdu.num_layers;
  pdu.mcs_index    = fapi_pdu.mcs_index;

  if (fapi_pdu.pusch_data) {
    pdu.tb_size  = fapi_pdu.pusch_data->tb_size.value();
    pdu.rv_index = fapi_pdu.pusch_data->rv_index;
    pdu.harq_id  = fapi_pdu.pusch_data->harq_process_id;
    pdu.new_data = fapi_pdu.pusch_data->new_data;
  }

  // TODO: Translate the MCS table, transform precoding, DMRS configuration, scrambling identifiers and the UCI
  // multiplexing parameters (fapi_pdu.pusch_uci).

  pdu.hdr.pdu_type = VENDOR_UL_PDU_TYPE_PUSCH;
  pdu.hdr.pdu_size = sizeof(pdu);
  return pdu.hdr.pdu_size;
}

/// Translates the FAPI PUCCH PDU and returns its context. The PDU size is written in the PDU header.
static pucch_context translate_pucch_pdu(::vendor_pucch_pdu& pdu, const fapi::ul_pucch_pdu& fapi_pdu, unsigned ue_id)
{
  pdu.rnti         = static_cast<uint16_t>(fapi_pdu.rnti);
  pdu.ue_id        = ue_id;
  pdu.prb_start    = fapi_pdu.prbs.start();
  pdu.prb_size     = fapi_pdu.prbs.length();
  pdu.start_symbol = fapi_pdu.symbols.start();
  pdu.nof_symbols  = fapi_pdu.symbols.length();

  // Expected UCI bit lengths, needed to build the UCI.indication.
  pucch_context context = {units::bits(0), units::bits(0), units::bits(0), pucch_format::FORMAT_0};
  if (const auto* f0 = std::get_if<fapi::ul_pucch_pdu_format_0>(&fapi_pdu.format)) {
    context = {units::bits(f0->sr_present), f0->bit_len_harq, units::bits(0), pucch_format::FORMAT_0};
  } else if (const auto* f1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format)) {
    context = {units::bits(f1->sr_present), f1->bit_len_harq, units::bits(0), pucch_format::FORMAT_1};
  } else if (const auto* f2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format)) {
    context = {units::bits(static_cast<uint8_t>(f2->sr_bit_len)),
               f2->bit_len_harq,
               f2->csi_part1_bit_length,
               pucch_format::FORMAT_2};
  } else if (const auto* f3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format)) {
    context = {units::bits(static_cast<uint8_t>(f3->sr_bit_len)),
               f3->bit_len_harq,
               f3->csi_part1_bit_length,
               pucch_format::FORMAT_3};
  } else if (const auto* f4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format)) {
    context = {units::bits(static_cast<uint8_t>(f4->sr_bit_len)),
               f4->bit_len_harq,
               f4->csi_part1_bit_length,
               pucch_format::FORMAT_4};
  }

  pdu.format            = static_cast<uint8_t>(context.format);
  pdu.sr_present        = context.sr_bitlength.value() != 0;
  pdu.harq_bit_len      = context.harq_bitlength.value();
  pdu.csi_part1_bit_len = context.csi_bitlength.value();

  // TODO: Translate the rest of the format specific parameters (initial cyclic shift, time domain OCC, hopping, DMRS,
  // scrambling, etc).

  pdu.hdr.pdu_type = VENDOR_UL_PDU_TYPE_PUCCH;
  pdu.hdr.pdu_size = sizeof(pdu);
  return context;
}

unsigned vendor::translate_ul_tti_request(::vendor_ul_tti_request&    ul_tti_request,
                                          const fapi::ul_tti_request& input,
                                          pusch_context_repository&   pusch_repo,
                                          pucch_context_repository&   pucch_repo,
                                          ocudulog::basic_logger&     logger)
{
  ul_tti_request.sfn_slot = {
      static_cast<uint16_t>(input.slot.sfn()), static_cast<uint16_t>(input.slot.slot_index()), 0};
  ul_tti_request.nof_pdus     = 0;
  ul_tti_request.rach_present = 0;
  ul_tti_request.nof_pusch    = 0;
  ul_tti_request.nof_pucch    = 0;

  // The PDUs are written one after the other, right after the UL_TTI.request structure.
  uint8_t* pdu_area        = reinterpret_cast<uint8_t*>(&ul_tti_request + 1);
  unsigned next_pdu_offset = 0;

  // TODO: Check the PHY limits, e.g. the maximum number of PDUs of each type per slot.
  for (const auto& fapi_pdu : input.pdus) {
    uint8_t* pdu_base = pdu_area + next_pdu_offset;

    if (const auto* prach = std::get_if<fapi::ul_prach_pdu>(&fapi_pdu.pdu)) {
      next_pdu_offset += translate_prach_pdu(*reinterpret_cast<::vendor_prach_pdu*>(pdu_base), *prach);
      ul_tti_request.rach_present = 1;
    } else if (const auto* pusch = std::get_if<fapi::ul_pusch_pdu>(&fapi_pdu.pdu)) {
      // The PUSCH index within the slot identifies the context when the CRC and RX_Data indications arrive.
      unsigned ue_id = ul_tti_request.nof_pusch++;
      if (pusch->pusch_data) {
        pusch_repo.add(input.slot, ue_id, {pusch->pusch_data->harq_process_id});
      }
      next_pdu_offset += translate_pusch_pdu(*reinterpret_cast<::vendor_pusch_pdu*>(pdu_base), *pusch, ue_id);
    } else if (const auto* pucch = std::get_if<fapi::ul_pucch_pdu>(&fapi_pdu.pdu)) {
      // The PUCCH index within the slot identifies the context when the UCI indication arrives.
      unsigned ue_id = ul_tti_request.nof_pucch++;
      auto&    pdu   = *reinterpret_cast<::vendor_pucch_pdu*>(pdu_base);
      pucch_repo.add(input.slot, ue_id, translate_pucch_pdu(pdu, *pucch, ue_id));
      next_pdu_offset += pdu.hdr.pdu_size;
    } else {
      logger.warning("Skipping unsupported PDU in UL_TTI.request for slot={}", input.slot);
      continue;
    }
    ++ul_tti_request.nof_pdus;
  }

  ul_tti_request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_UL_TTI_REQ;
  ul_tti_request.hdr.msg_len  = sizeof(ul_tti_request) + next_pdu_offset;
  return ul_tti_request.hdr.msg_len;
}
