/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "hw.h"

#include <stdint.h>

#include "bmi.h"
#include "core.h"
#include "hif.h"
#include "macros.h"
#include "wmi-ops.h"

const struct ath10k_hw_regs qca988x_regs = {
    .rtc_soc_base_address                   = 0x00004000,
    .rtc_wmac_base_address                  = 0x00005000,
    .soc_core_base_address                  = 0x00009000,
    .wlan_mac_base_address                  = 0x00020000,
    .ce_wrapper_base_address                = 0x00057000,
    .ce0_base_address                       = 0x00057400,
    .ce1_base_address                       = 0x00057800,
    .ce2_base_address                       = 0x00057c00,
    .ce3_base_address                       = 0x00058000,
    .ce4_base_address                       = 0x00058400,
    .ce5_base_address                       = 0x00058800,
    .ce6_base_address                       = 0x00058c00,
    .ce7_base_address                       = 0x00059000,
    .soc_reset_control_si0_rst_mask         = 0x00000001,
    .soc_reset_control_ce_rst_mask          = 0x00040000,
    .soc_chip_id_address                    = 0x000000ec,
    .scratch_3_address                      = 0x00000030,
    .fw_indicator_address                   = 0x00009030,
    .pcie_local_base_address                = 0x00080000,
    .ce_wrap_intr_sum_host_msi_lsb          = 0x00000008,
    .ce_wrap_intr_sum_host_msi_mask         = 0x0000ff00,
    .pcie_intr_fw_mask                      = 0x00000400,
    .pcie_intr_ce_mask_all                  = 0x0007f800,
    .pcie_intr_clr_address                  = 0x00000014,
};

const struct ath10k_hw_regs qca6174_regs = {
    .rtc_soc_base_address                   = 0x00000800,
    .rtc_wmac_base_address                  = 0x00001000,
    .soc_core_base_address                  = 0x0003a000,
    .wlan_mac_base_address                  = 0x00010000,
    .ce_wrapper_base_address                = 0x00034000,
    .ce0_base_address                       = 0x00034400,
    .ce1_base_address                       = 0x00034800,
    .ce2_base_address                       = 0x00034c00,
    .ce3_base_address                       = 0x00035000,
    .ce4_base_address                       = 0x00035400,
    .ce5_base_address                       = 0x00035800,
    .ce6_base_address                       = 0x00035c00,
    .ce7_base_address                       = 0x00036000,
    .soc_reset_control_si0_rst_mask         = 0x00000000,
    .soc_reset_control_ce_rst_mask          = 0x00000001,
    .soc_chip_id_address                    = 0x000000f0,
    .scratch_3_address                      = 0x00000028,
    .fw_indicator_address                   = 0x0003a028,
    .pcie_local_base_address                = 0x00080000,
    .ce_wrap_intr_sum_host_msi_lsb          = 0x00000008,
    .ce_wrap_intr_sum_host_msi_mask         = 0x0000ff00,
    .pcie_intr_fw_mask                      = 0x00000400,
    .pcie_intr_ce_mask_all                  = 0x0007f800,
    .pcie_intr_clr_address                  = 0x00000014,
    .cpu_pll_init_address                   = 0x00404020,
    .cpu_speed_address                      = 0x00404024,
    .core_clk_div_address                   = 0x00404028,
};

const struct ath10k_hw_regs qca99x0_regs = {
    .rtc_soc_base_address                   = 0x00080000,
    .rtc_wmac_base_address                  = 0x00000000,
    .soc_core_base_address                  = 0x00082000,
    .wlan_mac_base_address                  = 0x00030000,
    .ce_wrapper_base_address                = 0x0004d000,
    .ce0_base_address                       = 0x0004a000,
    .ce1_base_address                       = 0x0004a400,
    .ce2_base_address                       = 0x0004a800,
    .ce3_base_address                       = 0x0004ac00,
    .ce4_base_address                       = 0x0004b000,
    .ce5_base_address                       = 0x0004b400,
    .ce6_base_address                       = 0x0004b800,
    .ce7_base_address                       = 0x0004bc00,
    /* Note: qca99x0 supports upto 12 Copy Engines. Other than address of
     * CE0 and CE1 no other copy engine is directly referred in the code.
     * It is not really necessary to assign address for newly supported
     * CEs in this address table.
     *  Copy Engine     Address
     *  CE8         0x0004c000
     *  CE9         0x0004c400
     *  CE10            0x0004c800
     *  CE11            0x0004cc00
     */
    .soc_reset_control_si0_rst_mask         = 0x00000001,
    .soc_reset_control_ce_rst_mask          = 0x00000100,
    .soc_chip_id_address                    = 0x000000ec,
    .scratch_3_address                      = 0x00040050,
    .fw_indicator_address                   = 0x00040050,
    .pcie_local_base_address                = 0x00000000,
    .ce_wrap_intr_sum_host_msi_lsb          = 0x0000000c,
    .ce_wrap_intr_sum_host_msi_mask         = 0x00fff000,
    .pcie_intr_fw_mask                      = 0x00100000,
    .pcie_intr_ce_mask_all                  = 0x000fff00,
    .pcie_intr_clr_address                  = 0x00000010,
};

