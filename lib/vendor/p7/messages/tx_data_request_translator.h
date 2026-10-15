// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"

extern "C" {
struct vendor_tx_data_request;
}

namespace ocudu {

namespace fapi {
struct tx_data_request;
} // namespace fapi

namespace vendor {

namespace hal {
class data_memory_writer;
} // namespace hal

/// \brief Translates the FAPI TX_Data.request into the vendor TX_Data.request stored in the output buffer.
///
/// The transport blocks are copied into the data plane memory with the given data memory writer.
///
/// \return Number of written bytes, or zero on failure.
unsigned translate_tx_data_request(::vendor_tx_data_request&    tx_data_request,
                                   const fapi::tx_data_request& input,
                                   hal::data_memory_writer&     data_mem_writer,
                                   ocudulog::basic_logger&      logger);

} // namespace vendor
} // namespace ocudu
