/*
 * Copyright (c) 2004-2011 Atheros Communications Inc.
 * Copyright (c) 2011-2012 Qualcomm Atheros, Inc.
 * Copyright (c) 2016-2017 Erik Stromdahl <erik.stromdahl@gmail.com>
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

#ifndef _SDIO_H_
#define _SDIO_H_

#define ATH10K_HIF_MBOX_BLOCK_SIZE              256

#define QCA_MANUFACTURER_ID_BASE                GENMASK(11, 8)
#define QCA_MANUFACTURER_ID_AR6005_BASE         0x5
#define QCA_MANUFACTURER_ID_QCA9377_BASE        0x7
#define QCA_SDIO_ID_AR6005_BASE                 0x500
#define QCA_SDIO_ID_QCA9377_BASE                0x700
#define QCA_MANUFACTURER_ID_REV_MASK            0x00FF
#define QCA_MANUFACTURER_CODE                   0x271 /* Qualcomm/Atheros */

#define ATH10K_SDIO_MAX_BUFFER_SIZE             4096 /*Unsure of this constant*/

/* Mailbox address in SDIO address space */
#define ATH10K_HIF_MBOX_BASE_ADDR               0x1000
#define ATH10K_HIF_MBOX_WIDTH                   0x800

#define ATH10K_HIF_MBOX_TOT_WIDTH \
    (ATH10K_HIF_MBOX_NUM_MAX * ATH10K_HIF_MBOX_WIDTH)

#define ATH10K_HIF_MBOX0_EXT_BASE_ADDR          0x5000
#define ATH10K_HIF_MBOX0_EXT_WIDTH              (36 * 1024)
#define ATH10K_HIF_MBOX0_EXT_WIDTH_ROME_2_0     (56 * 1024)
#define ATH10K_HIF_MBOX1_EXT_WIDTH              (36 * 1024)
#define ATH10K_HIF_MBOX_DUMMY_SPACE_SIZE        (2 * 1024)

#define ATH10K_HTC_MBOX_MAX_PAYLOAD_LENGTH \
    (ATH10K_SDIO_MAX_BUFFER_SIZE - sizeof(struct ath10k_htc_hdr))

#define ATH10K_HIF_MBOX_NUM_MAX                 4
#define ATH10K_SDIO_BUS_REQUEST_MAX_NUM         64

#define ATH10K_SDIO_HIF_COMMUNICATION_TIMEOUT_HZ (100 * HZ)

/* HTC runs over mailbox 0 */
#define ATH10K_HTC_MAILBOX                      0
#define ATH10K_HTC_MAILBOX_MASK                 BIT(ATH10K_HTC_MAILBOX)

/* GMBOX addresses */
#define ATH10K_HIF_GMBOX_BASE_ADDR              0x7000
#define ATH10K_HIF_GMBOX_WIDTH                  0x4000

/* Modified versions of the sdio.h macros.
 * The macros in sdio.h can't be used easily with the FIELD_{PREP|GET}
 * macros in bitfield.h, so we define our own macros here.
 */
#define ATH10K_SDIO_DRIVE_DTSX_MASK \
    (SDIO_DRIVE_DTSx_MASK << SDIO_DRIVE_DTSx_SHIFT)

#define ATH10K_SDIO_DRIVE_DTSX_TYPE_B           0
#define ATH10K_SDIO_DRIVE_DTSX_TYPE_A           1
#define ATH10K_SDIO_DRIVE_DTSX_TYPE_C           2
#define ATH10K_SDIO_DRIVE_DTSX_TYPE_D           3

/* SDIO CCCR register definitions */
#define CCCR_SDIO_IRQ_MODE_REG                  0xF0
#define CCCR_SDIO_IRQ_MODE_REG_SDIO3            0x16

#define CCCR_SDIO_DRIVER_STRENGTH_ENABLE_ADDR   0xF2

#define CCCR_SDIO_DRIVER_STRENGTH_ENABLE_A      0x02
#define CCCR_SDIO_DRIVER_STRENGTH_ENABLE_C      0x04
#define CCCR_SDIO_DRIVER_STRENGTH_ENABLE_D      0x08

#define CCCR_SDIO_ASYNC_INT_DELAY_ADDRESS       0xF0
#define CCCR_SDIO_ASYNC_INT_DELAY_MASK          0xC0

/* mode to enable special 4-bit interrupt assertion without clock */
#define SDIO_IRQ_MODE_ASYNC_4BIT_IRQ            BIT(0)
#define SDIO_IRQ_MODE_ASYNC_4BIT_IRQ_SDIO3      BIT(1)

#define ATH10K_SDIO_TARGET_DEBUG_INTR_MASK      0x01