const struct ath10k_hw_regs qca4019_regs = {
    .rtc_soc_base_address                   = 0x00080000,
    .soc_core_base_address                  = 0x00082000,
    .wlan_mac_base_address                  = 0x00030000,
    .ce_wrapper_base_address                = 0x0004d000,
    .ce0_base_address                       = 0x0004a000,
    .ce1_base_address                       = 0x0004a400,
    .ce2_base_address                       = 0x0004a800,
    .ce3_base_address                       = 0x0004ac00,
    .ce4_base_address                       = 0x0004b000,
    .ce5_base_address                       = 0x0004b400,
    .ce6_base_address                       = 0x0004b800,
    .ce7_base_address                       = 0x0004bc00,
    /* qca4019 supports upto 12 copy engines. Since base address
     * of ce8 to ce11 are not directly referred in the code,
     * no need have them in separate members in this table.
     *      Copy Engine             Address
     *      CE8                     0x0004c000
     *      CE9                     0x0004c400
     *      CE10                    0x0004c800
     *      CE11                    0x0004cc00
     */
    .soc_reset_control_si0_rst_mask         = 0x00000001,
    .soc_reset_control_ce_rst_mask          = 0x00000100,
    .soc_chip_id_address                    = 0x000000ec,
    .fw_indicator_address                   = 0x0004f00c,
    .ce_wrap_intr_sum_host_msi_lsb          = 0x0000000c,
    .ce_wrap_intr_sum_host_msi_mask         = 0x00fff000,
    .pcie_intr_fw_mask                      = 0x00100000,
    .pcie_intr_ce_mask_all                  = 0x000fff00,
    .pcie_intr_clr_address                  = 0x00000010,
};

const struct ath10k_hw_values qca988x_values = {
    .rtc_state_val_on               = 3,
    .ce_count                       = 8,
    .msi_assign_ce_max              = 7,
    .num_target_ce_config_wlan      = 7,
    .ce_desc_meta_data_mask         = 0xFFFC,
    .ce_desc_meta_data_lsb          = 2,
};

const struct ath10k_hw_values qca6174_values = {
    .rtc_state_val_on               = 3,
    .ce_count                       = 8,
    .msi_assign_ce_max              = 7,
    .num_target_ce_config_wlan      = 7,
    .ce_desc_meta_data_mask         = 0xFFFC,
    .ce_desc_meta_data_lsb          = 2,
};

const struct ath10k_hw_values qca99x0_values = {
    .rtc_state_val_on               = 5,
    .ce_count                       = 12,
    .msi_assign_ce_max              = 12,
    .num_target_ce_config_wlan      = 10,
    .ce_desc_meta_data_mask         = 0xFFF0,
    .ce_desc_meta_data_lsb          = 4,
};

const struct ath10k_hw_values qca9888_values = {
    .rtc_state_val_on               = 3,
    .ce_count                       = 12,
    .msi_assign_ce_max              = 12,
    .num_target_ce_config_wlan      = 10,
    .ce_desc_meta_data_mask         = 0xFFF0,
    .ce_desc_meta_data_lsb          = 4,
};

const struct ath10k_hw_values qca4019_values = {
    .ce_count                       = 12,
    .num_target_ce_config_wlan      = 10,
    .ce_desc_meta_data_mask         = 0xFFF0,
    .ce_desc_meta_data_lsb          = 4,
};

static struct ath10k_hw_ce_regs_addr_map qcax_src_ring = {
    .msb    = 0x00000010,
    .lsb    = 0x00000010,
    .mask   = BITMASK(16, 16),
};

