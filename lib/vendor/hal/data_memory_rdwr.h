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

/// Data path implementation.
class data_memory_rdwr : public data_memory_reader, public data_memory_writer
{
public:
  data_memory_rdwr(memory_pool&                  pool_,
                   mac2phy_message_list_manager& msg_list_manager_,
                   const transport_context&      context_,
                   ocudulog::basic_logger&       logger_) :
    pool(pool_), msg_list_manager(msg_list_manager_), context(context_), logger(logger_)
  {
  }

  // See interface for documentation.
  unique_memory_block read(void* phy_addr) override;

  // See interface for documentation.
  uint8_t* write(span<const uint8_t> data, unsigned tb_idx) override;

private:
  // See interface for documentation.
  void deallocate_buffer(const void* ptr) override;

  memory_pool&                  pool;
  mac2phy_message_list_manager& msg_list_manager;
  const transport_context&      context;
  ocudulog::basic_logger&       logger;
};

} // namespace hal
} // namespace vendor
} // namespace ocudu
