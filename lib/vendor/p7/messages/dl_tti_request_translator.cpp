// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "dl_tti_request_translator.h"
#include "ocudu/fapi/p7/messages/dl_tti_request.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"
#include <algorithm>

using namespace ocudu;
using namespace vendor;

/// Translates the FAPI SSB PDU. Returns the PDU size in bytes.
static unsigned translate_ssb_pdu(::vendor_ssb_pdu& pdu, const fapi::dl_ssb_pdu& fapi_pdu)
{
  pdu.phy_cell_id           = fapi_pdu.phys_cell_id;
  pdu.ssb_block_index       = fapi_pdu.ssb_block_index.value();
  pdu.ssb_subcarrier_offset = fapi_pdu.subcarrier_offset.value();
  pdu.ssb_offset_point_a    = fapi_pdu.ssb_offset_pointA.value();
  pdu.bch_payload           = fapi_pdu.bch_payload;

  // TODO: Translate the power configuration and the precoding and beamforming information.

  pdu.hdr.pdu_type = VENDOR_DL_PDU_TYPE_SSB;
  pdu.hdr.pdu_size = sizeof(pdu);
  return pdu.hdr.pdu_size;
}

void vendor::translate_pdcch_pdu(::vendor_pdcch_pdu& pdu, const fapi::dl_pdcch_pdu& fapi_pdu)
{
  const fapi::dl_dci_pdu& fapi_dci = fapi_pdu.dl_dci;

  pdu.rnti              = static_cast<uint16_t>(fapi_dci.rnti);
  pdu.bwp_start         = fapi_pdu.coreset_bwp.start();
  pdu.bwp_size          = fapi_pdu.coreset_bwp.length();
  pdu.start_symbol      = fapi_pdu.symbols.start();
  pdu.duration_symbols  = fapi_pdu.symbols.length();
  pdu.aggregation_level = to_nof_cces(fapi_dci.dci_aggregation_level);
  pdu.cce_index         = fapi_dci.cce_index;

  // DCI payload.
  pdu.payload_size_bits = fapi_dci.payload.size();
  std::fill(std::begin(pdu.payload), std::end(pdu.payload), 0);
  fapi_dci.payload.to_packed_bits(span<uint8_t>(pdu.payload));

  // TODO: Translate the CORESET configuration (frequency domain resources, REG bundle and interleaving), the DCI
  // scrambling parameters, the power configuration and the precoding and beamforming information.

  pdu.hdr.pdu_type = VENDOR_DL_PDU_TYPE_PDCCH;
  pdu.hdr.pdu_size = sizeof(pdu);
}

/// Translates the FAPI PDSCH PDU. Returns the PDU size in bytes.
static unsigned translate_pdsch_pdu(::vendor_pdsch_pdu& pdu, const fapi::dl_pdsch_pdu& fapi_pdu, unsigned ue_id)
{
  pdu.rnti         = static_cast<uint16_t>(fapi_pdu.rnti);
  pdu.ue_id        = ue_id;
  pdu.bwp_start    = fapi_pdu.bwp.start();
  pdu.bwp_size     = fapi_pdu.bwp.length();
  pdu.start_symbol = fapi_pdu.symbols.start();
  pdu.nof_symbols  = fapi_pdu.symbols.length();

  if (!fapi_pdu.cws.empty()) {
    const fapi::dl_pdsch_codeword& cw = fapi_pdu.cws.front();
    pdu.mcs_index                     = cw.mcs_index.value();
    pdu.rv_index                      = cw.rv_index;
    pdu.tb_size                       = cw.tb_size.value();
  }

  // TODO: Translate the frequency domain allocation, the number of layers, the DMRS configuration, the power
  // configuration and the precoding and beamforming information.
  pdu.rb_start   = 0;
  pdu.rb_size    = 0;
  pdu.nof_layers = 1;

  pdu.hdr.pdu_type = VENDOR_DL_PDU_TYPE_PDSCH;
  pdu.hdr.pdu_size = sizeof(pdu);
  return pdu.hdr.pdu_size;
}

