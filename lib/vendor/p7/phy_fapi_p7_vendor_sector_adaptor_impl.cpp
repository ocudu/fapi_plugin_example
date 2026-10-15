// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "phy_fapi_p7_vendor_sector_adaptor_impl.h"
#include "hal/hal.h"

using namespace ocudu;
using namespace vendor;

phy_fapi_p7_sector_adaptor_impl::phy_fapi_p7_sector_adaptor_impl(subcarrier_spacing               scs,
                                                                 hal::hardware_abstraction_layer& hal,
                                                                 ocudulog::basic_logger&          logger) :
  pusch_repo(scs),
  pucch_repo(scs),
  msg_receiver(scs, hal.get_data_memory_reader(), pusch_repo, pucch_repo, logger),
  msg_transmitter(hal, pusch_repo, pucch_repo, logger)
{
}