static struct ath10k_hw_ce_regs_addr_map qcax_dst_ring = {
    .msb    = 0x00000011,
    .lsb    = 0x00000011,
    .mask   = BITMASK(17, 17),
};

static struct ath10k_hw_ce_regs_addr_map qcax_dmax = {
    .msb    = 0x0000000f,
    .lsb    = 0x00000000,
    .mask   = BITMASK(0, 15),
};

static struct ath10k_hw_ce_ctrl1 qcax_ctrl1 = {
    .addr           = 0x00000010,
    .hw_mask        = 0x0007ffff,
    .sw_mask        = 0x0007ffff,
    .hw_wr_mask     = 0x00000000,
    .sw_wr_mask     = 0x0007ffff,
    .reset_mask     = 0xffffffff,
    .reset          = 0x00000080,
    .src_ring       = &qcax_src_ring,
    .dst_ring       = &qcax_dst_ring,
    .dmax           = &qcax_dmax,
};

static struct ath10k_hw_ce_regs_addr_map qcax_cmd_halt_status = {
    .msb    = 0x00000003,
    .lsb    = 0x00000003,
    .mask   = BITMASK(3, 3),
};

static struct ath10k_hw_ce_cmd_halt qcax_cmd_halt = {
    .msb            = 0x00000000,
    .mask           = BITMASK(0, 0),
    .status_reset   = 0x00000000,
    .status         = &qcax_cmd_halt_status,
};

static struct ath10k_hw_ce_regs_addr_map qcax_host_ie_cc = {
    .msb    = 0x00000000,
    .lsb    = 0x00000000,
    .mask   = BITMASK(0, 0),
};

static struct ath10k_hw_ce_host_ie qcax_host_ie = {
    .copy_complete_reset    = 0x00000000,
    .copy_complete          = &qcax_host_ie_cc,
};

static struct ath10k_hw_ce_host_wm_regs qcax_wm_reg = {
    .dstr_lmask     = 0x00000010,
    .dstr_hmask     = 0x00000008,
    .srcr_lmask     = 0x00000004,
    .srcr_hmask     = 0x00000002,
    .cc_mask        = 0x00000001,
    .wm_mask        = 0x0000001E,
    .addr           = 0x00000030,
};

static struct ath10k_hw_ce_misc_regs qcax_misc_reg = {
    .axi_err        = 0x00000400,
    .dstr_add_err   = 0x00000200,
    .srcr_len_err   = 0x00000100,
    .dstr_mlen_vio  = 0x00000080,
    .dstr_overflow  = 0x00000040,
    .srcr_overflow  = 0x00000020,
    .err_mask       = 0x000007E0,
    .addr           = 0x00000038,
};

static struct ath10k_hw_ce_regs_addr_map qcax_src_wm_low = {
    .msb    = 0x0000001f,
    .lsb    = 0x00000010,
    .mask   = BITMASK(16, 31),
};

static struct ath10k_hw_ce_regs_addr_map qcax_src_wm_high = {
    .msb    = 0x0000000f,
    .lsb    = 0x00000000,
    .mask   = BITMASK(0, 15),
};

static struct ath10k_hw_ce_dst_src_wm_regs qcax_wm_src_ring = {
    .addr           = 0x0000004c,
    .low_rst        = 0x00000000,
    .high_rst       = 0x00000000,
    .wm_low         = &qcax_src_wm_low,
    .wm_high        = &qcax_src_wm_high,
};

static struct ath10k_hw_ce_regs_addr_map qcax_dst_wm_low = {
    .lsb    = 0x00000010,
    .mask   = BITMASK(16, 31),
};

static struct ath10k_hw_ce_regs_addr_map qcax_dst_wm_high = {
    .msb    = 0x0000000f,
    .lsb    = 0x00000000,
    .mask   = BITMASK(0, 15),
};

static struct ath10k_hw_ce_dst_src_wm_regs qcax_wm_dst_ring = {
    .addr           = 0x00000050,
    .low_rst        = 0x00000000,
    .high_rst       = 0x00000000,
    .wm_low         = &qcax_dst_wm_low,
    .wm_high        = &qcax_dst_wm_high,
};

