#ifndef _LL_PHY_H_
#define _LL_PHY_H_

#define BTXEN_ON_TIME_1M    (0x2C0)
#define BRXEN_ON_TIME_1M    (0x2C0)
#define BTXEN_ON_TIME_2M    (0x2A0)
#define BRXEN_ON_TIME_2M    (0x2A0)

#define ADV_BTXEN_ON_TIME   (0x2A0)
#define ADV_BRXEN_ON_TIME   (0x2A0)

#define DTM_BTXEN_ON_TIME   (0x2C0)
#define DTM_BRXEN_ON_TIME   (0x2C0)


#define RD_ACC_ADDR         (0x0823)
#define RD_MODE_ADDR        (0x1E02)

#define IFS_RX_LEN_1M       (0xAF0)
#define IFS_TX_LEN_1M       (0x892)
#define IFS_RX_LEN_2M       (0xAF0)
#define IFS_TX_LEN_2M       (0x8F0)


#define LL_REG_WRITE(ADDR, VALUE) (*((uint32 *)(0x40010000+(ADDR<<2))) = (uint32)(VALUE))
#define LL_REG_READ(ADDR) *((volatile uint32 *)(0x40010000+(ADDR<<2)))

#define CCM_REG_WRITE(ADDR, VALUE) (*((uint32 *)(0x40000000+(ADDR<<2))) = (uint32)(VALUE))
#define CCM_REG_READ(ADDR) *((volatile uint32 *)(0x40000000+(ADDR<<2)))


extern void ll_phy_init(void);
extern void ll_phy_config(uint32 rx_ccm_addr, uint32 tx_ccm_addr, uint32 itv_us, uint16 tx_header, uint32 acccode, uint8 chidx, uint8 mode, uint32 crc_init, uint8 phy_cfg);
extern void ll_phy_lsn_bgn(uint32 time_val);
extern void ll_phy_get_sch_timer_count(uint32 * sch_timer_val_us, uint32 * ll_timer_val_16m, uint32 * ifs_timer);
extern uint32 ll_get_llc_control_ltc_en(void);
extern void ll_phy_update_reload_cnt(uint32 reload_val);
extern uint8 ll_phy_get_llc_mode(void);
extern uint16 ll_phy_get_llc_rx_header(void);
extern void ll_phy_stop_tx(void);
extern void ll_phy_update_part_adv_para(uint32 addr, uint16 tx_header);
extern uint16 ll_phy_get_llc_tx_header(void);
extern void ll_phy_set_idle(void);
extern void ll_phy_dtm_config(uint32 itv_us, uint16 tx_header);
extern void ll_scan_phy_config(void);
extern void ll_init_phy_config(void);
extern void ll_conn_get_rx_addr(uint32 *ctrl_rx_ptr, uint32 *data_rx_ptr);
extern void ll_conn_mst_phy_config(uint32 ctrl_rx_ptr, uint32 data_rx_ptr, uint32 tx_ptr, uint16 tx_header, uint8 is_event_end);
extern void ll_phy_doze(void);
//extern void ll_phy_trig_next_rx(void);
extern void ll_phy_stop_trx_state_machine(void);
extern void ll_phy_ccm_aes128(uint32 * key, uint32 * data_in, uint32 * data_out);
extern void ll_phy_ccm_config(uint8 idx, uint8 is_event_closed);
extern void ll_phy_ccm_confg_rx_at_tx_end(void);
extern uint8 ll_phy_ccm_mic_status(void);
extern void ll_conn_slv_rx_phy_config(uint32 ctrl_rx_ptr, uint32 data_rx_ptr);
extern void ll_conn_slv_tx_phy_config(uint32 tx_ptr, uint16 tx_header);
extern uint32 ll_phy_get_anchor_delta(void);
extern uint32 ll_phy_printf_mic(void);
extern uint8 ll_phy_ccm_done_status(void);
extern void ll_phy_ccm_set_bypass_mode(void);
extern void ll_conn_update_phy_ccm_para(uint16 tx_header, uint32 tx_buff);
extern uint32 ll_get_sch_timer_cnt(void);
extern void ll_phy_debug_trig(void);
extern void ll_phy_ccm_config_revert(uint8 idx);
extern void ll_phy_set_ltc_timer_cycles(uint32 val);
extern void ll_phy_ltc_trig_cfg(uint32 val);
extern void ll_phy_set_slp_length(uint32 count);
extern uint32 ll_phy_get_slp_count(void);
extern void ll_conn_slv_tx_ccm_config(void);
extern void ll_phy_ccm_debug_trig(void);
extern void ll_phy_set_fake_acccode(void);
extern void ll_phy_set_mode(uint8 mode);
extern uint32 ll_phy_get_lsn_bgn(void);
extern void ll_phy_update_config(uint8 mode);

#endif

