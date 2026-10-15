// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p5/p5_messages.h"

extern "C" {
struct vendor_config_request;
}

namespace ocudu {
namespace vendor {

/// \brief Translates the contents of the FAPI CONFIG.request into the vendor CONFIG.request stored in the output
/// buffer.
///
/// \return Number of bytes written.
unsigned translate_config_request(::vendor_config_request& config_request, const fapi::config_request& fapi_request);

} // namespace vendor
} // namespace ocudu
