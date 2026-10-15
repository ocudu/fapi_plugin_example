// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_p7_requests_transmitter.h"
#include "hal/hal.h"
#include "messages/dl_tti_request_translator.h"
#include "messages/tx_data_request_translator.h"
#include "messages/ul_dci_request_translator.h"
#include "messages/ul_tti_request_translator.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

template <typename Func>
void vendor_p7_requests_transmitter::send_message(unsigned msg_id, Func&& fill)
{
  // The first message of the slot starts a new transaction.
  if (!message_in_slot) {
    if (!hal.get_memory_send_sequencer().mark_send_start()) {
      logger.error("Unable to mark the send start");
      return;
    }
    message_in_slot = true;
  }

  uint8_t* buffer = hal.get_memory_allocator().allocate();
  if (!buffer) {
    logger.error("Unable to allocate memory for message id '{}'", msg_id);
    return;
  }

  unsigned msg_size = fill(buffer);
  if (msg_size == 0) {
    logger.warning("Unable to translate message id '{}'", msg_id);
    hal.get_memory_allocator().deallocate(buffer);
    return;
  }

  hal.get_binary_message_gateway().send(msg_id, {buffer, msg_size});
}

void vendor_p7_requests_transmitter::send_dl_tti_request(const fapi::dl_tti_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_DL_TTI_REQ, [&](uint8_t* buffer) {
    return translate_dl_tti_request(*reinterpret_cast<::vendor_dl_tti_request*>(buffer), msg, logger);
  });
}

void vendor_p7_requests_transmitter::send_ul_tti_request(const fapi::ul_tti_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_UL_TTI_REQ, [&](uint8_t* buffer) {
    return translate_ul_tti_request(
        *reinterpret_cast<::vendor_ul_tti_request*>(buffer), msg, pusch_repo, pucch_repo, logger);
  });
}

void vendor_p7_requests_transmitter::send_ul_dci_request(const fapi::ul_dci_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_UL_DCI_REQ, [&](uint8_t* buffer) {
    return translate_ul_dci_request(*reinterpret_cast<::vendor_ul_dci_request*>(buffer), msg);
  });
}

void vendor_p7_requests_transmitter::send_tx_data_request(const fapi::tx_data_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_TX_DATA_REQ, [&](uint8_t* buffer) {
    return translate_tx_data_request(
        *reinterpret_cast<::vendor_tx_data_request*>(buffer), msg, hal.get_data_memory_writer(), logger);
  });
}

void vendor_p7_requests_transmitter::on_last_message(slot_point slot)
{
  if (!message_in_slot) {
    logger.error("No FAPI messages in slot={}", slot);
    return;
  }

  // Send all the messages of the slot to the PHY.
  if (!hal.get_memory_send_sequencer().mark_send_end()) {
    logger.error("Unable to send the messages of slot={}", slot);
  }

  message_in_slot = false;
}
