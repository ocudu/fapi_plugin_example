// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "incoming_messages_dispatcher.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <thread>

using namespace ocudu;
using namespace vendor;
using namespace hal;

void incoming_messages_dispatcher::start(binary_message_notifier& notifier_)
{
  stop_manager.reset();
  notifier = &notifier_;
  sync_event start_event;

  if (!executor.defer([this, token = start_event.get_token()]() { receive_loop(); })) {
    report_error("Unable to start the vendor HAL receiver");
  }

  // Block waiting for the executor to start.
  start_event.wait();

  logger.info("HAL: Started the receiver");
}

void incoming_messages_dispatcher::stop()
{
  // Blocks until the receive loop has finished.
  stop_manager.stop();
  notifier = nullptr;

  logger.info("HAL: Stopped the receiver");
}

unsigned incoming_messages_dispatcher::provide_rx_blocks()
{
  unsigned nof_blocks = 0;
  while (void* block = pool.allocate()) {
    if (!transport_provide_rx_block(context, block)) {
      pool.deallocate(block);
      break;
    }
    ++nof_blocks;
  }
  return nof_blocks;
}

void incoming_messages_dispatcher::receive_loop()
{
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  extract_messages();

  // Retry the task deferring when it fails.
  while (!executor.defer([this, tk = std::move(token)]() { receive_loop(); })) {
    std::this_thread::sleep_for(std::chrono::microseconds(20));
  }
}

void incoming_messages_dispatcher::extract_messages()
{
  unsigned nof_messages = 0;

  while (std::optional<transport_rx_message> msg = transport_receive(context)) {
    ++nof_messages;
    logger.debug("HAL: Received message of '{}' bytes from the PHY", msg->size);

    notifier->on_new_message({msg->data, msg->size});

    // The message has been processed, return its memory block to the pool.
    pool.deallocate(msg->block);
  }

  if (nof_messages) {
    // Replace the blocks consumed by the PHY.
    provide_rx_blocks();
  } else {
    // TODO: Tune the polling interval to the latency requirements of the PHY.
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}