/* The theoretical maximum number of RX messages that can be fetched
 * from the mbox interrupt handler in one loop is derived in the following
 * way:
 *
 * Let's assume that each packet in a bundle of the maximum bundle size
 * (HTC_HOST_MAX_MSG_PER_BUNDLE) has the HTC header bundle count set
 * to the maximum value (HTC_HOST_MAX_MSG_PER_BUNDLE).
 *
 * in this case the driver must allocate
 * (HTC_HOST_MAX_MSG_PER_BUNDLE * HTC_HOST_MAX_MSG_PER_BUNDLE) skb's.
 */
#define ATH10K_SDIO_MAX_RX_MSGS \
    (HTC_HOST_MAX_MSG_PER_BUNDLE * HTC_HOST_MAX_MSG_PER_BUNDLE)

#define ATH10K_FIFO_TIMEOUT_AND_CHIP_CONTROL   0x00000868u
#define ATH10K_FIFO_TIMEOUT_AND_CHIP_CONTROL_DISABLE_SLEEP_OFF 0xFFFEFFFF
#define ATH10K_FIFO_TIMEOUT_AND_CHIP_CONTROL_DISABLE_SLEEP_ON 0x10000

struct ath10k_sdio_bus_request {
    struct list_head list;

    /* sdio address */
    uint32_t address;

    struct sk_buff* skb;
    enum ath10k_htc_ep_id eid;
    int status;
    /* Specifies if the current request is an HTC message.
     * If not, the eid is not applicable an the TX completion handler
     * associated with the endpoint will not be invoked.
     */
    bool htc_msg;
    /* Completion that (if set) will be invoked for non HTC requests
     * (htc_msg == false) when the request has been processed.
     */
    struct completion* comp;
};

struct ath10k_sdio_rx_data {
    struct sk_buff* skb;
    size_t alloc_len;
    size_t act_len;
    enum ath10k_htc_ep_id eid;
    bool part_of_bundle;
    bool last_in_bundle;
    bool trailer_only;
    int status;
};

struct ath10k_sdio_irq_proc_regs {
    uint8_t host_int_status;
    uint8_t cpu_int_status;
    uint8_t error_int_status;
    uint8_t counter_int_status;
    uint8_t mbox_frame;
    uint8_t rx_lookahead_valid;
    uint8_t host_int_status2;
    uint8_t gmbox_rx_avail;
    uint32_t rx_lookahead[2];
    uint32_t rx_gmbox_lookahead_alias[2];
};

struct ath10k_sdio_irq_enable_regs {
    uint8_t int_status_en;
    uint8_t cpu_int_status_en;
    uint8_t err_int_status_en;
    uint8_t cntr_int_status_en;
};

struct ath10k_sdio_irq_data {
    /* protects irq_proc_reg and irq_en_reg below.
     * We use a mutex here and not a spinlock since we will have the
     * mutex locked while calling the sdio_memcpy_ functions.
     * These function require non atomic context, and hence, spinlocks
     * can be held while calling these functions.
     */
    struct mutex mtx;
    struct ath10k_sdio_irq_proc_regs* irq_proc_reg;
    struct ath10k_sdio_irq_enable_regs* irq_en_reg;
};

struct ath10k_mbox_ext_info {
    uint32_t htc_ext_addr;
    uint32_t htc_ext_sz;
};

struct ath10k_mbox_info {
    uint32_t htc_addr;
    struct ath10k_mbox_ext_info ext_info[2];
    uint32_t block_size;
    uint32_t block_mask;
    uint32_t gmbox_addr;
    uint32_t gmbox_sz;
};

struct ath10k_sdio {
    struct sdio_func* func;

    struct ath10k_mbox_info mbox_info;
    bool swap_mbox;
    uint32_t mbox_addr[ATH10K_HTC_EP_COUNT];
    uint32_t mbox_size[ATH10K_HTC_EP_COUNT];

    /* available bus requests */
    struct ath10k_sdio_bus_request bus_req[ATH10K_SDIO_BUS_REQUEST_MAX_NUM];
    /* free list of bus requests */
    struct list_head bus_req_freeq;
    /* protects access to bus_req_freeq */
    spinlock_t lock;

    struct ath10k_sdio_rx_data rx_pkts[ATH10K_SDIO_MAX_RX_MSGS];
    size_t n_rx_pkts;

    struct ath10k* ar;
    struct ath10k_sdio_irq_data irq_data;

    /* temporary buffer for BMI requests */
    uint8_t* bmi_buf;

    wait_queue_head_t irq_wq;

    bool is_disabled;

    struct workqueue_struct* workqueue;
    struct work_struct wr_async_work;
    struct list_head wr_asyncq;
    /* protects access to wr_asyncq */
    spinlock_t wr_async_lock;
};

static inline struct ath10k_sdio* ath10k_sdio_priv(struct ath10k* ar) {
    return (struct ath10k_sdio*)ar->drv_priv;
}

#endif
