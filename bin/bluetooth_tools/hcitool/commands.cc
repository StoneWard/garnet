// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands.h"

#include <endian.h>

#include <iostream>
#include <cstring>

#include "apps/bluetooth/hci/advertising_report_parser.h"
#include "apps/bluetooth/hci/command_channel.h"
#include "apps/bluetooth/hci/command_packet.h"
#include "apps/bluetooth/hci/event_packet.h"
#include "lib/ftl/strings/string_number_conversions.h"
#include "lib/ftl/strings/string_printf.h"
#include "lib/ftl/time/time_delta.h"

#include "command_dispatcher.h"

using namespace bluetooth;

using std::placeholders::_1;
using std::placeholders::_2;

namespace hcitool {
namespace {

void StatusCallback(ftl::Closure complete_cb,
                    bluetooth::hci::CommandChannel::TransactionId id,
                    bluetooth::hci::Status status) {
  std::cout << "  Command Status: " << ftl::StringPrintf("0x%02x", status)
            << " (id=" << id << ")" << std::endl;
  if (status != bluetooth::hci::Status::kSuccess) complete_cb();
}

hci::CommandChannel::TransactionId SendCommand(
    const CommandDispatcher& owner,
    const hci::CommandPacket& packet,
    const hci::CommandChannel::CommandCompleteCallback& cb,
    const ftl::Closure& complete_cb) {
  return owner.cmd_channel()->SendCommand(
      packet, std::bind(&StatusCallback, complete_cb, _1, _2), cb,
      owner.task_runner());
}

void LogCommandComplete(hci::Status status,
                        hci::CommandChannel::TransactionId id) {
  std::cout << "  Command Complete - status: "
            << ftl::StringPrintf("0x%02x", status) << " (id=" << id << ")"
            << std::endl;
}

constexpr size_t BufferSize(size_t payload_size) {
  return hci::CommandPacket::GetMinBufferSize(payload_size);
}

bool HandleReset(const CommandDispatcher& owner,
                 const ftl::CommandLine& cmd_line,
                 const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size() || cmd_line.options().size()) {
    std::cout << "  Usage: reset" << std::endl;
    return false;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto status = event.GetReturnParams<hci::ResetReturnParams>()->status;
    LogCommandComplete(status, id);
    complete_cb();
  };

  common::StaticByteBuffer<BufferSize(0u)> buffer;
  hci::CommandPacket packet(hci::kReset, &buffer);
  packet.EncodeHeader();

  auto id = SendCommand(owner, packet, cb, complete_cb);
  std::cout << "  Sent HCI_Reset (id=" << id << ")" << std::endl;

  return true;
}

bool HandleReadBDADDR(const CommandDispatcher& owner,
                      const ftl::CommandLine& cmd_line,
                      const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size() || cmd_line.options().size()) {
    std::cout << "  Usage: read-bdaddr" << std::endl;
    return false;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params = event.GetReturnParams<hci::ReadBDADDRReturnParams>();
    LogCommandComplete(return_params->status, id);
    if (return_params->status != hci::Status::kSuccess) {
      complete_cb();
      return;
    }

    std::cout << "  BD_ADDR: " << return_params->bd_addr.ToString() << std::endl;
    complete_cb();
  };

  common::StaticByteBuffer<BufferSize(0u)> buffer;
  hci::CommandPacket packet(hci::kReadBDADDR, &buffer);
  packet.EncodeHeader();

  auto id = SendCommand(owner, packet, cb, complete_cb);
  std::cout << "  Sent HCI_Read_BDADDR (id=" << id << ")" << std::endl;

  return true;
}

bool HandleReadLocalName(const CommandDispatcher& owner,
                         const ftl::CommandLine& cmd_line,
                         const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size() || cmd_line.options().size()) {
    std::cout << "  Usage: read-local-name" << std::endl;
    return false;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::ReadLocalNameReturnParams>();
    LogCommandComplete(return_params->status, id);
    if (return_params->status != hci::Status::kSuccess) {
      complete_cb();
      return;
    }

    std::cout << "  Local Name: " << return_params->local_name << std::endl;

    complete_cb();
  };

  common::StaticByteBuffer<BufferSize(0u)> buffer;
  hci::CommandPacket packet(hci::kReadLocalName, &buffer);
  packet.EncodeHeader();

  auto id = SendCommand(owner, packet, cb, complete_cb);
  std::cout << "  Sent HCI_Read_Local_Name (id=" << id << ")" << std::endl;

  return true;
}

