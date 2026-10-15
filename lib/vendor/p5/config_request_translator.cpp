// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "config_request_translator.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/vendor/sdk/vendor_l1_api.h"

using namespace ocudu;
using namespace vendor;

unsigned vendor::translate_config_request(::vendor_config_request&    config_request,
                                          const fapi::config_request& fapi_request)
{
  const fapi::cell_configuration& cell = fapi_request.cell_cfg;
  nr_band                         band = band_helper::get_band_from_dl_arfcn(cell.carrier_cfg.dl_f_ref_arfcn);

  unsigned nof_prbs = band_helper::get_n_rbs_from_bw(
      MHz_to_bs_channel_bandwidth(cell.carrier_cfg.dl_bandwidth), cell.scs_common, band_helper::get_freq_range(band));

  config_request.carrier_idx = 0;
  config_request.phy_cell_id = cell.pci;
  config_request.dl_abs_freq_point_a_khz =
      band_helper::get_abs_freq_point_a_from_f_ref(
          band_helper::nr_arfcn_to_freq(cell.carrier_cfg.dl_f_ref_arfcn), nof_prbs, cell.scs_common) *
      band_helper::HZ_TO_KHZ;
  config_request.ul_abs_freq_point_a_khz =
      band_helper::get_abs_freq_point_a_from_f_ref(
          band_helper::nr_arfcn_to_freq(cell.carrier_cfg.ul_f_ref_arfcn), nof_prbs, cell.scs_common) *
      band_helper::HZ_TO_KHZ;
  config_request.dl_bandwidth_mhz = cell.carrier_cfg.dl_bandwidth;
  config_request.ul_bandwidth_mhz = cell.carrier_cfg.ul_bandwidth;
  config_request.numerology       = to_numerology_value(cell.scs_common);
  config_request.duplex_mode      = static_cast<uint8_t>(cell.duplex);
  config_request.nof_tx_antennas  = cell.carrier_cfg.num_tx_ant;
  config_request.nof_rx_antennas  = cell.carrier_cfg.num_rx_ant;

  // TODO: Translate the rest of the cell configuration required by the PHY. The FAPI cell configuration contains,
  // among others:
  //  - SSB configuration:   cell.ssb_cfg (periodicity, k_SSB, offset to point A, etc).
  //  - PRACH configuration: cell.prach_cfg (configuration index, root sequence, zero correlation zone, etc).
  //  - TDD configuration:   cell.tdd_ul_dl_cfg_common (present when cell.duplex is TDD).
  //  - Cyclic prefix:       cell.cp.

  config_request.hdr.msg_type = VENDOR_MSG_TYPE_PHY_CONFIG_REQ;
  config_request.hdr.msg_len  = sizeof(::vendor_config_request);

  return config_request.hdr.msg_len;
}
