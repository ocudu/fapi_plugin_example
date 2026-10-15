// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "rx_data_indication_translator.h"
#include "ocudu/fapi/p7/builders/rx_data_indication_builder.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

void vendor::translate_rx_data_indication(fapi::rx_data_indication&                           indication,
                                          slot_point                                          slot,
                                          const ::vendor_rx_data_pdu&                         pdu,
                                          const pusch_context&                                context,
                                          const hal::data_memory_reader::unique_memory_block& buffer)
{
  fapi::rx_data_indication_builder builder(indication);
  builder.set_slot(slot);
  builder.set_pdu(to_rnti(pdu.rnti), context.harq_process_id, {buffer.get(), pdu.pdu_length});
}
