// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "hal/hal.h"
#include "p5/vendor_p5_responses_receiver.h"
#include "p7/vendor_p7_indications_receiver.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

namespace ocudu {
namespace vendor {

/// Routes the messages received from the PHY to the P5 or the P7 receiver.
class vendor_rx_message_handler : public hal::binary_message_notifier
{
public:
  vendor_rx_message_handler(vendor_p5_responses_receiver&   p5_receiver_,
                            vendor_p7_indications_receiver& p7_receiver_,
                            ocudulog::basic_logger&         logger_) :
    p5_receiver(p5_receiver_), p7_receiver(p7_receiver_), logger(logger_)
  {
  }

  // See interface for documentation.
  void on_new_message(span<const uint8_t> msg) override
  {
    if (OCUDU_UNLIKELY(msg.size() < sizeof(::vendor_msg_header))) {
      logger.warning("Dropping message of '{}' bytes, smaller than the message header", msg.size());
      return;
    }

    // TODO: Map the message types of your PHY.
    switch (reinterpret_cast<const ::vendor_msg_header*>(msg.data())->msg_type) {
      case VENDOR_MSG_TYPE_PHY_CONFIG_RESP:
      case VENDOR_MSG_TYPE_PHY_START_RESP:
      case VENDOR_MSG_TYPE_PHY_STOP_RESP:
        p5_receiver.handle_new_message(msg);
        break;
      // ERROR.indications are handled by the P7 receiver, as they are usually associated with a slot.
      case VENDOR_MSG_TYPE_PHY_ERR_IND:
      case VENDOR_MSG_TYPE_PHY_SLOT_IND:
      case VENDOR_MSG_TYPE_PHY_RACH_IND:
      case VENDOR_MSG_TYPE_PHY_CRC_IND:
      case VENDOR_MSG_TYPE_PHY_RX_DATA_IND:
      case VENDOR_MSG_TYPE_PHY_UCI_IND:
      case VENDOR_MSG_TYPE_PHY_PUSCH_UCI_IND:
        p7_receiver.handle_new_message(msg);
        break;
      default:
        logger.warning("Dropping unsupported message with id '{}'",
                       reinterpret_cast<const ::vendor_msg_header*>(msg.data())->msg_type);
    }
  }

private:
  vendor_p5_responses_receiver&   p5_receiver;
  vendor_p7_indications_receiver& p7_receiver;
  ocudulog::basic_logger&         logger;
};

} // namespace vendor
} // namespace ocudu
