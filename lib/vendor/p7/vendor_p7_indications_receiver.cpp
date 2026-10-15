// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vendor_p7_indications_receiver.h"
#include "hal/hal.h"
#include "messages/crc_indication_translator.h"
#include "messages/rach_indication_translator.h"
#include "messages/rx_data_indication_translator.h"
#include "messages/uci_indication_translator.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/fapi/p7/messages/slot_indication.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

/// \brief Number of slots the L2 runs ahead of the PHY.
///
/// The requests of a slot must be sent ahead of the over-the-air time, so that the PHY has time to process them. The
/// slot reported in the SLOT.indication is advanced accordingly.
// TODO: Set the time advance required by your PHY.
static constexpr unsigned L2_TIME_ADVANCE_SLOTS = 2;

/// Converts the given vendor SFN and slot into a slot point.
static slot_point to_slot_point(const ::vendor_sfn_slot& sfn_slot, subcarrier_spacing scs)
{
  return {scs, sfn_slot.sfn, sfn_slot.slot};
}

void vendor_p7_indications_receiver::handle_new_message(span<const uint8_t> msg)
{
  report_error_if_not(slot_notifier && p7_notifier && error_notifier, "P7 notifiers not set");

  const auto& msg_header = *reinterpret_cast<const ::vendor_msg_header*>(msg.data());

  switch (msg_header.msg_type) {
    case VENDOR_MSG_TYPE_PHY_SLOT_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_slot_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs) + L2_TIME_ADVANCE_SLOTS;

      slot_notifier->on_slot_indication(
          {.slot = slot_point_extended(slot), .time_point = std::chrono::system_clock::now()});
      break;
    }
    case VENDOR_MSG_TYPE_PHY_ERR_IND: {
      const auto& ind = *reinterpret_cast<const ::vendor_error_indication*>(msg.data());
      logger.debug("ERROR.indication received with status {}", ind.status);

      // TODO: Map the error codes and message identifiers of your PHY.
      fapi::error_indication error_msg;
      error_msg.slot       = to_slot_point(ind.sfn_slot, scs);
      error_msg.error_code = static_cast<fapi::error_code_id>(ind.status);
      error_notifier->on_error_indication(error_msg);
      break;
    }
    case VENDOR_MSG_TYPE_PHY_RACH_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_rach_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs);

      for (unsigned i = 0; i != ind.nof_preambles; ++i) {
        fapi::rach_indication fapi_ind;
        translate_rach_indication(fapi_ind, slot, ind.preambles[i]);
        p7_notifier->on_rach_indication(fapi_ind);
      }
      break;
    }
    case VENDOR_MSG_TYPE_PHY_CRC_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_crc_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs);

      for (unsigned i = 0; i != ind.nof_crcs; ++i) {
        std::optional<pusch_context> context = pusch_repo.get(slot, ind.crcs[i].ue_id);
        if (!context) {
          logger.warning("Dropping CRC with invalid UE index '{}'", ind.crcs[i].ue_id);
          continue;
        }

        fapi::crc_indication fapi_ind;
        translate_crc_indication(fapi_ind, slot, ind.crcs[i], *context);
        p7_notifier->on_crc_indication(fapi_ind);
      }
      break;
    }
    case VENDOR_MSG_TYPE_PHY_RX_DATA_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_rx_data_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs);

      for (unsigned i = 0; i != ind.nof_pdus; ++i) {
        const auto&                  pdu     = ind.pdus[i];
        std::optional<pusch_context> context = pusch_repo.get(slot, pdu.ue_id);
        if (!context) {
          logger.warning("Dropping RX_Data with invalid UE index '{}'", pdu.ue_id);
          continue;
        }

        // The buffer is returned to the HAL when it goes out of scope, after the indication has been notified.
        hal::data_memory_reader::unique_memory_block buffer = data_mem_reader.read(pdu.payload);

        fapi::rx_data_indication fapi_ind;
        translate_rx_data_indication(fapi_ind, slot, pdu, *context, buffer);
        p7_notifier->on_rx_data_indication(fapi_ind);
      }
      break;
    }
    case VENDOR_MSG_TYPE_PHY_UCI_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_uci_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs);

      for (unsigned i = 0; i != ind.nof_ucis; ++i) {
        std::optional<pucch_context> context = pucch_repo.get(slot, ind.ucis[i].ue_id);
        if (!context) {
          logger.warning("Dropping UCI with invalid UE index '{}'", ind.ucis[i].ue_id);
          continue;
        }

        fapi::uci_indication fapi_ind;
        translate_uci_indication(fapi_ind, slot, ind.ucis[i], *context);
        p7_notifier->on_uci_indication(fapi_ind);
      }
      break;
    }
    case VENDOR_MSG_TYPE_PHY_PUSCH_UCI_IND: {
      const auto& ind  = *reinterpret_cast<const ::vendor_pusch_uci_indication*>(msg.data());
      slot_point  slot = to_slot_point(ind.sfn_slot, scs);

      for (unsigned i = 0; i != ind.nof_ucis; ++i) {
        fapi::uci_indication fapi_ind;
        translate_uci_indication(fapi_ind, slot, ind.ucis[i]);
        p7_notifier->on_uci_indication(fapi_ind);
      }
      break;
    }
    default:
      report_error("Invalid message id '{}'. This receiver only handles P7 messages", msg_header.msg_type);
  }
}
