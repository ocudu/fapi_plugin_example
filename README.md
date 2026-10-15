# FAPI Plugin Example

[![build](https://gitlab.com/ocudu/plugin_examples/fapi_plugin_example/badges/main/pipeline.svg?key_text=build)](https://gitlab.com/ocudu/plugin_examples/fapi_plugin_example/-/pipelines?ref=main)

A skeleton OCUDU **split 6** plugin that connects the OCUDU DU-high (MAC) to a third-party PHY/L1 over FAPI.

OCUDU produces and consumes [SCF FAPI](https://scf.io/) P5/P7 messages as C++ structures. Most commercial PHYs use
their own binary L2-L1 API instead, carried over a vendor-specific transport (shared memory, sockets, PCIe, ...). This
plugin sits between them. It translates the OCUDU FAPI structures to and from the PHY's binary messages, and moves
those messages through a small hardware abstraction layer (HAL).

The skeleton compiles and links into the `*_split_6` applications. The PHY-specific parts are stubbed out and marked
with `TODO` comments. Every identifier uses the placeholder name `vendor`, which you can rename with the
[helper script](#renaming-the-plugin).

## Architecture

```
OCUDU DU-high (MAC / scheduler)
   │ ▲   FAPI P5/P7 (C++ structures)
   ▼ │
phy_fapi_vendor_adaptor
└─ phy_fapi_vendor_sector_adaptor (one per cell)
   ├─ P5 sector adaptor
   │  ├─ vendor_p5_requests_transmitter     CONFIG / START / STOP.request    ──► HAL
   │  └─ vendor_p5_responses_receiver       CONFIG / START / STOP.response   ◄── RX handler
   ├─ P7 sector adaptor
   │  ├─ vendor_p7_requests_transmitter     DL_TTI / UL_TTI / UL_DCI / TX_Data ──► HAL
   │  ├─ vendor_p7_indications_receiver     SLOT / RACH / CRC / RX_Data / UCI / ERROR ◄── RX handler
   │  └─ PUSCH / PUCCH context repositories
   └─ vendor_rx_message_handler             routes RX messages to the P5 or P7 receiver
   │ ▲   vendor binary messages
   ▼ │
HAL (interfaces in lib/vendor/hal/hal.h)
├─ message_transmitter_impl                 TX control path: allocator, gateway, per-slot send sequencer
├─ data_memory_rdwr                         data path: transport blocks to/from the PHY
├─ incoming_messages_dispatcher             RX path: polls the PHY on the split 6 executor
└─ transport.cpp                            all PHY specific I/O lives here
   │ ▲   vendor L2-L1 API / transport
   ▼ │
Vendor PHY
```

### Message flow

* **P5 (cell configuration):** `vendor_p5_requests_transmitter` translates each request into a vendor message in a
  memory block and sends it right away. `vendor_p5_responses_receiver` turns the PHY responses back into FAPI
  responses/indications. FAPI has no START.response, so a successful start is signalled by the first SLOT.indication.
* **P7 downlink (per slot):** the MAC sends DL_TTI, UL_TTI, UL_DCI and TX_Data requests for a slot, followed by
  `on_last_message()`.
  * The first request of a slot calls `memory_send_sequencer::mark_send_start()`.
  * Each request is translated into a memory block and queued with `binary_message_gateway::send()`.
  * Transport blocks are copied into data memory through `data_memory_writer::write()`.
  * `on_last_message()` calls `mark_send_end()`, which flushes the whole slot to the PHY through `transport_send()`.
* **P7 uplink:** `incoming_messages_dispatcher` polls `transport_receive()` on the split 6 executor.
  `vendor_rx_message_handler` routes each message to the P5 or the P7 receiver.
* **UL context:** FAPI indications need information that the PHY usually does not report back, such as the HARQ process
  ID or the expected UCI bit lengths. The UL_TTI.request translator stores it in the `pusch_context_repository` and
  `pucch_context_repository`, keyed by slot and per-slot UE index. The indications receiver reads it back from there.
* **Timing:** SLOT.indications are advanced by `L2_TIME_ADVANCE_SLOTS`
  (`lib/vendor/p7/vendor_p7_indications_receiver.cpp`), so that the MAC works ahead of the over-the-air time.

## Directory layout

```
CMakeLists.txt                                     Plugin entry point (PHY SDK lookup goes here)
apps/units/flexible_o_du/split_6/o_du_high/vendor/ Application glue
  vendor_split6_du_application_unit_factory.cpp    Provides ocudu::create_split6_plugin()
  vendor_plugin_impl.{h,cpp}                       split6_plugin implementation (config, validation, adaptor creation)
  vendor_plugin_config.h, vendor_plugin_cli11_schema.{h,cpp}  Plugin configuration and CLI/YAML schema
include/ocudu/vendor/
  phy_fapi_vendor_adaptor_factory.h                Adaptor factory and cell configuration used by the application
  sdk/vendor_l1_api.h                              PLACEHOLDER for the PHY L2-L1 API header
lib/vendor/                                        PHY-FAPI adaptor library (fapi_vendor_adaptor)
  phy_fapi_vendor_*                                Top level and sector adaptors
  vendor_rx_message_handler.h                      RX message router
  hal/                                             HAL interfaces (hal.h), implementation and transport stubs
  p5/                                              P5 adaptor, transmitter, receiver and CONFIG.request translator
  p7/                                              P7 adaptor, transmitter, receiver and UL context repositories
  p7/messages/                                     One translator per FAPI P7 message
scripts/rename_plugin.sh                           Renames the 'vendor' placeholder
```

## Building

OCUDU picks up every directory under `<ocudu>/plugins/` that has a `CMakeLists.txt`. Clone or symlink this repository
there:

```bash
cd <ocudu>
mkdir -p plugins && ln -s /path/to/fapi_plugin_example plugins/fapi_plugin_example
mkdir build && cd build
cmake .. -DBUILD_TESTING=OFF
make -j$(nproc) gnb_split_6      # or odu_split_6
```

The CMake output should contain:

```
-- Adding plugin: plugins/fapi_plugin_example
```

The plugin replaces the dummy split 6 plugin in the `ocudu_split6_plugin` library, so it only affects the
`<app>_split_6` binaries. To build without it, add `-DENABLE_PLUGINS=False` to the CMake command line.

### Continuous integration

`.gitlab-ci.yml` contains a single `build` job. It runs in the OCUDU builder image, clones OCUDU (`OCUDU_REF`,
`main` by default), links this repository into `<ocudu>/plugins/`, and builds the targets listed in `BUILD_TARGETS`:
the plugin libraries and `odu_split_6`, to check that the plugin links. Compiler results are cached with ccache.

## Configuration

The plugin adds a `vendor` section and a `log.vendor_level` option to the application configuration:

```yaml
vendor:
  device_name: "phy0"        # Identifier of the device/channel used to reach the PHY (mandatory).

log:
  vendor_level: info         # Log level of the plugin ("VENDOR" logger).
```

Extend `vendor_plugin_config.h` and `vendor_plugin_cli11_schema.cpp` with any other parameter your PHY needs.

## Porting checklist

Look for `TODO` comments (`grep -rn TODO apps include lib`). The main steps are:

1. **PHY SDK.** Replace `include/ocudu/vendor/sdk/vendor_l1_api.h` with your PHY's API header. Find the SDK in the
   top-level `CMakeLists.txt` and link it in `lib/vendor/CMakeLists.txt`.
2. **Transport.** Implement `lib/vendor/hal/transport.cpp`: open/close, send (with batch position), receive,
   hand free RX blocks to the PHY, and address translation. The placeholder only allocates local memory and never
   receives anything, so the DU starts but no SLOT.indication ever arrives.
3. **HAL tuning.** In `lib/vendor/hal/`, review `memory_pool.h` (block size), `mac2phy_message_list.h` (lifetime of
   sent blocks, messages per transaction), and `message_transmitter_impl.cpp` / `data_memory_rdwr.cpp` (per-message
   headers or offsets, message ordering within a slot).
4. **P5 translators.** Fill the vendor CONFIG.request in `p5/config_request_translator.cpp` and complete START/STOP in
   `p5/vendor_p5_requests_transmitter.cpp`. Decide how to handle PARAM.request.
5. **P7 request translators.** Complete the translators in `p7/messages/` (DL_TTI with SSB, PDCCH, PDSCH and CSI-RS;
   UL_TTI with PRACH, PUSCH and PUCCH; UL_DCI; TX_Data).
6. **P7 indication translators.** Complete the CRC, RX_Data, UCI and RACH translators, including measurement unit
   conversions, and adapt how indications are matched to the stored UL contexts (`ue_id`).
7. **Plugin glue.** Tighten `vendor_plugin_impl::is_ran_config_supported()` to reject configurations your PHY cannot
   handle, and initialize any process-wide resources (e.g. a DPDK EAL) in `create_fapi_adaptor()`.
8. **Multi-cell.** The factory creates a single HAL owned by the only sector adaptor. Create one HAL per cell, or
   share it, if you need more.

## Known limitation in OCUDU

At the time of writing, the split 6 O-DU high (`apps/units/flexible_o_du/split_6/o_du_high/split6_o_du_impl.cpp`)
connects the P7 notifiers to the plugin but not the P5 ones (`// TODO: connect P5`). Until that is fixed in OCUDU,
the first PARAM.request stops the application with `P5 responses notifier not set`.

## Renaming the plugin

```bash
scripts/rename_plugin.sh acme
```

The script renames the files, directories, namespaces, CMake targets, macros and the configuration section from
`vendor` to `acme` (`Vendor` → `Acme`, `VENDOR` → `ACME`). It also changes the word in comments, so review those
afterwards.

## License

BSD-3-Clause-Open-MPI. See [LICENSE](LICENSE) and [COPYRIGHTS.txt](COPYRIGHTS.txt).