bool HandleWriteLocalName(const CommandDispatcher& owner,
                          const ftl::CommandLine& cmd_line,
                          const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size() != 1 || cmd_line.options().size()) {
    std::cout << "  Usage: write-local-name <name>" << std::endl;
    return false;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::WriteLocalNameReturnParams>();
    LogCommandComplete(return_params->status, id);
    complete_cb();
  };

  const std::string& name = cmd_line.positional_args()[0];
  common::StaticByteBuffer<BufferSize(hci::kMaxLocalNameLength)> buffer;
  hci::CommandPacket packet(hci::kWriteLocalName, &buffer, name.length() + 1);
  buffer.GetMutableData()[name.length()] = '\0';
  std::strcpy(
      (char*)packet.GetPayload<hci::WriteLocalNameCommandParams>()->local_name,
      name.c_str());
  packet.EncodeHeader();

  auto id = SendCommand(owner, packet, cb, complete_cb);
  std::cout << "  Sent HCI_Write_Local_Name (id=" << id << ")" << std::endl;

  return true;
}

bool HandleSetAdvEnable(const CommandDispatcher& owner,
                        const ftl::CommandLine& cmd_line,
                        const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size() != 1 || cmd_line.options().size()) {
    std::cout << "  Usage: set-adv-enable [enable|disable]" << std::endl;
    return false;
  }

  hci::GenericEnableParam value;
  std::string cmd_arg = cmd_line.positional_args()[0];
  if (cmd_arg == "enable") {
    value = hci::GenericEnableParam::kEnable;
  } else if (cmd_arg == "disable") {
    value = hci::GenericEnableParam::kDisable;
  } else {
    std::cout << "  Unrecognized parameter: " << cmd_arg << std::endl;
    std::cout << "  Usage: set-adv-enable [enable|disable]" << std::endl;
    return false;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetAdvertisingEnableReturnParams>();
    LogCommandComplete(return_params->status, id);
    complete_cb();
  };

  constexpr size_t kPayloadSize =
      sizeof(hci::LESetAdvertisingEnableCommandParams);
  constexpr size_t kBufferSize = BufferSize(kPayloadSize);

  common::StaticByteBuffer<kBufferSize> buffer;
  hci::CommandPacket packet(hci::kLESetAdvertisingEnable, &buffer,
                            kPayloadSize);
  packet.GetPayload<hci::LESetAdvertisingEnableCommandParams>()
      ->advertising_enable = value;
  packet.EncodeHeader();

  auto id = SendCommand(owner, packet, cb, complete_cb);

  std::cout << "  Sent HCI_LE_Set_Advertising_Enable (id=" << id << ")"
            << std::endl;
  return true;
}

bool HandleSetAdvParams(const CommandDispatcher& owner,
                        const ftl::CommandLine& cmd_line,
                        const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size()) {
    std::cout << "  Usage: set-adv-params [--help|--type]" << std::endl;
    return false;
  }

  if (cmd_line.HasOption("help")) {
    std::cout
        << "  Options: \n"
           "    --help - Display this help message\n"
           "    --type=<type> - The advertising type. Possible values are:\n"
           "          - nonconn: non-connectable undirected (default)\n"
           "          - adv-ind: connectable and scannable undirected\n"
           "          - direct-low: connectable directed low-duty\n"
           "          - direct-high: connectable directed high-duty\n"
           "          - scan: scannable undirected";
    std::cout << std::endl;
    return false;
  }

  hci::LEAdvertisingType adv_type = hci::LEAdvertisingType::kAdvNonConnInd;
  std::string type;
  if (cmd_line.GetOptionValue("type", &type)) {
    if (type == "adv-ind") {
      adv_type = hci::LEAdvertisingType::kAdvInd;
    } else if (type == "direct-low") {
      adv_type = hci::LEAdvertisingType::kAdvDirectIndLowDutyCycle;
    } else if (type == "direct-high") {
      adv_type = hci::LEAdvertisingType::kAdvDirectIndHighDutyCycle;
    } else if (type == "scan") {
      adv_type = hci::LEAdvertisingType::kAdvScanInd;
    } else if (type == "nonconn") {
      adv_type = hci::LEAdvertisingType::kAdvNonConnInd;
    } else {
      std::cout << "  Unrecognized advertising type: " << type << std::endl;
      return false;
    }
  }

  constexpr size_t kPayloadSize =
      sizeof(hci::LESetAdvertisingParametersCommandParams);
  common::StaticByteBuffer<BufferSize(kPayloadSize)> buffer;
  hci::CommandPacket packet(hci::kLESetAdvertisingParameters, &buffer,
                            kPayloadSize);
  auto params =
      packet.GetPayload<hci::LESetAdvertisingParametersCommandParams>();
  params->adv_interval_min = htole16(hci::kLEAdvertisingIntervalDefault);
  params->adv_interval_max = htole16(hci::kLEAdvertisingIntervalDefault);
  params->adv_type = adv_type;
  params->own_address_type = hci::LEOwnAddressType::kPublic;
  params->peer_address_type = hci::LEPeerAddressType::kPublic;
  params->peer_address.SetToZero();
  params->adv_channel_map = hci::kLEAdvertisingChannelAll;
  params->adv_filter_policy = hci::LEAdvFilterPolicy::kAllowAll;

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetAdvertisingParametersReturnParams>();
    LogCommandComplete(return_params->status, id);
    complete_cb();
  };

  packet.EncodeHeader();
  auto id = SendCommand(owner, packet, cb, complete_cb);

  std::cout << "  Sent HCI_LE_Set_Advertising_Parameters (id=" << id << ")"
            << std::endl;

  return true;
}