struct ath10k_hw_ce_regs qcax_ce_regs = {
    .sr_base_addr           = 0x00000000,
    .sr_size_addr           = 0x00000004,
    .dr_base_addr           = 0x00000008,
    .dr_size_addr           = 0x0000000c,
    .ce_cmd_addr            = 0x00000018,
    .misc_ie_addr           = 0x00000034,
    .sr_wr_index_addr       = 0x0000003c,
    .dst_wr_index_addr      = 0x00000040,
    .current_srri_addr      = 0x00000044,
    .current_drri_addr      = 0x00000048,
    .host_ie_addr           = 0x0000002c,
    .ctrl1_regs             = &qcax_ctrl1,
    .cmd_halt               = &qcax_cmd_halt,
    .host_ie                = &qcax_host_ie,
    .wm_regs                = &qcax_wm_reg,
    .misc_regs              = &qcax_misc_reg,
    .wm_srcr                = &qcax_wm_src_ring,
    .wm_dstr                = &qcax_wm_dst_ring,
};

const struct ath10k_hw_clk_params qca6174_clk[ATH10K_HW_REFCLK_COUNT] = {
    {
        .refclk = 48000000,
        .div = 0xe,
        .rnfrac = 0x2aaa8,
        .settle_time = 2400,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 19200000,
        .div = 0x24,
        .rnfrac = 0x2aaa8,
        .settle_time = 960,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 24000000,
        .div = 0x1d,
        .rnfrac = 0x15551,
        .settle_time = 1200,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 26000000,
        .div = 0x1b,
        .rnfrac = 0x4ec4,
        .settle_time = 1300,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 37400000,
        .div = 0x12,
        .rnfrac = 0x34b49,
        .settle_time = 1870,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 38400000,
        .div = 0x12,
        .rnfrac = 0x15551,
        .settle_time = 1920,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 40000000,
        .div = 0x12,
        .rnfrac = 0x26665,
        .settle_time = 2000,
        .refdiv = 0,
        .outdiv = 1,
    },
    {
        .refclk = 52000000,
        .div = 0x1b,
        .rnfrac = 0x4ec4,
        .settle_time = 2600,
        .refdiv = 0,
        .outdiv = 1,
    },
};

#if 0 // NEEDS PORTING
void ath10k_hw_fill_survey_time(struct ath10k* ar, struct survey_info* survey,
                                uint32_t cc, uint32_t rcc, uint32_t cc_prev, uint32_t rcc_prev) {
    uint32_t cc_fix = 0;
    uint32_t rcc_fix = 0;
    enum ath10k_hw_cc_wraparound_type wraparound_type;

    survey->filled |= SURVEY_INFO_TIME |
                      SURVEY_INFO_TIME_BUSY;

    wraparound_type = ar->hw_params.cc_wraparound_type;

    if (cc < cc_prev || rcc < rcc_prev) {
        switch (wraparound_type) {
        case ATH10K_HW_CC_WRAP_SHIFTED_ALL:
            if (cc < cc_prev) {
                cc_fix = 0x7fffffff;
                survey->filled &= ~SURVEY_INFO_TIME_BUSY;
            }
            break;
        case ATH10K_HW_CC_WRAP_SHIFTED_EACH:
            if (cc < cc_prev) {
                cc_fix = 0x7fffffff;
            }

            if (rcc < rcc_prev) {
                rcc_fix = 0x7fffffff;
            }
            break;
        case ATH10K_HW_CC_WRAP_DISABLED:
            break;
        }
    }

    cc -= cc_prev - cc_fix;
    rcc -= rcc_prev - rcc_fix;

    survey->time = CCNT_TO_MSEC(ar, cc);
    survey->time_busy = CCNT_TO_MSEC(ar, rcc);
}
#endif // NEEDS PORTING

/* The firmware does not support setting the coverage class. Instead this
 * function monitors and modifies the corresponding MAC registers.
 */
