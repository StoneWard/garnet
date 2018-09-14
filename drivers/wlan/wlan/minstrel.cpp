// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minstrel.h"

#include <wlan/mlme/debug.h>

#include <random>

namespace wlan {

zx::duration HeaderTxTimeErp() {
    // TODO(eyw): Implement Erp preamble and header
    return zx::nsec(0);
}

zx::duration PayloadTxTimeErp(SupportedRate rate) {
    // D_{bps} as defined in IEEE 802.11-2016 Table 17-4
    // Unit: Number of data bits per OFDM symbol
    uint16_t bits_per_symbol = rate.rate() * 2;
    constexpr int kTxTimePerSymbol = 4000;  // nanoseconds.

    uint32_t total_time = kTxTimePerSymbol * 8 * kMinstrelFrameLength / bits_per_symbol;
    return zx::nsec(total_time);
}

zx::duration TxTimeErp(SupportedRate rate) {
    return HeaderTxTimeErp() + PayloadTxTimeErp(rate);
}

std::unordered_map<tx_vec_idx_t, zx::duration> GetSupportedErp(
    const std::vector<SupportedRate>& rates) {
    std::unordered_map<tx_vec_idx_t, zx::duration> tx_vec_to_add;
    for (const auto& rate : rates) {
        TxVector tx_vector;
        zx_status_t status = TxVector::FromSupportedRate(rate, &tx_vector);
        ZX_DEBUG_ASSERT(status == ZX_OK);
        // Fuchsia only uses 802.11a/g/n and later data rates for transmission.
        if (tx_vector.phy != WLAN_PHY_ERP) { continue; }
        tx_vec_idx_t tx_vector_idx;
        status = tx_vector.ToIdx(&tx_vector_idx);
        zx::duration perfect_tx_time = TxTimeErp(rate);
        tx_vec_to_add.emplace(tx_vector_idx, TxTimeErp(rate));
        debugmstl("%s, tx_time %lu nsec\n", debug::Describe(tx_vector).c_str(),
                  perfect_tx_time.to_nsecs());
        tx_vec_to_add.emplace(tx_vector_idx, perfect_tx_time);
    }
    return tx_vec_to_add;
}

zx::duration HeaderTxTimeHt() {
    // TODO(eyw): Implement Plcp preamble and header
    return zx::nsec(0);
}

// relative_mcs_idx is the index for combination of (modulation, coding rate) tuple
// when listed in the same order as MCS Index, without nss. i.e.
// 0: BPSK, 1/2
// 1: QPSK, 1/2
// 2: QPSK, 3/4
// 3: 16-QAM, 1/2
// 4: 16-QAM, 3/4
// 5: 64-QAM, 2/3
// 6: 64-QAM, 3/4
// 7: 64-QAM, 5/6
// 8: 256-QAM, 3/4 (since VHT)
// 9: 256-QAM, 5/6 (since VHT)
zx::duration PayloadTxTimeHt(CBW cbw, GI gi, size_t mcs_idx) {
    // D_{bps} as defined in IEEE 802.11-2016 Table 19-26
    // Unit: Number of data bits per OFDM symbol (20 MHz channel width)
    constexpr uint16_t bits_per_symbol_list[] = {
        26, 52, 78, 104, 156, 208, 234, 260, /* since VHT */ 312, 347};
    constexpr uint16_t kDataSubCarriers20 = 52;
    constexpr uint16_t kDataSubCarriers40 = 108;
    // TODO(eyw): VHT would have kDataSubCarriers80 = 234 and kDataSubCarriers160 = 468

    ZX_DEBUG_ASSERT(gi == WLAN_GI_400NS || gi == WLAN_GI_800NS);

    int nss = 1 + mcs_idx / kNumUniqueMcsHt;
    int relative_mcs_idx = mcs_idx % kNumUniqueMcsHt;

    uint16_t bits_per_symbol = bits_per_symbol_list[relative_mcs_idx];
    if (cbw == CBW40) {
        bits_per_symbol = bits_per_symbol * kDataSubCarriers40 / kDataSubCarriers20;
    }

    constexpr int kTxTimePerSymbolGi800 = 4000;  // nanoseconds.
    constexpr int kTxTimePerSymbolGi400 = 3600;  // nanoseconds.
    // Perform multiplication before division to prevent precision loss
    uint32_t total_time =
        kTxTimePerSymbolGi800 * 8 * kMinstrelFrameLength / (nss * bits_per_symbol);

    if (gi == WLAN_GI_400NS) {
        total_time =
            800 + (kTxTimePerSymbolGi400 * 8 * kMinstrelFrameLength / (nss * bits_per_symbol));
    }
    return zx::nsec(total_time);
}

zx::duration TxTimeHt(CBW cbw, GI gi, uint8_t relative_mcs_idx) {
    return HeaderTxTimeHt() + PayloadTxTimeHt(cbw, gi, relative_mcs_idx);
}

// SupportedMcsRx is 78 bit long in IEEE802.11-2016, Figure 9-334
// In reality, devices implement MCS 0-31, sometimes 32, almost never beyond 32.
std::unordered_map<tx_vec_idx_t, zx::duration> GetSupportedHt(
    CBW cbw, GI gi, const SupportedMcsRxMcsHead& mcs_set) {
    std::unordered_map<tx_vec_idx_t, zx::duration> tx_vec_to_add;
    for (uint8_t mcs_idx = 0; mcs_idx < kHtNumMcs; ++mcs_idx) {
        // Skip if this mcs is not supported
        if (!mcs_set.Support(mcs_idx)) { continue; }

        TxVector tx_vector{
            .phy = WLAN_PHY_HT,
            .gi = gi,
            .cbw = cbw,
            .mcs_idx = mcs_idx,
        };
        tx_vec_idx_t tx_vector_idx;
        zx_status_t status = tx_vector.ToIdx(&tx_vector_idx);
        ZX_DEBUG_ASSERT(status == ZX_OK);
        zx::duration perfect_tx_time = TxTimeHt(cbw, gi, mcs_idx);
        debugmstl("%s, tx_time %lu nsec\n", debug::Describe(tx_vector).c_str(),
                  perfect_tx_time.to_nsecs());
        tx_vec_to_add.emplace(tx_vector_idx, perfect_tx_time);
    }
    return tx_vec_to_add;
}

MinstrelRateSelector::MinstrelRateSelector(TimerManager&& timer_mgr)
    : timer_mgr_(fbl::move(timer_mgr)) {}

void AddErp(std::unordered_map<tx_vec_idx_t, TxStats>* tx_stats_map,
            const wlan_assoc_ctx_t& assoc_ctx) {
    std::vector<SupportedRate> legacy_rates(assoc_ctx.supported_rates_cnt +
                                            assoc_ctx.ext_supported_rates_cnt);

    std::transform(assoc_ctx.supported_rates,
                   assoc_ctx.supported_rates + assoc_ctx.supported_rates_cnt, legacy_rates.begin(),
                   SupportedRate::basic);
    std::transform(assoc_ctx.ext_supported_rates,
                   assoc_ctx.ext_supported_rates + assoc_ctx.ext_supported_rates_cnt,
                   legacy_rates.begin() + assoc_ctx.supported_rates_cnt, SupportedRate::basic);

    debugmstl("Supported rates: %s\n", debug::Describe(legacy_rates).c_str());
    auto erp_to_add = GetSupportedErp(legacy_rates);
    debugmstl("%zu ERP added.\n", erp_to_add.size());
    for (auto iter : erp_to_add) {
        TxStats tx_stats;
        tx_stats.tx_vector_idx = iter.first;
        tx_stats.perfect_tx_time = iter.second;
        tx_stats_map->emplace(iter.first, tx_stats);
    }
}

void AddHt(std::unordered_map<tx_vec_idx_t, TxStats>* tx_stats_map, const HtCapabilities& ht_cap) {
    tx_vec_idx_t max_size = kHtNumMcs;
    uint8_t assoc_chan_width = 20;

    if (ht_cap.ht_cap_info.chan_width_set() == HtCapabilityInfo::ChanWidthSet::TWENTY_FORTY) {
        assoc_chan_width = 40;
        max_size *= 2;
    }
    uint8_t assoc_sgi = WLAN_GI_800NS;      // SGI not supported yet
    max_size = max_size + kErpNumTxVector;  // Taking in to account legacy_rates.

    debugmstl("max_size is %d.\n", max_size);

    tx_stats_map->reserve(max_size);

    // Enumerate all combinations of chan_width, gi, nss and mcs_idx
    for (uint8_t bw = 20; bw <= assoc_chan_width; bw *= 2) {
        uint8_t cbw = bw == 20 ? CBW20 : CBW40;
        for (uint8_t gi = 1 << 0; gi <= assoc_sgi; gi <<= 1) {
            auto tx_params_to_add = GetSupportedHt(static_cast<CBW>(cbw), static_cast<GI>(gi),
                                                   ht_cap.mcs_set.rx_mcs_head);
            debugmstl("%zu HT added with cbw=%u, gi=%u\n", tx_params_to_add.size(), cbw, gi);
            for (auto iter : tx_params_to_add) {
                TxStats tx_stats;
                tx_stats.tx_vector_idx = iter.first;
                tx_stats.perfect_tx_time = iter.second;
                tx_stats_map->emplace(iter.first, tx_stats);
            }
        }
    }
    debugmstl("tx_stats_map size: %zu.\n", tx_stats_map->size());
}

void MinstrelRateSelector::AddPeer(const wlan_assoc_ctx_t& assoc_ctx) {
    auto addr = common::MacAddr(assoc_ctx.bssid);
    Peer peer{};
    peer.addr = addr;

    HtCapabilities ht_cap;
    constexpr uint32_t kMcsMask0_31 = 0xFFFFFFFF;
    if (assoc_ctx.has_ht_cap) {
        ht_cap = HtCapabilities::FromDdk(assoc_ctx.ht_cap);
        if ((ht_cap.mcs_set.rx_mcs_head.bitmask() & kMcsMask0_31) == 0) {
            errorf("Invalid AssocCtx: HT supported but no valid MCS. %s\n",
                   debug::Describe(ht_cap.mcs_set).c_str());
            ZX_DEBUG_ASSERT(false);
        } else {
            peer.is_ht = true;
            AddHt(&peer.tx_stats_map, ht_cap);
        }
    }

    if (assoc_ctx.supported_rates_cnt + assoc_ctx.ext_supported_rates_cnt > 0) {
        AddErp(&peer.tx_stats_map, assoc_ctx);
    }

    debugmstl("tx_stats_map populated. size: %zu.\n", peer.tx_stats_map.size());

    if (peer.tx_stats_map.size() == 0) {
        errorf("No usable rates for peer %s.\n", addr.ToString().c_str());
        ZX_DEBUG_ASSERT(false);
    }

    debugmstl("Minstrel peer added: %s\n", addr.ToString().c_str());
    peer_map_.emplace(addr, std::move(peer));
    // TODO(eyw): RemovePeer() needs to be called at de-association.
}

void MinstrelRateSelector::RemovePeer(const common::MacAddr& addr) {
    auto iter = peer_map_.find(addr);
    if (iter == peer_map_.end()) { return; }

    peer_map_.erase(iter);
}

void MinstrelRateSelector::HandleTxStatusReport(const wlan_tx_status_t& tx_status) {
    auto peer_addr = common::MacAddr(tx_status.peer_addr);
    auto peer = GetPeer(peer_addr);
    ZX_DEBUG_ASSERT(peer != nullptr);
    if (peer == nullptr) {
        errorf("Peer [%s] does not exist for tx status report.\n", peer_addr.ToString().c_str());
        return;
    }

    auto tx_stats_map = &peer->tx_stats_map;
    tx_vec_idx_t last_idx = kInvalidTxVectorIdx;
    for (auto entry : tx_status.tx_status_entry) {
        if (entry.tx_vector_idx == kInvalidTxVectorIdx) { break; }
        last_idx = entry.tx_vector_idx;
        (*tx_stats_map)[last_idx].attempts += entry.attempts;
    }
    if (tx_status.success && last_idx != kInvalidTxVectorIdx) {
        (*tx_stats_map)[last_idx].success++;
    }

    peer->has_update = true;
}

void MinstrelRateSelector::UpdateStats() {
    for (auto iter = peer_map_.begin(); iter != peer_map_.end(); ++iter) {
        if (!iter->second.has_update) { continue; }
        iter->second.has_update = false;

        // TODO(eyw): Loop through all tx_stats and pick the best combination for next update period
    }
}

Peer* MinstrelRateSelector::GetPeer(const common::MacAddr& addr) {
    auto iter = peer_map_.find(addr);
    if (iter != peer_map_.end()) { return &(iter->second); }
    return nullptr;
}
}  // namespace wlan
