// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

extern "C" {
struct vendor_ul_dci_request;
}

namespace ocudu {

namespace fapi {
struct ul_dci_request;
} // namespace fapi

namespace vendor {

/// \brief Translates the FAPI UL_DCI.request into the vendor UL_DCI.request stored in the output buffer.
///
/// \return Number of written bytes.
unsigned translate_ul_dci_request(::vendor_ul_dci_request& ul_dci_request, const fapi::ul_dci_request& input);

} // namespace vendor
} // namespace ocudu