bool HandleSetAdvData(const CommandDispatcher& owner,
                      const ftl::CommandLine& cmd_line,
                      const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size()) {
    std::cout << "  Usage: set-adv-data [--help|--name]" << std::endl;
    return false;
  }

  if (cmd_line.HasOption("help")) {
    std::cout
        << "  Options: \n"
           "    --help - Display this help message\n"
           "    --name=<local-name> - Set the \"Complete Local Name\" field";
    std::cout << std::endl;
    return false;
  }

  constexpr size_t kPayloadSize =
      sizeof(hci::LESetAdvertisingDataCommandParams);
  common::StaticByteBuffer<BufferSize(kPayloadSize)> buffer;
  buffer.SetToZeros();
  hci::CommandPacket packet(hci::kLESetAdvertisingData, &buffer, kPayloadSize);

  std::string name;
  if (cmd_line.GetOptionValue("name", &name)) {
    // Each advertising data structure consists of a 1 octet length field, 1
    // octet type field.
    size_t adv_data_len = 2 + name.length();
    if (adv_data_len > hci::kMaxLEAdvertisingDataLength) {
      std::cout << "  Given name is too long" << std::endl;
      return false;
    }

    auto params = packet.GetPayload<hci::LESetAdvertisingDataCommandParams>();
    params->adv_data_length = adv_data_len;
    params->adv_data[0] = adv_data_len - 1;
    params->adv_data[1] = 0x09;  // Complete Local Name
    std::strncpy((char*)params->adv_data + 2, name.c_str(), name.length());
  } else {
    packet.GetPayload<hci::LESetAdvertisingDataCommandParams>()
        ->adv_data_length = 0;
  }

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetAdvertisingDataReturnParams>();
    LogCommandComplete(return_params->status, id);
    complete_cb();
  };

  packet.EncodeHeader();
  auto id = SendCommand(owner, packet, cb, complete_cb);

  std::cout << "  Sent HCI_LE_Set_Advertising_Data (id=" << id << ")"
            << std::endl;

  return true;
}

bool HandleSetScanParams(const CommandDispatcher& owner,
                         const ftl::CommandLine& cmd_line,
                         const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size()) {
    std::cout << "  Usage: set-scan-params [--help|--type]" << std::endl;
    return false;
  }

  if (cmd_line.HasOption("help")) {
    std::cout << "  Options: \n"
                 "    --help - Display this help message\n"
                 "    --type=<type> - The scan type. Possible values are:\n"
                 "          - passive: passive scanning (default)\n"
                 "          - active: active scanning; sends scan requests";
    std::cout << std::endl;
    return false;
  }

  hci::LEScanType scan_type = hci::LEScanType::kPassive;
  std::string type;
  if (cmd_line.GetOptionValue("type", &type)) {
    if (type == "passive") {
      scan_type = hci::LEScanType::kPassive;
    } else if (type == "active") {
      scan_type = hci::LEScanType::kActive;
    } else {
      std::cout << "  Unrecognized scan type: " << type << std::endl;
      return false;
    }
  }

  constexpr size_t kPayloadSize = sizeof(hci::LESetScanParametersCommandParams);
  common::StaticByteBuffer<BufferSize(kPayloadSize)> buffer;
  hci::CommandPacket packet(hci::kLESetScanParameters, &buffer, kPayloadSize);

  auto params = packet.GetPayload<hci::LESetScanParametersCommandParams>();
  params->scan_type = scan_type;
  params->scan_interval = htole16(hci::kLEScanIntervalDefault);
  params->scan_window = htole16(hci::kLEScanIntervalDefault);
  params->own_address_type = hci::LEOwnAddressType::kPublic;
  params->filter_policy = hci::LEScanFilterPolicy::kNoWhiteList;

  auto cb = [complete_cb](hci::CommandChannel::TransactionId id,
                          const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetScanParametersReturnParams>();
    LogCommandComplete(return_params->status, id);
    complete_cb();
  };

  packet.EncodeHeader();
  auto id = SendCommand(owner, packet, cb, complete_cb);

  std::cout << "  Sent HCI_LE_Set_Scan_Parameters (id=" << id << ")"
            << std::endl;

  return true;
}

