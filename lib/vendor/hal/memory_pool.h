// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/support/ocudu_assert.h"
#include <memory>

namespace ocudu {
namespace vendor {
namespace hal {

/// Memory pool of fixed size blocks. It is thread-safe to operate with it from different threads.
class memory_pool
{
  /// Block size in bytes.
  // TODO: Adjust the block size to the largest message/transport block exchanged with the PHY.
  static constexpr unsigned block_size = 1u << 18;

  using queue_type =
      concurrent_queue<void*, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  memory_pool(void* base_ptr, size_t size) : freelist(size / block_size)
  {
    ocudu_assert(size > sizeof(void*), "Memory size '{}' is too small", size);

    // Align the base pointer to the block size.
    std::align(block_size, block_size, base_ptr, size);

    // Partition the memory into blocks of block_size bytes.
    unsigned num_blocks = size / block_size;
    auto*    ptr        = reinterpret_cast<uint8_t*>(base_ptr);
    for (unsigned i = 0; i != num_blocks; ++i) {
      (void)freelist.try_push(ptr);
      ptr += block_size;
    }
  }

  /// Allocates a fixed size memory block. Returns nullptr if the pool is empty.
  void* allocate()
  {
    void* ptr = nullptr;
    if (!freelist.try_pop(ptr)) {
      return nullptr;
    }
    return ptr;
  }

  /// Returns back the given pointer to the memory pool.
  /// NOTE: User must make sure that the memory that is given back was originally allocated by this pool, otherwise the
  /// behaviour is undefined.
  void deallocate(void* ptr) { (void)freelist.try_push(ptr); }

private:
  /// Free list of memory blocks.
  queue_type freelist;
};

} // namespace hal
} // namespace vendor
} // namespace ocudu
