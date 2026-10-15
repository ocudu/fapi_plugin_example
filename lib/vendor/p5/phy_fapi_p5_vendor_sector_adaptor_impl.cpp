// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "phy_fapi_p5_vendor_sector_adaptor_impl.h"

using namespace ocudu;
using namespace vendor;

void phy_fapi_p5_sector_adaptor_impl::set_p5_responses_notifier(fapi::p5_responses_notifier& notifier)
{
  msg_receiver.set_p5_responses_notifier(notifier);
  msg_transmitter.set_p5_responses_notifier(notifier);
}

void phy_fapi_p5_sector_adaptor_impl::set_error_indication_notifier(fapi::error_indication_notifier& notifier)
{
  msg_receiver.set_error_indication_notifier(notifier);
}