bool HandleSetScanEnable(const CommandDispatcher& owner,
                         const ftl::CommandLine& cmd_line,
                         const ftl::Closure& complete_cb) {
  if (cmd_line.positional_args().size()) {
    std::cout << "  Usage: set-scan-params [--help|--timeout=<t>|--no-dedup]"
              << std::endl;
    return false;
  }

  if (cmd_line.HasOption("help")) {
    std::cout
        << "  Options: \n"
           "    --help - Display this help message\n"
           "    --timeout=<t> - Duration (in seconds) during which to scan\n"
           "                    (default is 10 seconds)\n"
           "    --no-dedup - Tell the controller not to filter duplicate "
           "reports";
    std::cout << std::endl;
    return false;
  }

  auto timeout = ftl::TimeDelta::FromSeconds(10);  // Default to 10 seconds.
  std::string timeout_str;
  if (cmd_line.GetOptionValue("timeout", &timeout_str)) {
    uint32_t time_seconds;
    if (!ftl::StringToNumberWithError(timeout_str, &time_seconds)) {
      std::cout << "  Malformed timeout value: " << timeout_str << std::endl;
      return false;
    }

    timeout = ftl::TimeDelta::FromSeconds(time_seconds);
  }

  hci::GenericEnableParam filter_duplicates = hci::GenericEnableParam::kEnable;
  if (cmd_line.HasOption("no-dedup")) {
    filter_duplicates = hci::GenericEnableParam::kDisable;
  }

  constexpr size_t kPayloadSize = sizeof(hci::LESetScanEnableCommandParams);
  common::StaticByteBuffer<BufferSize(kPayloadSize)> buffer;
  hci::CommandPacket packet(hci::kLESetScanEnable, &buffer, kPayloadSize);

  auto params = packet.GetPayload<hci::LESetScanEnableCommandParams>();
  params->scanning_enabled = hci::GenericEnableParam::kEnable;
  params->filter_duplicates = filter_duplicates;

  // Event handler to log when we receive advertising reports
  auto le_meta_event_cb = [](const hci::EventPacket& event) {
    if (event.GetPayload<hci::LEMetaEventParams>()->subevent_code !=
        hci::kLEAdvertisingReportSubeventCode)
      return;

    hci::AdvertisingReportParser parser(event);

    // TODO(armansito): Factor the report logging out into a helper function.
    std::cout << "  LE_Advertising_Report Event" << std::endl;
    hci::LEAdvertisingReportData* data;
    int8_t rssi;
    while (parser.GetNextReport(&data, &rssi)) {
      std::cout << "  Report:" << std::endl;
      std::cout << "    RSSI: " << ftl::NumberToString(rssi) << std::endl;
      std::cout << "    type: ";
      switch (data->event_type) {
        case hci::LEAdvertisingEventType::kAdvInd:
          std::cout << "ADV_IND" << std::endl;
          break;
        case hci::LEAdvertisingEventType::kAdvDirectInd:
          std::cout << "ADV_DIRECT_IND" << std::endl;
          break;
        case hci::LEAdvertisingEventType::kAdvScanInd:
          std::cout << "ADV_SCAN_IND" << std::endl;
          break;
        case hci::LEAdvertisingEventType::kAdvNonConnInd:
          std::cout << "ADV_NONCONN_IND" << std::endl;
          break;
        case hci::LEAdvertisingEventType::kScanRsp:
          std::cout << "SCAN_RSP" << std::endl;
          break;
        default:
          std::cout << "(unknown)" << std::endl;
          break;
      }
      std::cout << "    addres type: ";
      switch (data->address_type) {
        case hci::LEAddressType::kPublic:
          std::cout << "public" << std::endl;
          break;
        case hci::LEAddressType::kRandom:
          std::cout << "random" << std::endl;
          break;
        case hci::LEAddressType::kPublicIdentity:
          std::cout << "public-identity (resolved private)" << std::endl;
          break;
        case hci::LEAddressType::kRandomIdentity:
          std::cout << "random-identity (resolved private)" << std::endl;
          break;
        default:
          std::cout << "(unknown)" << std::endl;
          break;
      }
      std::cout << "    BD_ADDR: " << data->address.ToString() << std::endl;
      std::cout << "    Data Length: " << ftl::NumberToString(data->length_data)
                << " bytes" << std::endl;
    }
  };
  auto event_handler_id = owner.cmd_channel()->AddEventHandler(
      hci::kLEMetaEventCode, le_meta_event_cb, owner.task_runner());

  auto cleanup_cb =
      [ complete_cb, event_handler_id, cmd_channel = owner.cmd_channel() ] {
    cmd_channel->RemoveEventHandler(event_handler_id);
    complete_cb();
  };

  // The callback invoked after scanning is stopped.
  auto final_cb = [cleanup_cb](hci::CommandChannel::TransactionId id,
                               const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetScanEnableReturnParams>();
    LogCommandComplete(return_params->status, id);
    cleanup_cb();
  };

  // Delayed task that stops scanning.
  auto scan_disable_cb = [kPayloadSize, cleanup_cb, final_cb, &owner] {
    common::StaticByteBuffer<BufferSize(kPayloadSize)> buffer;
    hci::CommandPacket packet(hci::kLESetScanEnable, &buffer, kPayloadSize);

    auto params = packet.GetPayload<hci::LESetScanEnableCommandParams>();
    params->scanning_enabled = hci::GenericEnableParam::kDisable;
    params->filter_duplicates = hci::GenericEnableParam::kDisable;

    packet.EncodeHeader();
    auto id = SendCommand(owner, packet, final_cb, cleanup_cb);

    std::cout << "  Sent HCI_LE_Set_Scan_Enable (disabled) (id=" << id << ")"
              << std::endl;
  };

  auto cb = [
    scan_disable_cb, cleanup_cb, timeout, task_runner = owner.task_runner()
  ](hci::CommandChannel::TransactionId id, const hci::EventPacket& event) {
    auto return_params =
        event.GetReturnParams<hci::LESetScanEnableReturnParams>();
    LogCommandComplete(return_params->status, id);
    if (return_params->status != hci::Status::kSuccess) {
      cleanup_cb();
      return;
    }
    task_runner->PostDelayedTask(scan_disable_cb, timeout);
  };

  packet.EncodeHeader();
  auto id = SendCommand(owner, packet, cb, complete_cb);

  std::cout << "  Sent HCI_LE_Set_Scan_Enable (enabled) (id=" << id << ")"
            << std::endl;

  return true;
}

}  // namespace

