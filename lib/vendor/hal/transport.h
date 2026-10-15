// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Low level transport between the L2 and the PHY.
///
/// All the PHY specific I/O primitives used by the HAL are declared here, so that the rest of the HAL is independent
/// of the mechanism used to exchange messages with the PHY (shared memory, sockets, PCIe, etc).
///
/// TODO: Implement these functions in transport.cpp using the API provided by your PHY.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <cstdint>
#include <optional>

namespace ocudu {
namespace vendor {
namespace hal {

/// Transport context information.
struct transport_context {
  /// Transport instance handle.
  void* handle;
  /// Base pointer of the memory region shared with the PHY.
  void* mem_base_ptr;
  /// Memory size available for the L2 to manage.
  uint64_t mem_size;
};

/// Position of a message within the transaction it belongs to.
enum class transport_batch_position { single, first, middle, last };

/// Message received from the PHY.
struct transport_rx_message {
  /// Memory block holding the message. It is returned to the memory pool once the message has been processed.
  void* block;
  /// Pointer to the start of the message.
  const uint8_t* data;
  /// Message size in bytes.
  unsigned size;
};

/// Opens the transport, allocating all the memory needed by the L2. Returns \c std::nullopt on failure.
std::optional<transport_context> open_transport(const char* devname, ocudulog::basic_logger& logger);

/// Releases all the resources of the given transport.
void close_transport(const transport_context& ctx);

/// \brief Sends a single message to the PHY.
///
/// \param[in] ctx      Transport context.
/// \param[in] addr     Address of the message (L2 address space).
/// \param[in] size     Message size in bytes.
/// \param[in] msg_type Message type identifier.
/// \param[in] position Position of the message in the current transaction.
/// \return True on success, otherwise false.
bool transport_send(const transport_context& ctx,
                    void*                    addr,
                    unsigned                 size,
                    unsigned                 msg_type,
                    transport_batch_position position);

/// Polls the PHY for a new message. Returns \c std::nullopt if no message is available.
std::optional<transport_rx_message> transport_receive(const transport_context& ctx);

/// Hands a free memory block to the PHY, to write uplink messages and data. Returns false if the PHY rejects it.
bool transport_provide_rx_block(const transport_context& ctx, void* block);

/// Converts an L2 address into the address space of the PHY.
void* transport_to_phy_address(const transport_context& ctx, void* l2_addr);

/// Converts a PHY address into the L2 address space.
void* transport_from_phy_address(const transport_context& ctx, void* phy_addr);

} // namespace hal
} // namespace vendor
} // namespace ocudu