/// Translates the FAPI CSI-RS PDU. Returns the PDU size in bytes.
static unsigned translate_csi_rs_pdu(::vendor_csi_rs_pdu& pdu, const fapi::dl_csi_rs_pdu& fapi_pdu)
{
  pdu.bwp_start     = fapi_pdu.bwp.start();
  pdu.bwp_size      = fapi_pdu.bwp.length();
  pdu.start_rb      = fapi_pdu.crbs.start();
  pdu.nof_rbs       = fapi_pdu.crbs.length();
  pdu.row           = fapi_pdu.row;
  pdu.symbol_l0     = fapi_pdu.symb_L0;
  pdu.scrambling_id = fapi_pdu.scramb_id;

  // TODO: Translate the CSI-RS type, frequency domain allocation, CDM type, frequency density and power configuration.

  pdu.hdr.pdu_type = VENDOR_DL_PDU_TYPE_CSI_RS;
  pdu.hdr.pdu_size = sizeof(pdu);
  return pdu.hdr.pdu_size;
}

unsigned vendor::translate_dl_tti_request(::vendor_dl_tti_request&    dl_tti_request,
                                          const fapi::dl_tti_request& input,
                                          ocudulog::basic_logger&     logger)
{
  dl_tti_request.sfn_slot = {
      static_cast<uint16_t>(input.slot.sfn()), static_cast<uint16_t>(input.slot.slot_index()), 0};
  dl_tti_request.nof_pdus = 0;
  dl_tti_request.nof_dcis = 0;

  // The PDUs are written one after the other, right after the DL_TTI.request structure.
  uint8_t* pdu_area        = reinterpret_cast<uint8_t*>(&dl_tti_request + 1);
  unsigned next_pdu_offset = 0;
  unsigned pdsch_index     = 0;

  // TODO: Check the PHY limits, e.g. the maximum number of PDUs of each type per slot.
  for (const auto& fapi_pdu : input.pdus) {
    uint8_t* pdu_base = pdu_area + next_pdu_offset;

    if (const auto* ssb = std::get_if<fapi::dl_ssb_pdu>(&fapi_pdu.pdu)) {
      next_pdu_offset += translate_ssb_pdu(*reinterpret_cast<::vendor_ssb_pdu*>(pdu_base), *ssb);
    } else if (const auto* pdcch = std::get_if<fapi::dl_pdcch_pdu>(&fapi_pdu.pdu)) {
      auto& pdu = *reinterpret_cast<::vendor_pdcch_pdu*>(pdu_base);
      translate_pdcch_pdu(pdu, *pdcch);
      next_pdu_offset += pdu.hdr.pdu_size;
      ++dl_tti_request.nof_dcis;
    } else if (const auto* pdsch = std::get_if<fapi::dl_pdsch_pdu>(&fapi_pdu.pdu)) {
      next_pdu_offset += translate_pdsch_pdu(*reinterpret_cast<::vendor_pdsch_pdu*>(pdu_base), *pdsch, pdsch_index++);
    } else if (const auto* csi_rs = std::get_if<fapi::dl_csi_rs_pdu>(&fapi_pdu.pdu)) {
      next_pdu_offset += translate_csi_rs_pdu(*reinterpret_cast<::vendor_csi_rs_pdu*>(pdu_base), *csi_rs);
    } else {
      logger.warning("Skipping unsupported PDU in DL_TTI.request for slot={}", input.slot);
      continue;
    }
    ++dl_tti_request.nof_pdus;
  }

  dl_tti_request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_DL_TTI_REQ;
  dl_tti_request.hdr.msg_len  = sizeof(dl_tti_request) + next_pdu_offset;
  return dl_tti_request.hdr.msg_len;
}
