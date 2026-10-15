// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "message_transmitter_impl.h"
#include <algorithm>

using namespace ocudu;
using namespace vendor;
using namespace hal;

/// Returns the position of the message with the given index in a transaction of the given size.
static transport_batch_position get_batch_position(unsigned idx, unsigned size)
{
  if (size == 1) {
    return transport_batch_position::single;
  }
  if (idx == 0) {
    return transport_batch_position::first;
  }
  if (idx == size - 1) {
    return transport_batch_position::last;
  }
  return transport_batch_position::middle;
}

uint8_t* message_transmitter_impl::allocate()
{
  // TODO: If the PHY requires a header in front of each message, reserve the space for it here.
  return static_cast<uint8_t*>(pool.allocate());
}

void message_transmitter_impl::send(unsigned msg_id, span<const uint8_t> msg)
{
  auto& fapi_msgs = msg_list_manager.get_current_list().fapi_msgs;
  if (fapi_msgs.full()) {
    logger.error("HAL: Dropping message id '{}', as the maximum number of messages per transaction was reached",
                 msg_id);
    pool.deallocate(const_cast<uint8_t*>(msg.data()));
    return;
  }

  fapi_msgs.push_back({const_cast<uint8_t*>(msg.data()), unsigned(msg.size()), msg_id});

  logger.debug("HAL: Queued message id '{}' of '{}' bytes", msg_id, msg.size());
}

bool message_transmitter_impl::mark_send_start()
{
  msg_list_manager.recycle_message_list();

  return true;
}

bool message_transmitter_impl::mark_send_end()
{
  mac2phy_message_list& list = msg_list_manager.get_current_list();

  if (list.fapi_msgs.empty()) {
    logger.debug("HAL: No messages to send");
    return true;
  }

  // TODO: Reorder the messages here if the PHY requires a specific message order within a slot.

  // FAPI messages are sent first, followed by the data messages.
  unsigned total = list.fapi_msgs.size() + list.data_msgs.size();
  unsigned idx   = 0;
  auto     send  = [&](const memory_block_context& msg) {
    if (!transport_send(context, msg.addr, msg.size, msg.msg_type, get_batch_position(idx++, total))) {
      logger.error("HAL: Failed to send message id '{}' to the PHY", msg.msg_type);
      return false;
    }
    return true;
  };
  if (!std::all_of(list.fapi_msgs.begin(), list.fapi_msgs.end(), send) ||
      !std::all_of(list.data_msgs.begin(), list.data_msgs.end(), send)) {
    return false;
  }

  logger.debug("HAL: Sent '{}' messages to the PHY", total);
  return true;
}
