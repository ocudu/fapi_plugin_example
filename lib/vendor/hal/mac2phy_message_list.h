// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "memory_pool.h"
#include "ocudu/adt/static_vector.h"
#include <array>

namespace ocudu {
namespace vendor {
namespace hal {

/// Message stored in a memory block, ready to be sent to the PHY.
struct memory_block_context {
  /// Memory block address.
  void* addr;
  /// Message size in bytes.
  unsigned size;
  /// Message type.
  unsigned msg_type;
};

/// Maximum number of FAPI messages that can be transmitted in a single transaction.
constexpr unsigned MAX_NUM_MAC2PHY_FAPI_MESSAGES = 8;
/// Maximum number of data (transport block) messages that can be transmitted in a single transaction.
constexpr unsigned MAX_NUM_MAC2PHY_DATA_MESSAGES = 32;

/// Messages of a single transaction.
struct mac2phy_message_list {
  static_vector<memory_block_context, MAX_NUM_MAC2PHY_FAPI_MESSAGES> fapi_msgs;
  static_vector<memory_block_context, MAX_NUM_MAC2PHY_DATA_MESSAGES> data_msgs;
};

/// \brief Manages the lifetime of the messages sent to the PHY.
///
/// The memory blocks of a transaction are kept for \c NOF_LISTS transactions, as the PHY may still be reading them
/// after they have been sent. Calling \c recycle_message_list returns the blocks of the oldest transaction to the
/// memory pool and makes its list the current one.
// TODO: Adjust NOF_LISTS to the time the PHY needs to consume the messages.
class mac2phy_message_list_manager
{
  static constexpr unsigned NOF_LISTS = 4;

  /// Returns the memory blocks of the given list to the memory pool.
  void release(mac2phy_message_list& list)
  {
    for (const auto& msg : list.fapi_msgs) {
      pool.deallocate(msg.addr);
    }
    for (const auto& msg : list.data_msgs) {
      pool.deallocate(msg.addr);
    }
    list.fapi_msgs.clear();
    list.data_msgs.clear();
  }

public:
  explicit mac2phy_message_list_manager(memory_pool& pool_) : pool(pool_) {}

  ~mac2phy_message_list_manager()
  {
    for (auto& list : lists) {
      release(list);
    }
  }

  /// Starts a new transaction, recycling the oldest message list.
  void recycle_message_list()
  {
    index = (index + 1) % NOF_LISTS;
    release(lists[index]);
  }

  /// Returns the message list of the current transaction.
  mac2phy_message_list& get_current_list() { return lists[index]; }

private:
  memory_pool&                                pool;
  unsigned                                    index = 0;
  std::array<mac2phy_message_list, NOF_LISTS> lists;
};

} // namespace hal
} // namespace vendor
} // namespace ocudu
