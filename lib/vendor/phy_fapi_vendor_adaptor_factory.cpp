// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/vendor/phy_fapi_vendor_adaptor_factory.h"
#include "hal/hal.h"
#include "phy_fapi_vendor_sector_adaptor_impl.h"
#include <vector>

using namespace ocudu;
using namespace vendor;

namespace {

/// Vendor PHY-FAPI adaptor implementation, holding one sector adaptor per cell.
class phy_fapi_adaptor_impl : public fapi_adaptor::phy_fapi_adaptor
{
public:
  explicit phy_fapi_adaptor_impl(std::vector<std::unique_ptr<phy_fapi_sector_adaptor_impl>> sectors_) :
    sectors(std::move(sectors_))
  {
  }

  // See interface for documentation.
  fapi_adaptor::phy_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) override
  {
    ocudu_assert(cell_id < sectors.size(), "Invalid cell index '{}'", cell_id);
    return *sectors[cell_id];
  }

private:
  std::vector<std::unique_ptr<phy_fapi_sector_adaptor_impl>> sectors;
};

} // namespace

std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
vendor::create_phy_fapi_vendor_adaptor(span<const vendor_adaptor_cell_config> cells_cfg,
                                       const std::string&                     device_name,
                                       ocudulog::basic_logger&                logger,
                                       task_executor&                         executor)
{
  report_error_if_not(cells_cfg.size() == 1, "The vendor PHY-FAPI adaptor supports a single cell");

  // TODO: To support multiple cells, create one HAL per cell or share it among the sector adaptors.
  std::unique_ptr<hal::hardware_abstraction_layer> hal = hal::create_hal(device_name.c_str(), executor, logger);
  report_error_if_not(hal, "Unable to create the vendor HAL for device '{}'", device_name);

  std::vector<std::unique_ptr<phy_fapi_sector_adaptor_impl>> sectors;
  sectors.push_back(std::make_unique<phy_fapi_sector_adaptor_impl>(cells_cfg.front(), std::move(hal), logger));

  return std::make_unique<phy_fapi_adaptor_impl>(std::move(sectors));
}