static void ath10k_hw_qca988x_set_coverage_class(struct ath10k* ar,
        int16_t value) {
    ZX_ASSERT(0);
#if 0 // NEEDS PORTING
    uint32_t slottime_reg;
    uint32_t slottime;
    uint32_t timeout_reg;
    uint32_t ack_timeout;
    uint32_t cts_timeout;
    uint32_t phyclk_reg;
    uint32_t phyclk;
    uint64_t fw_dbglog_mask;
    uint32_t fw_dbglog_level;

    mtx_lock(&ar->conf_mutex);

    /* Only modify registers if the core is started. */
    if ((ar->state != ATH10K_STATE_ON) &&
            (ar->state != ATH10K_STATE_RESTARTED)) {
        goto unlock;
    }

    /* Retrieve the current values of the two registers that need to be
     * adjusted.
     */
    slottime_reg = ath10k_hif_read32(ar, WLAN_MAC_BASE_ADDRESS +
                                     WAVE1_PCU_GBL_IFS_SLOT);
    timeout_reg = ath10k_hif_read32(ar, WLAN_MAC_BASE_ADDRESS +
                                    WAVE1_PCU_ACK_CTS_TIMEOUT);
    phyclk_reg = ath10k_hif_read32(ar, WLAN_MAC_BASE_ADDRESS +
                                   WAVE1_PHYCLK);
    phyclk = MS(phyclk_reg, WAVE1_PHYCLK_USEC) + 1;

    if (value < 0) {
        value = ar->fw_coverage.coverage_class;
    }

    /* Break out if the coverage class and registers have the expected
     * value.
     */
    if (value == ar->fw_coverage.coverage_class &&
            slottime_reg == ar->fw_coverage.reg_slottime_conf &&
            timeout_reg == ar->fw_coverage.reg_ack_cts_timeout_conf &&
            phyclk_reg == ar->fw_coverage.reg_phyclk) {
        goto unlock;
    }

    /* Store new initial register values from the firmware. */
    if (slottime_reg != ar->fw_coverage.reg_slottime_conf) {
        ar->fw_coverage.reg_slottime_orig = slottime_reg;
    }
    if (timeout_reg != ar->fw_coverage.reg_ack_cts_timeout_conf) {
        ar->fw_coverage.reg_ack_cts_timeout_orig = timeout_reg;
    }
    ar->fw_coverage.reg_phyclk = phyclk_reg;

    /* Calculat new value based on the (original) firmware calculation. */
    slottime_reg = ar->fw_coverage.reg_slottime_orig;
    timeout_reg = ar->fw_coverage.reg_ack_cts_timeout_orig;

    /* Do some sanity checks on the slottime register. */
    if (slottime_reg % phyclk) {
        ath10k_warn("failed to set coverage class: expected integer microsecond value in register\n");

        goto store_regs;
    }

    slottime = MS(slottime_reg, WAVE1_PCU_GBL_IFS_SLOT);
    slottime = slottime / phyclk;
    if (slottime != 9 && slottime != 20) {
        ath10k_warn("failed to set coverage class: expected slot time of 9 or 20us in HW register. It is %uus.\n",
                    slottime);

        goto store_regs;
    }

    /* Recalculate the register values by adding the additional propagation
     * delay (3us per coverage class).
     */

    slottime = MS(slottime_reg, WAVE1_PCU_GBL_IFS_SLOT);
    slottime += value * 3 * phyclk;
    slottime = MIN_T(uint32_t, slottime, WAVE1_PCU_GBL_IFS_SLOT_MAX);
    slottime = SM(slottime, WAVE1_PCU_GBL_IFS_SLOT);
    slottime_reg = (slottime_reg & ~WAVE1_PCU_GBL_IFS_SLOT_MASK) | slottime;

    /* Update ack timeout (lower halfword). */
    ack_timeout = MS(timeout_reg, WAVE1_PCU_ACK_CTS_TIMEOUT_ACK);
    ack_timeout += 3 * value * phyclk;
    ack_timeout = MIN_T(uint32_t, ack_timeout, WAVE1_PCU_ACK_CTS_TIMEOUT_MAX);
    ack_timeout = SM(ack_timeout, WAVE1_PCU_ACK_CTS_TIMEOUT_ACK);

    /* Update cts timeout (upper halfword). */
    cts_timeout = MS(timeout_reg, WAVE1_PCU_ACK_CTS_TIMEOUT_CTS);
    cts_timeout += 3 * value * phyclk;
    cts_timeout = MIN_T(uint32_t, cts_timeout, WAVE1_PCU_ACK_CTS_TIMEOUT_MAX);
    cts_timeout = SM(cts_timeout, WAVE1_PCU_ACK_CTS_TIMEOUT_CTS);

    timeout_reg = ack_timeout | cts_timeout;

    ath10k_hif_write32(ar,
                       WLAN_MAC_BASE_ADDRESS + WAVE1_PCU_GBL_IFS_SLOT,
                       slottime_reg);
    ath10k_hif_write32(ar,
                       WLAN_MAC_BASE_ADDRESS + WAVE1_PCU_ACK_CTS_TIMEOUT,
                       timeout_reg);

    /* Ensure we have a debug level of WARN set for the case that the
     * coverage class is larger than 0. This is important as we need to
     * set the registers again if the firmware does an internal reset and
     * this way we will be notified of the event.
     */
    fw_dbglog_mask = ath10k_debug_get_fw_dbglog_mask(ar);
    fw_dbglog_level = ath10k_debug_get_fw_dbglog_level(ar);

    if (value > 0) {
        if (fw_dbglog_level > ATH10K_DBGLOG_LEVEL_WARN) {
            fw_dbglog_level = ATH10K_DBGLOG_LEVEL_WARN;
        }
        fw_dbglog_mask = ~0;
    }

    ath10k_wmi_dbglog_cfg(ar, fw_dbglog_mask, fw_dbglog_level);

store_regs:
    /* After an error we will not retry setting the coverage class. */
    mtx_lock(&ar->data_lock);
    ar->fw_coverage.coverage_class = value;
    mtx_unlock(&ar->data_lock);

    ar->fw_coverage.reg_slottime_conf = slottime_reg;
    ar->fw_coverage.reg_ack_cts_timeout_conf = timeout_reg;

unlock:
    mtx_unlock(&ar->conf_mutex);
#endif // NEEDS PORTING
}

