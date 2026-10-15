// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "p7/ul_context_repository.h"
#include "ocudu/ocudulog/logger.h"

extern "C" {
struct vendor_ul_tti_request;
}

namespace ocudu {

namespace fapi {
struct ul_tti_request;
} // namespace fapi

namespace vendor {

/// \brief Translates the FAPI UL_TTI.request into the vendor UL_TTI.request stored in the output buffer.
///
/// The contexts needed to build the uplink indications are stored in the given repositories.
///
/// \return Number of written bytes.
unsigned translate_ul_tti_request(::vendor_ul_tti_request&    ul_tti_request,
                                  const fapi::ul_tti_request& input,
                                  pusch_context_repository&   pusch_repo,
                                  pucch_context_repository&   pucch_repo,
                                  ocudulog::basic_logger&     logger);

} // namespace vendor
} // namespace ocudu
