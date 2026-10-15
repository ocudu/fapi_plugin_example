// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_p5_requests_transmitter.h"
#include "config_request_translator.h"
#include "hal/hal.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

template <typename Func>
void vendor_p5_requests_transmitter::send_message(unsigned msg_id, Func&& fill)
{
  if (!hal.get_memory_send_sequencer().mark_send_start()) {
    logger.error("Unable to mark the send start of message id '{}'", msg_id);
    return;
  }

  uint8_t* buffer = hal.get_memory_allocator().allocate();
  if (!buffer) {
    logger.error("Unable to allocate memory for message id '{}'", msg_id);
    return;
  }

  unsigned msg_size = fill(buffer);
  hal.get_binary_message_gateway().send(msg_id, {buffer, msg_size});

  if (!hal.get_memory_send_sequencer().mark_send_end()) {
    logger.error("Unable to mark the send end of message id '{}'", msg_id);
  }
}

void vendor_p5_requests_transmitter::send_param_request(const fapi::param_request& msg)
{
  // TODO: If the PHY supports PARAM.request, translate and send it like the other requests. Otherwise, answer it
  // locally to indicate that the PARAM transaction was a success.
  report_error_if_not(p5_notifier, "P5 responses notifier not set");

  fapi::param_response response;
  response.error_code = fapi::error_code_id::msg_ok;
  p5_notifier->on_param_response(response);
}

void vendor_p5_requests_transmitter::send_config_request(const fapi::config_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_CONFIG_REQ, [&msg](uint8_t* buffer) {
    return translate_config_request(*reinterpret_cast<::vendor_config_request*>(buffer), msg);
  });
}

void vendor_p5_requests_transmitter::send_start_request(const fapi::start_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_START_REQ, [](uint8_t* buffer) {
    auto& request = *reinterpret_cast<::vendor_start_request*>(buffer);
    // TODO: Fill the START.request fields required by the PHY.
    request.sfn_slot     = {};
    request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_START_REQ;
    request.hdr.msg_len  = sizeof(request);
    return request.hdr.msg_len;
  });
}

void vendor_p5_requests_transmitter::send_stop_request(const fapi::stop_request& msg)
{
  send_message(VENDOR_MSG_TYPE_PHY_STOP_REQ, [](uint8_t* buffer) {
    auto& request = *reinterpret_cast<::vendor_stop_request*>(buffer);
    // TODO: Fill the STOP.request fields required by the PHY.
    request.sfn_slot     = {};
    request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_STOP_REQ;
    request.hdr.msg_len  = sizeof(request);
    return request.hdr.msg_len;
  });
}
