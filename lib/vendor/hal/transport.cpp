// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "transport.h"
#include <cstdlib>

using namespace ocudu;
using namespace vendor;
using namespace hal;

/// Size of the placeholder memory region allocated by the stub implementation.
static constexpr uint64_t PLACEHOLDER_MEMORY_SIZE = 64U * 1024U * 1024U;

std::optional<transport_context> hal::open_transport(const char* devname, ocudulog::basic_logger& logger)
{
  // TODO: Open the communication channel with the PHY identified by 'devname' and obtain the memory region shared
  // with it. The placeholder below allocates a local memory region so that the skeleton can be started.
  logger.warning("HAL: Using the placeholder transport for device '{}'. No messages will reach the PHY", devname);

  void* mem_base_ptr = std::malloc(PLACEHOLDER_MEMORY_SIZE);
  if (!mem_base_ptr) {
    logger.error("HAL: Cannot allocate transport memory");
    return std::nullopt;
  }

  return transport_context{.handle = nullptr, .mem_base_ptr = mem_base_ptr, .mem_size = PLACEHOLDER_MEMORY_SIZE};
}

void hal::close_transport(const transport_context& ctx)
{
  // TODO: Release the resources acquired in open_transport().
  std::free(ctx.mem_base_ptr);
}

bool hal::transport_send(const transport_context& ctx,
                         void*                    addr,
                         unsigned                 size,
                         unsigned                 msg_type,
                         transport_batch_position position)
{
  // TODO: Push the message to the PHY. Use the batch position if the PHY requires the messages of a slot to be grouped
  // in a single transaction (scatter-gather).
  return true;
}

std::optional<transport_rx_message> hal::transport_receive(const transport_context& ctx)
{
  // TODO: Poll the PHY for new messages. Return the memory block holding the message, so that it can be returned to the
  // memory pool after processing, and the location of the message within the block.
  return std::nullopt;
}

bool hal::transport_provide_rx_block(const transport_context& ctx, void* block)
{
  // TODO: Give the memory block to the PHY, so it can write uplink messages and data into it. Return false when the PHY
  // does not accept more blocks.
  return false;
}

void* hal::transport_to_phy_address(const transport_context& ctx, void* l2_addr)
{
  // TODO: Translate the address if the PHY uses a different address space (e.g. physical addresses).
  return l2_addr;
}

void* hal::transport_from_phy_address(const transport_context& ctx, void* phy_addr)
{
  // TODO: Translate the address if the PHY uses a different address space (e.g. physical addresses).
  return phy_addr;
}
