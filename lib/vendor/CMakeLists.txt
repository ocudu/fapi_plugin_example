# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set(SOURCES
        phy_fapi_vendor_adaptor_factory.cpp
        phy_fapi_vendor_sector_adaptor_impl.cpp
        hal/data_memory_rdwr.cpp
        hal/hal_impl.cpp
        hal/incoming_messages_dispatcher.cpp
        hal/message_transmitter_impl.cpp
        hal/transport.cpp
        p5/config_request_translator.cpp
        p5/phy_fapi_p5_vendor_sector_adaptor_impl.cpp
        p5/vendor_p5_requests_transmitter.cpp
        p5/vendor_p5_responses_receiver.cpp
        p7/messages/crc_indication_translator.cpp
        p7/messages/dl_tti_request_translator.cpp
        p7/messages/rach_indication_translator.cpp
        p7/messages/rx_data_indication_translator.cpp
        p7/messages/tx_data_request_translator.cpp
        p7/messages/uci_indication_translator.cpp
        p7/messages/ul_dci_request_translator.cpp
        p7/messages/ul_tti_request_translator.cpp
        p7/phy_fapi_p7_vendor_sector_adaptor_impl.cpp
        p7/vendor_p7_indications_receiver.cpp
        p7/vendor_p7_requests_transmitter.cpp)

add_library(fapi_vendor_adaptor STATIC ${SOURCES})
target_include_directories(fapi_vendor_adaptor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# TODO: Link here the libraries of your PHY SDK, e.g. ${VENDOR_SDK_LIBRARY}.
target_link_libraries(fapi_vendor_adaptor PRIVATE ocudu_ran ocudu_support)
