// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Placeholder L2-L1 binary API.
///
/// This header stands in for the message definitions provided by the PHY SDK. It only contains the minimum
/// set of fields required for the skeleton to compile and to illustrate the data flow between the FAPI translators and
/// the HAL.
///
/// TODO: Remove this file and include the API header shipped with your PHY instead. Then update the translators in
/// lib/vendor/p5/config_request_translator.cpp and lib/vendor/p7/messages to fill/parse the real structures.

#pragma once

#include <cstdint>

extern "C" {

/// Message type identifiers.
enum vendor_msg_type : uint16_t {
  // P5 messages.
  VENDOR_MSG_TYPE_PHY_CONFIG_REQ  = 0x01,
  VENDOR_MSG_TYPE_PHY_CONFIG_RESP = 0x02,
  VENDOR_MSG_TYPE_PHY_START_REQ   = 0x03,
  VENDOR_MSG_TYPE_PHY_START_RESP  = 0x04,
  VENDOR_MSG_TYPE_PHY_STOP_REQ    = 0x05,
  VENDOR_MSG_TYPE_PHY_STOP_RESP   = 0x06,
  VENDOR_MSG_TYPE_PHY_ERR_IND     = 0x07,
  // P7 requests.
  VENDOR_MSG_TYPE_PHY_DL_TTI_REQ  = 0x10,
  VENDOR_MSG_TYPE_PHY_UL_TTI_REQ  = 0x11,
  VENDOR_MSG_TYPE_PHY_UL_DCI_REQ  = 0x12,
  VENDOR_MSG_TYPE_PHY_TX_DATA_REQ = 0x13,
  // P7 indications.
  VENDOR_MSG_TYPE_PHY_SLOT_IND      = 0x20,
  VENDOR_MSG_TYPE_PHY_RACH_IND      = 0x21,
  VENDOR_MSG_TYPE_PHY_CRC_IND       = 0x22,
  VENDOR_MSG_TYPE_PHY_RX_DATA_IND   = 0x23,
  VENDOR_MSG_TYPE_PHY_UCI_IND       = 0x24,
  VENDOR_MSG_TYPE_PHY_PUSCH_UCI_IND = 0x25,
  // Data plane messages (transport blocks).
  VENDOR_MSG_TYPE_PHY_DATA_BLOCK0 = 0x30,
  VENDOR_MSG_TYPE_PHY_DATA_BLOCK1 = 0x31,
};

/// PDU type identifiers.
enum vendor_pdu_type : uint16_t {
  VENDOR_DL_PDU_TYPE_SSB    = 0,
  VENDOR_DL_PDU_TYPE_PDCCH  = 1,
  VENDOR_DL_PDU_TYPE_PDSCH  = 2,
  VENDOR_DL_PDU_TYPE_CSI_RS = 3,
  VENDOR_UL_PDU_TYPE_PRACH  = 10,
  VENDOR_UL_PDU_TYPE_PUSCH  = 11,
  VENDOR_UL_PDU_TYPE_PUCCH  = 12,
};

/// Maximum number of PDUs per message.
#define VENDOR_MAX_PDUS_PER_MSG 64
/// Maximum number of UCI/CRC/RX_Data entries per indication.
#define VENDOR_MAX_ENTRIES_PER_IND 32
/// Maximum number of PRACH preambles per indication.
#define VENDOR_MAX_PREAMBLES_PER_IND 64

/// Common message header.
struct vendor_msg_header {
  /// Message type, see \ref vendor_msg_type.
  uint16_t msg_type;
  uint16_t reserved;
  /// Message length in bytes, including this header.
  uint32_t msg_len;
};

/// Common PDU header.
struct vendor_pdu_header {
  /// PDU type, see \ref vendor_pdu_type.
  uint16_t pdu_type;
  /// PDU size in bytes, including this header.
  uint16_t pdu_size;
};

/// System frame number and slot.
struct vendor_sfn_slot {
  uint16_t sfn;
  uint16_t slot;
  uint16_t carrier_idx;
};

// ---------------------------------------------------------------------------------------------------------------------
// P5 messages.
// ---------------------------------------------------------------------------------------------------------------------

struct vendor_config_request {
  vendor_msg_header hdr;
  uint16_t          carrier_idx;
  uint16_t          phy_cell_id;
  uint32_t          dl_abs_freq_point_a_khz;
  uint32_t          ul_abs_freq_point_a_khz;
  uint16_t          dl_bandwidth_mhz;
  uint16_t          ul_bandwidth_mhz;
  uint8_t           numerology;
  uint8_t           duplex_mode;
  uint8_t           nof_tx_antennas;
  uint8_t           nof_rx_antennas;
  // TODO: Add the remaining cell, SSB, PRACH and TDD configuration parameters required by your PHY.
};

struct vendor_config_response {
  vendor_msg_header hdr;
  /// Zero on success.
  uint32_t status;
};

struct vendor_start_request {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
};

struct vendor_start_response {
  vendor_msg_header hdr;
  /// Zero on success.
  uint32_t status;
};

struct vendor_stop_request {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
};

struct vendor_stop_response {
  vendor_msg_header hdr;
  /// Zero on success.
  uint32_t status;
};

struct vendor_error_indication {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
  uint16_t          msg_id;
  uint32_t          status;
};

// ---------------------------------------------------------------------------------------------------------------------
// P7 requests.
// ---------------------------------------------------------------------------------------------------------------------

struct vendor_ssb_pdu {
  vendor_pdu_header hdr;
  uint16_t          phy_cell_id;
  uint8_t           ssb_subcarrier_offset;
  uint16_t          ssb_offset_point_a;
  uint8_t           ssb_block_index;
  uint32_t          bch_payload;
};

struct vendor_pdcch_pdu {
  vendor_pdu_header hdr;
  uint16_t          rnti;
  uint16_t          bwp_start;
  uint16_t          bwp_size;
  uint8_t           start_symbol;
  uint8_t           duration_symbols;
  uint8_t           aggregation_level;
  uint16_t          cce_index;
  uint16_t          payload_size_bits;
  uint8_t           payload[16];
};

struct vendor_pdsch_pdu {
  vendor_pdu_header hdr;
  uint16_t          rnti;
  uint16_t          ue_id;
  uint16_t          bwp_start;
  uint16_t          bwp_size;
  uint16_t          rb_start;
  uint16_t          rb_size;
  uint8_t           start_symbol;
  uint8_t           nof_symbols;
  uint8_t           nof_layers;
  uint8_t           mcs_index;
  uint8_t           rv_index;
  uint32_t          tb_size;
};

struct vendor_csi_rs_pdu {
  vendor_pdu_header hdr;
  uint16_t          bwp_start;
  uint16_t          bwp_size;
  uint16_t          start_rb;
  uint16_t          nof_rbs;
  uint8_t           row;
  uint8_t           symbol_l0;
  uint16_t          scrambling_id;
};

/// DL_TTI.request. The DL PDUs are appended right after this structure.
struct vendor_dl_tti_request {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
  uint16_t          nof_pdus;
  uint16_t          nof_dcis;
};

/// UL_DCI.request.
struct vendor_ul_dci_request {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
  uint16_t          nof_dcis;
  vendor_pdcch_pdu  dcis[VENDOR_MAX_PDUS_PER_MSG];
};

struct vendor_prach_pdu {
  vendor_pdu_header hdr;
  uint16_t          phy_cell_id;
  uint8_t           nof_prach_occasions;
  uint8_t           prach_format;
  uint8_t           start_symbol;
};

struct vendor_pusch_pdu {
  vendor_pdu_header hdr;
  uint16_t          rnti;
  uint16_t          ue_id;
  uint16_t          bwp_start;
  uint16_t          bwp_size;
  uint16_t          rb_start;
  uint16_t          rb_size;
  uint8_t           start_symbol;
  uint8_t           nof_symbols;
  uint8_t           nof_layers;
  uint8_t           mcs_index;
  uint8_t           rv_index;
  uint8_t           harq_id;
  uint8_t           new_data;
  uint32_t          tb_size;
};

struct vendor_pucch_pdu {
  vendor_pdu_header hdr;
  uint16_t          rnti;
  uint16_t          ue_id;
  uint8_t           format;
  uint16_t          prb_start;
  uint16_t          prb_size;
  uint8_t           start_symbol;
  uint8_t           nof_symbols;
  uint8_t           sr_present;
  uint16_t          harq_bit_len;
  uint16_t          csi_part1_bit_len;
};

/// UL_TTI.request. The UL PDUs are appended right after this structure.
struct vendor_ul_tti_request {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
  uint16_t          nof_pdus;
  uint8_t           rach_present;
  uint8_t           nof_pusch;
  uint8_t           nof_pucch;
};

struct vendor_tx_data_pdu {
  uint16_t pdu_index;
  uint32_t pdu_length;
  /// Address of the payload, as seen by the PHY.
  uint8_t* payload;
};

/// TX_Data.request.
struct vendor_tx_data_request {
  vendor_msg_header  hdr;
  vendor_sfn_slot    sfn_slot;
  uint16_t           nof_pdus;
  vendor_tx_data_pdu pdus[VENDOR_MAX_PDUS_PER_MSG];
};

// ---------------------------------------------------------------------------------------------------------------------
// P7 indications.
// ---------------------------------------------------------------------------------------------------------------------

struct vendor_slot_indication {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
};

struct vendor_rach_preamble {
  uint8_t  preamble_index;
  uint16_t timing_advance;
  /// Preamble power in 0.01 dBm steps.
  int16_t preamble_power;
  uint8_t slot_index;
  uint8_t symbol_index;
  uint8_t fd_ra_index;
};

struct vendor_rach_indication {
  vendor_msg_header    hdr;
  vendor_sfn_slot      sfn_slot;
  uint16_t             nof_preambles;
  vendor_rach_preamble preambles[VENDOR_MAX_PREAMBLES_PER_IND];
};

struct vendor_crc_pdu {
  uint16_t ue_id;
  uint16_t rnti;
  uint8_t  crc_ok;
  /// SINR in 0.01 dB steps.
  int16_t sinr;
  /// Timing advance as per TS38.213 Section 4.2.
  uint16_t timing_advance;
};

struct vendor_crc_indication {
  vendor_msg_header hdr;
  vendor_sfn_slot   sfn_slot;
  uint16_t          nof_crcs;
  vendor_crc_pdu    crcs[VENDOR_MAX_ENTRIES_PER_IND];
};

struct vendor_rx_data_pdu {
  uint16_t ue_id;
  uint16_t rnti;
  uint32_t pdu_length;
  /// Address of the payload, as seen by the PHY.
  uint8_t* payload;
};

struct vendor_rx_data_indication {
  vendor_msg_header  hdr;
  vendor_sfn_slot    sfn_slot;
  uint16_t           nof_pdus;
  vendor_rx_data_pdu pdus[VENDOR_MAX_ENTRIES_PER_IND];
};

struct vendor_pucch_uci_pdu {
  uint16_t ue_id;
  uint16_t rnti;
  uint8_t  sr_detected;
  uint8_t  nof_harq_bits;
  uint8_t  harq_bits[16];
  uint16_t nof_csi_part1_bits;
  uint8_t  csi_part1_payload[64];
};

struct vendor_uci_indication {
  vendor_msg_header    hdr;
  vendor_sfn_slot      sfn_slot;
  uint16_t             nof_ucis;
  vendor_pucch_uci_pdu ucis[VENDOR_MAX_ENTRIES_PER_IND];
};

struct vendor_pusch_uci_pdu {
  uint16_t ue_id;
  uint16_t rnti;
  uint8_t  nof_harq_bits;
  uint8_t  harq_bits[16];
  uint16_t nof_csi_part1_bits;
  uint8_t  csi_part1_payload[64];
};

struct vendor_pusch_uci_indication {
  vendor_msg_header    hdr;
  vendor_sfn_slot      sfn_slot;
  uint16_t             nof_ucis;
  vendor_pusch_uci_pdu ucis[VENDOR_MAX_ENTRIES_PER_IND];
};

} // extern "C"
