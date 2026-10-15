// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ul_dci_request_translator.h"
#include "dl_tti_request_translator.h"
#include "ocudu/fapi/p7/messages/ul_dci_request.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

unsigned vendor::translate_ul_dci_request(::vendor_ul_dci_request& ul_dci_request, const fapi::ul_dci_request& input)
{
  ul_dci_request.sfn_slot = {
      static_cast<uint16_t>(input.slot.sfn()), static_cast<uint16_t>(input.slot.slot_index()), 0};
  ul_dci_request.nof_dcis = 0;

  for (const auto& ul_dci_pdu : input.pdus) {
    translate_pdcch_pdu(ul_dci_request.dcis[ul_dci_request.nof_dcis++], ul_dci_pdu.pdu);
  }

  ul_dci_request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_UL_DCI_REQ;
  ul_dci_request.hdr.msg_len  = sizeof(ul_dci_request);
  return ul_dci_request.hdr.msg_len;
}