void RegisterCommands(CommandDispatcher* handler_map) {
  FTL_DCHECK(handler_map);

  handler_map->RegisterHandler("reset", "Send HCI_Reset", HandleReset);
  handler_map->RegisterHandler("read-bdaddr", "Send HCI_Read_BDADDR",
                               HandleReadBDADDR);
  handler_map->RegisterHandler("read-local-name", "Send HCI_Read_Local_Name",
                               HandleReadLocalName);
  handler_map->RegisterHandler("write-local-name", "Send HCI_Write_Local_Name",
                               HandleWriteLocalName);
  handler_map->RegisterHandler("set-adv-enable",
                               "Send HCI_LE_Set_Advertising_Enable",
                               HandleSetAdvEnable);
  handler_map->RegisterHandler("set-adv-params",
                               "Send HCI_LE_Set_Advertising_Parameters",
                               HandleSetAdvParams);
  handler_map->RegisterHandler(
      "set-adv-data", "Send HCI_LE_Set_Advertising_Data", HandleSetAdvData);
  handler_map->RegisterHandler("set-scan-params",
                               "Send HCI_LE_Set_Scan_Parameters",
                               HandleSetScanParams);
  handler_map->RegisterHandler(
      "set-scan-enable", "Perform a LE device scan for a limited duration",
      HandleSetScanEnable);
}

}  // namespace hcitool
