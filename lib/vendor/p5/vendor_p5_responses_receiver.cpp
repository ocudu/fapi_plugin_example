// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_p5_responses_receiver.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

/// Notifies an ERROR.indication for the given failed request.
static void notify_request_failure(fapi::error_indication_notifier& notifier, fapi::message_type_id msg_id)
{
  fapi::error_indication error_msg;
  error_msg.message_id = msg_id;
  error_msg.error_code = fapi::error_code_id::msg_invalid_state;
  notifier.on_error_indication(error_msg);
}

void vendor_p5_responses_receiver::handle_new_message(span<const uint8_t> msg)
{
  report_error_if_not(p5_notifier && error_notifier, "P5 notifiers not set");

  const auto& msg_header = *reinterpret_cast<const ::vendor_msg_header*>(msg.data());

  // TODO: Map the status codes of your PHY.
  switch (msg_header.msg_type) {
    case VENDOR_MSG_TYPE_PHY_CONFIG_RESP: {
      const auto& response = *reinterpret_cast<const ::vendor_config_response*>(msg.data());
      logger.debug("CONFIG.response received with status {}", response.status);

      fapi::config_response fapi_response;
      fapi_response.error_code =
          (response.status == 0) ? fapi::error_code_id::msg_ok : fapi::error_code_id::msg_invalid_state;
      p5_notifier->on_config_response(fapi_response);
      break;
    }
    case VENDOR_MSG_TYPE_PHY_START_RESP: {
      const auto& response = *reinterpret_cast<const ::vendor_start_response*>(msg.data());
      logger.debug("START.response received with status {}", response.status);

      // FAPI has no START.response: success is signalled by the first SLOT.indication, failure by an ERROR.indication.
      if (response.status != 0) {
        notify_request_failure(*error_notifier, fapi::message_type_id::start_request);
      }
      break;
    }
    case VENDOR_MSG_TYPE_PHY_STOP_RESP: {
      const auto& response = *reinterpret_cast<const ::vendor_stop_response*>(msg.data());
      logger.debug("STOP.response received with status {}", response.status);

      if (response.status == 0) {
        p5_notifier->on_stop_indication({});
      } else {
        notify_request_failure(*error_notifier, fapi::message_type_id::stop_request);
      }
      break;
    }
    default:
      report_error("Invalid message id '{}'. This receiver only handles P5 messages", msg_header.msg_type);
  }
}
