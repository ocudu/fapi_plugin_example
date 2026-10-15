// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Uplink context repositories.
///
/// The FAPI uplink indications carry information that the PHY usually does not report back, such as the HARQ process
/// of a PUSCH transmission or the number of UCI bits expected in a PUCCH. This information is stored when the
/// UL_TTI.request is translated and recovered when the corresponding indication arrives, using the slot and a per-slot
/// UE index that is sent to the PHY with each PDU and reported back in the indications.
///
/// The repositories are written from the thread sending the requests and read from the thread receiving the
/// indications, so each entry is stored as an atomic integer.

#pragma once

#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/units.h"
#include <array>
#include <atomic>
#include <optional>
#include <vector>

namespace ocudu {
namespace vendor {

/// PUSCH context.
struct pusch_context {
  harq_id_t harq_process_id;

  using packed_type = uint8_t;
  packed_type          pack() const { return static_cast<packed_type>(harq_process_id); }
  static pusch_context unpack(packed_type data) { return {to_harq_id(data)}; }
};

/// PUCCH context.
struct pucch_context {
  units::bits  sr_bitlength;
  units::bits  harq_bitlength;
  units::bits  csi_bitlength;
  pucch_format format;

  using packed_type = uint64_t;
  packed_type pack() const
  {
    return (sr_bitlength.value() & 0xffU) | (static_cast<uint64_t>(harq_bitlength.value() & 0xffffU) << 8U) |
           (static_cast<uint64_t>(csi_bitlength.value() & 0xffffU) << 24U) | (static_cast<uint64_t>(format) << 40U);
  }
  static pucch_context unpack(packed_type data)
  {
    return {units::bits(static_cast<uint8_t>(data)),
            units::bits(static_cast<uint16_t>(data >> 8U)),
            units::bits(static_cast<uint16_t>(data >> 24U)),
            static_cast<pucch_format>(static_cast<uint8_t>(data >> 40U))};
  }
};

/// \brief Uplink context repository.
///
/// \tparam ContextType Context type. It must define a \c packed_type integer type and the \c pack() and \c unpack()
///                     methods.
template <typename ContextType>
class ul_context_repository
{
  using packed_type = typename ContextType::packed_type;

  /// Maximum supported number of UEs per slot.
  static constexpr unsigned MAX_UES_PER_SLOT = 128;

  /// Contexts of a slot.
  using slot_entry = std::array<std::atomic<packed_type>, MAX_UES_PER_SLOT>;

  std::atomic<packed_type>& get_entry(slot_point slot, unsigned ue_id)
  {
    return entries[slot.system_slot() % entries.size()][ue_id];
  }

  std::vector<slot_entry> entries;

public:
  /// Creates a repository that stores the contexts of two frames.
  explicit ul_context_repository(subcarrier_spacing scs) :
    entries(2 * slot_point(to_numerology_value(scs), 0).nof_slots_per_frame())
  {
  }

  /// Stores the context of the given slot and UE index. Returns false if the UE index is out of range.
  bool add(slot_point slot, unsigned ue_id, const ContextType& context)
  {
    if (ue_id >= MAX_UES_PER_SLOT) {
      return false;
    }
    get_entry(slot, ue_id).store(context.pack(), std::memory_order_relaxed);
    return true;
  }

  /// Returns the context of the given slot and UE index, or \c std::nullopt if the UE index is out of range.
  std::optional<ContextType> get(slot_point slot, unsigned ue_id)
  {
    if (ue_id >= MAX_UES_PER_SLOT) {
      return std::nullopt;
    }
    return ContextType::unpack(get_entry(slot, ue_id).load(std::memory_order_relaxed));
  }
};

using pusch_context_repository = ul_context_repository<pusch_context>;
using pucch_context_repository = ul_context_repository<pucch_context>;

} // namespace vendor
} // namespace ocudu