/**
 * ath10k_hw_qca6174_enable_pll_clock() - enable the qca6174 hw pll clock
 * @ar: the ath10k blob
 *
 * This function is very hardware specific, the clock initialization
 * steps is very sensitive and could lead to unknown crash, so they
 * should be done in sequence.
 *
 * *** Be aware if you planned to refactor them. ***
 *
 * Return: 0 if successfully enable the pll, otherwise EINVAL
 */
static zx_status_t ath10k_hw_qca6174_enable_pll_clock(struct ath10k* ar) {
    zx_status_t ret;
    int wait_limit;
    uint32_t clk_div_addr, pll_init_addr, speed_addr;
    uint32_t addr, reg_val, mem_val;
    struct ath10k_hw_params* hw;
    const struct ath10k_hw_clk_params* hw_clk;

    hw = &ar->hw_params;

    if (ar->regs->core_clk_div_address == 0 ||
            ar->regs->cpu_pll_init_address == 0 ||
            ar->regs->cpu_speed_address == 0) {
        return ZX_ERR_INVALID_ARGS;
    }

    clk_div_addr = ar->regs->core_clk_div_address;
    pll_init_addr = ar->regs->cpu_pll_init_address;
    speed_addr = ar->regs->cpu_speed_address;

    /* Read efuse register to find out the right hw clock configuration */
    addr = (RTC_SOC_BASE_ADDRESS | EFUSE_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* sanitize if the hw refclk index is out of the boundary */
    if (MS(reg_val, EFUSE_XTAL_SEL) > ATH10K_HW_REFCLK_COUNT) {
        return ZX_ERR_INVALID_ARGS;
    }

    hw_clk = &hw->hw_clk[MS(reg_val, EFUSE_XTAL_SEL)];

    /* Set the rnfrac and outdiv params to bb_pll register */
    addr = (RTC_SOC_BASE_ADDRESS | BB_PLL_CONFIG_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~(BB_PLL_CONFIG_FRAC_MASK | BB_PLL_CONFIG_OUTDIV_MASK);
    reg_val |= (SM(hw_clk->rnfrac, BB_PLL_CONFIG_FRAC) |
                SM(hw_clk->outdiv, BB_PLL_CONFIG_OUTDIV));
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Set the correct settle time value to pll_settle register */
    addr = (RTC_WMAC_BASE_ADDRESS | WLAN_PLL_SETTLE_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~WLAN_PLL_SETTLE_TIME_MASK;
    reg_val |= SM(hw_clk->settle_time, WLAN_PLL_SETTLE_TIME);
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Set the clock_ctrl div to core_clk_ctrl register */
    addr = (RTC_SOC_BASE_ADDRESS | SOC_CORE_CLK_CTRL_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~SOC_CORE_CLK_CTRL_DIV_MASK;
    reg_val |= SM(1, SOC_CORE_CLK_CTRL_DIV);
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Set the clock_div register */
    mem_val = 1;
    ret = ath10k_bmi_write_memory(ar, clk_div_addr, &mem_val,
                                  sizeof(mem_val));
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Configure the pll_control register */
    addr = (RTC_WMAC_BASE_ADDRESS | WLAN_PLL_CONTROL_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val |= (SM(hw_clk->refdiv, WLAN_PLL_CONTROL_REFDIV) |
                SM(hw_clk->div, WLAN_PLL_CONTROL_DIV) |
                SM(1, WLAN_PLL_CONTROL_NOPWD));
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* busy wait (max 1s) the rtc_sync status register indicate ready */
    wait_limit = 100000;
    addr = (RTC_WMAC_BASE_ADDRESS | RTC_SYNC_STATUS_OFFSET);
    do {
        ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
        if (ret != ZX_OK) {
            return ZX_ERR_INVALID_ARGS;
        }

        if (!MS(reg_val, RTC_SYNC_STATUS_PLL_CHANGING)) {
            break;
        }

        wait_limit--;
        zx_nanosleep(zx_deadline_after(ZX_USEC(10)));

    } while (wait_limit > 0);

    if (MS(reg_val, RTC_SYNC_STATUS_PLL_CHANGING)) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Unset the pll_bypass in pll_control register */
    addr = (RTC_WMAC_BASE_ADDRESS | WLAN_PLL_CONTROL_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~WLAN_PLL_CONTROL_BYPASS_MASK;
    reg_val |= SM(0, WLAN_PLL_CONTROL_BYPASS);
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* busy wait (max 1s) the rtc_sync status register indicate ready */
    wait_limit = 100000;
    addr = (RTC_WMAC_BASE_ADDRESS | RTC_SYNC_STATUS_OFFSET);
    do {
        ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
        if (ret != ZX_OK) {
            return ZX_ERR_INVALID_ARGS;
        }

        if (!MS(reg_val, RTC_SYNC_STATUS_PLL_CHANGING)) {
            break;
        }

        wait_limit--;
        zx_nanosleep(zx_deadline_after(ZX_USEC(10)));

    } while (wait_limit > 0);

    if (MS(reg_val, RTC_SYNC_STATUS_PLL_CHANGING)) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* Enable the hardware cpu clock register */
    addr = (RTC_SOC_BASE_ADDRESS | SOC_CPU_CLOCK_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~SOC_CPU_CLOCK_STANDARD_MASK;
    reg_val |= SM(1, SOC_CPU_CLOCK_STANDARD);
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* unset the nopwd from pll_control register */
    addr = (RTC_WMAC_BASE_ADDRESS | WLAN_PLL_CONTROL_OFFSET);
    ret = ath10k_bmi_read_soc_reg(ar, addr, &reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    reg_val &= ~WLAN_PLL_CONTROL_NOPWD_MASK;
    ret = ath10k_bmi_write_soc_reg(ar, addr, reg_val);
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* enable the pll_init register */
    mem_val = 1;
    ret = ath10k_bmi_write_memory(ar, pll_init_addr, &mem_val,
                                  sizeof(mem_val));
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    /* set the target clock frequency to speed register */
    ret = ath10k_bmi_write_memory(ar, speed_addr, &hw->target_cpu_freq,
                                  sizeof(hw->target_cpu_freq));
    if (ret != ZX_OK) {
        return ZX_ERR_INVALID_ARGS;
    }

    return ZX_OK;
}

const struct ath10k_hw_ops qca988x_ops = {
    .set_coverage_class = ath10k_hw_qca988x_set_coverage_class,
};

static int ath10k_qca99x0_rx_desc_get_l3_pad_bytes(struct htt_rx_desc* rxd) {
    ZX_ASSERT(0);
    return 0;
#if 0 // NEEDS PORTING
    return MS(rxd->msdu_end.qca99x0.info1,
              RX_MSDU_END_INFO1_L3_HDR_PAD);
#endif // NEEDS PORTING
}

const struct ath10k_hw_ops qca99x0_ops = {
    .rx_desc_get_l3_pad_bytes = ath10k_qca99x0_rx_desc_get_l3_pad_bytes,
};

const struct ath10k_hw_ops qca6174_ops = {
    .set_coverage_class = ath10k_hw_qca988x_set_coverage_class,
    .enable_pll_clk = ath10k_hw_qca6174_enable_pll_clock,
};
