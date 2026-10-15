// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Hardware abstraction layer (HAL) interfaces.
///
/// The HAL moves binary messages between the FAPI translators and the PHY. It is organized in three paths:
///  - TX control path: messages are written into memory blocks obtained from the \ref memory_allocator, queued through
///    the \ref binary_message_gateway, and sent to the PHY in a single transaction delimited by the
///    \ref memory_send_sequencer (one transaction per slot for P7).
///  - Data path: transport blocks are written with the \ref data_memory_writer and read with the
///    \ref data_memory_reader.
///  - RX path: once started through the \ref message_dispatcher_operation_controller, the HAL polls the PHY and
///    forwards every received message to a \ref binary_message_notifier.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include <cstdint>
#include <memory>

namespace ocudu {

class task_executor;

namespace vendor {
namespace hal {

/// Allocates memory blocks to write messages for the PHY.
class memory_allocator
{
public:
  virtual ~memory_allocator() = default;

  /// Allocates a block of memory. Returns a pointer to the memory block on success, otherwise nullptr.
  virtual uint8_t* allocate() = 0;

  /// Returns to the allocator a memory block that will not be sent.
  virtual void deallocate(uint8_t* block) = 0;
};

/// Gateway used for sending binary messages to the PHY.
class binary_message_gateway
{
public:
  virtual ~binary_message_gateway() = default;

  /// \brief Queues a binary message in the current transaction.
  ///
  /// \param[in] msg_id Message type ID.
  /// \param[in] msg    Message contents. It must be stored in a memory block obtained from the \ref memory_allocator.
  virtual void send(unsigned msg_id, span<const uint8_t> msg) = 0;
};

/// Marks the start and end of a transaction, i.e. a group of messages that are sent together to the PHY.
class memory_send_sequencer
{
public:
  virtual ~memory_send_sequencer() = default;

  /// Marks the start of a new transaction. Returns true on success, otherwise false.
  virtual bool mark_send_start() = 0;

  /// Marks the end of the current transaction, sending its messages to the PHY. Returns true on success.
  virtual bool mark_send_end() = 0;
};

/// Writes transport blocks into the data plane memory.
class data_memory_writer
{
public:
  virtual ~data_memory_writer() = default;

  /// \brief Writes a transport block in the current transaction.
  ///
  /// \param[in] data   Data to be written.
  /// \param[in] tb_idx Transport block index (must be either 0 or 1).
  /// \return Address of the data as seen by the PHY on success, nullptr otherwise.
  virtual uint8_t* write(span<const uint8_t> data, unsigned tb_idx) = 0;
};

/// Reads transport blocks written by the PHY in the data plane memory.
class data_memory_reader
{
  /// Deallocates the given memory block.
  virtual void deallocate_buffer(const void* ptr) = 0;

public:
  /// Returns the memory block back to the HAL on destruction.
  struct buffer_deleter {
    data_memory_reader* reader;
    void                operator()(const uint8_t* p) const
    {
      if (p) {
        reader->deallocate_buffer(p);
      }
    }
  };

  /// Memory block that is returned to the HAL when it goes out of scope.
  using unique_memory_block = std::unique_ptr<const uint8_t, buffer_deleter>;

  virtual ~data_memory_reader() = default;

  /// Reads the memory block at the given address, as reported by the PHY.
  virtual unique_memory_block read(void* phy_addr) = 0;
};

/// Notifies the reception of binary messages from the PHY.
class binary_message_notifier
{
public:
  virtual ~binary_message_notifier() = default;

  /// Notifies the reception of a binary message.
  virtual void on_new_message(span<const uint8_t> msg) = 0;
};

/// Controls the reception of messages from the PHY.
class message_dispatcher_operation_controller
{
public:
  virtual ~message_dispatcher_operation_controller() = default;

  /// Starts polling the PHY, forwarding the received messages to the given notifier.
  virtual void start(binary_message_notifier& notifier) = 0;

  /// Stops polling the PHY.
  virtual void stop() = 0;
};

/// Hardware abstraction layer interface.
class hardware_abstraction_layer
{
public:
  virtual ~hardware_abstraction_layer() = default;

  virtual memory_allocator&                        get_memory_allocator()                        = 0;
  virtual binary_message_gateway&                  get_binary_message_gateway()                  = 0;
  virtual memory_send_sequencer&                   get_memory_send_sequencer()                   = 0;
  virtual data_memory_writer&                      get_data_memory_writer()                      = 0;
  virtual data_memory_reader&                      get_data_memory_reader()                      = 0;
  virtual message_dispatcher_operation_controller& get_message_dispatcher_operation_controller() = 0;
};

/// Creates the HAL connected to the given PHY device. Returns nullptr on failure.
std::unique_ptr<hardware_abstraction_layer>
create_hal(const char* devname, task_executor& executor, ocudulog::basic_logger& logger);

} // namespace hal
} // namespace vendor
} // namespace ocudu
