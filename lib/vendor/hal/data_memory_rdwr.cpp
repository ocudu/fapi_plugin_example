// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "data_memory_rdwr.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"
#include <cstring>

using namespace ocudu;
using namespace vendor;
using namespace hal;

void data_memory_rdwr::deallocate_buffer(const void* ptr)
{
  pool.deallocate(const_cast<void*>(ptr));
}

data_memory_rdwr::unique_memory_block data_memory_rdwr::read(void* phy_addr)
{
  return unique_memory_block(static_cast<const uint8_t*>(transport_from_phy_address(context, phy_addr)),
                             buffer_deleter{this});
}

uint8_t* data_memory_rdwr::write(span<const uint8_t> data, unsigned tb_idx)
{
  auto& data_msgs = msg_list_manager.get_current_list().data_msgs;
  if (data_msgs.full()) {
    logger.warning("HAL: Maximum number of transport blocks per transaction reached");
    return nullptr;
  }

  void* ptr = pool.allocate();
  if (!ptr) {
    logger.warning("HAL: Unable to allocate memory for '{}' bytes of data", data.size());
    return nullptr;
  }

  // TODO: If the PHY expects a header or an offset in front of the transport block, account for it here.
  std::memcpy(ptr, data.data(), data.size());
  data_msgs.push_back(
      {ptr, unsigned(data.size()), (tb_idx == 0) ? VENDOR_MSG_TYPE_PHY_DATA_BLOCK0 : VENDOR_MSG_TYPE_PHY_DATA_BLOCK1});

  return static_cast<uint8_t*>(transport_to_phy_address(context, ptr));
}
