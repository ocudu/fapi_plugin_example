// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/units/flexible_o_du/split_6/o_du_high/split6_plugin.h"
#include "vendor_plugin_impl.h"

using namespace ocudu;

std::unique_ptr<split6_plugin> ocudu::create_split6_plugin(std::string_view app_name)
{
  return std::make_unique<vendor::vendor_plugin_impl>();
}
