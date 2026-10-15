// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "tx_data_request_translator.h"
#include "hal/hal.h"
#include "ocudu/fapi/p7/messages/tx_data_request.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

unsigned vendor::translate_tx_data_request(::vendor_tx_data_request&    tx_data_request,
                                           const fapi::tx_data_request& input,
                                           hal::data_memory_writer&     data_mem_writer,
                                           ocudulog::basic_logger&      logger)
{
  tx_data_request.sfn_slot = {
      static_cast<uint16_t>(input.slot.sfn()), static_cast<uint16_t>(input.slot.slot_index()), 0};
  tx_data_request.nof_pdus = 0;

  for (const auto& fapi_pdu : input.pdus) {
    span<const uint8_t> tb = fapi_pdu.pdu.get_buffer();

    // Copy the transport block into the data plane memory.
    uint8_t* phy_addr = data_mem_writer.write(tb, fapi_pdu.cw_index);
    if (!phy_addr) {
      logger.warning("Failed to write {} bytes of TX_Data for slot={}", tb.size(), input.slot);
      return 0;
    }

    ::vendor_tx_data_pdu& pdu = tx_data_request.pdus[tx_data_request.nof_pdus++];
    pdu.pdu_index             = fapi_pdu.pdu_index;
    pdu.pdu_length            = tb.size();
    pdu.payload               = phy_addr;
  }

  tx_data_request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_TX_DATA_REQ;
  tx_data_request.hdr.msg_len  = sizeof(tx_data_request);
  return tx_data_request.hdr.msg_len;
}
