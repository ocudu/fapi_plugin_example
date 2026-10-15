// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "hal.h"
#include "memory_pool.h"
#include "transport.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {
namespace vendor {
namespace hal {

/// \brief RX path implementation.
///
/// Polls the PHY for new messages from the given executor, forwards them to the notifier and returns the processed
/// memory blocks to the memory pool. It also keeps the PHY supplied with free memory blocks to write uplink messages.
class incoming_messages_dispatcher : public message_dispatcher_operation_controller
{
public:
  incoming_messages_dispatcher(memory_pool&             pool_,
                               const transport_context& context_,
                               task_executor&           executor_,
                               ocudulog::basic_logger&  logger_) :
    pool(pool_), context(context_), executor(executor_), logger(logger_)
  {
  }

  // See interface for documentation.
  void start(binary_message_notifier& notifier) override;

  // See interface for documentation.
  void stop() override;

  /// Hands free memory blocks to the PHY until it rejects them. Returns the number of blocks handed over.
  unsigned provide_rx_blocks();

private:
  /// Polls the PHY and reschedules itself until stopped.
  void receive_loop();

  /// Extracts all the pending messages from the PHY.
  void extract_messages();

  memory_pool&             pool;
  const transport_context& context;
  task_executor&           executor;
  ocudulog::basic_logger&  logger;
  binary_message_notifier* notifier = nullptr;
  stop_event_source        stop_manager;
};

} // namespace hal
} // namespace vendor
} // namespace ocudu
