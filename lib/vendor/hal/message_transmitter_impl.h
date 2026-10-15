// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "hal.h"
#include "mac2phy_message_list.h"
#include "transport.h"

namespace ocudu {
namespace vendor {
namespace hal {

/// \brief TX control path implementation.
///
/// Messages are written in memory blocks taken from the memory pool, queued in the current transaction and sent to the
/// PHY when the end of the transaction is marked.
class message_transmitter_impl : public memory_allocator, public binary_message_gateway, public memory_send_sequencer
{
public:
  message_transmitter_impl(memory_pool&                  pool_,
                           mac2phy_message_list_manager& msg_list_manager_,
                           const transport_context&      context_,
                           ocudulog::basic_logger&       logger_) :
    pool(pool_), msg_list_manager(msg_list_manager_), context(context_), logger(logger_)
  {
  }

  // See interface for documentation.
  uint8_t* allocate() override;

  // See interface for documentation.
  void deallocate(uint8_t* block) override { pool.deallocate(block); }

  // See interface for documentation.
  void send(unsigned msg_id, span<const uint8_t> msg) override;

  // See interface for documentation.
  bool mark_send_start() override;

  // See interface for documentation.
  bool mark_send_end() override;

private:
  memory_pool&                  pool;
  mac2phy_message_list_manager& msg_list_manager;
  const transport_context&      context;
  ocudulog::basic_logger&       logger;
};

} // namespace hal
} // namespace vendor
} // namespace ocudu
