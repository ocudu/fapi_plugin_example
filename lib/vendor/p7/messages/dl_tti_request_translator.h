// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"

extern "C" {
struct vendor_dl_tti_request;
struct vendor_pdcch_pdu;
}

namespace ocudu {

namespace fapi {
struct dl_pdcch_pdu;
struct dl_tti_request;
} // namespace fapi

namespace vendor {

/// \brief Translates the FAPI DL_TTI.request into the vendor DL_TTI.request stored in the output buffer.
///
/// \return Number of written bytes.
unsigned translate_dl_tti_request(::vendor_dl_tti_request&    dl_tti_request,
                                  const fapi::dl_tti_request& input,
                                  ocudulog::basic_logger&     logger);

/// Translates the FAPI PDCCH PDU into the vendor PDCCH PDU. Used by both DL_TTI.request and UL_DCI.request.
void translate_pdcch_pdu(::vendor_pdcch_pdu& pdcch_pdu, const fapi::dl_pdcch_pdu& fapi_pdu);

} // namespace vendor
} // namespace ocudu
