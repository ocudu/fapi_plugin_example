// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "data_memory_rdwr.h"
#include "hal.h"
#include "incoming_messages_dispatcher.h"
#include "mac2phy_message_list.h"
#include "memory_pool.h"
#include "message_transmitter_impl.h"
#include "transport.h"

using namespace ocudu;
using namespace vendor;
using namespace hal;

namespace {

/// HAL implementation.
class hardware_abstraction_layer_impl : public hardware_abstraction_layer
{
public:
  hardware_abstraction_layer_impl(const transport_context& context_,
                                  task_executor&           executor,
                                  ocudulog::basic_logger&  logger) :
    context(context_),
    pool(context.mem_base_ptr, context.mem_size),
    msg_list_manager(pool),
    transmitter(pool, msg_list_manager, context, logger),
    data_rdwr(pool, msg_list_manager, context, logger),
    dispatcher(pool, context, executor, logger)
  {
    // Supply the PHY with free memory blocks to write uplink messages.
    // TODO: Some PHYs need to be supplied with free blocks several times during start-up.
    unsigned nof_blocks = dispatcher.provide_rx_blocks();
    logger.debug("HAL: Provided '{}' free blocks to the PHY", nof_blocks);
  }

  ~hardware_abstraction_layer_impl() override { close_transport(context); }

  memory_allocator&                        get_memory_allocator() override { return transmitter; }
  binary_message_gateway&                  get_binary_message_gateway() override { return transmitter; }
  memory_send_sequencer&                   get_memory_send_sequencer() override { return transmitter; }
  data_memory_writer&                      get_data_memory_writer() override { return data_rdwr; }
  data_memory_reader&                      get_data_memory_reader() override { return data_rdwr; }
  message_dispatcher_operation_controller& get_message_dispatcher_operation_controller() override { return dispatcher; }

private:
  const transport_context      context;
  memory_pool                  pool;
  mac2phy_message_list_manager msg_list_manager;
  message_transmitter_impl     transmitter;
  data_memory_rdwr             data_rdwr;
  incoming_messages_dispatcher dispatcher;
};

} // namespace

std::unique_ptr<hardware_abstraction_layer>
hal::create_hal(const char* devname, task_executor& executor, ocudulog::basic_logger& logger)
{
  std::optional<transport_context> context = open_transport(devname, logger);
  if (!context) {
    return nullptr;
  }

  return std::make_unique<hardware_abstraction_layer_impl>(*context, executor, logger);
}
