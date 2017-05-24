
#ifndef _RMC_CONFIG_H
#define _RMC_CONFIG_H

#define RMC_WITH_VOICE 1

#define _SUPPORT_NEXUS_7_VOICE

#define HIDS_KB_REPORT_ID                      1
#define HIDS_MOUSE_REPORT_ID                   2
#if MULTIMEDIA_KEYBOARD
#define HIDS_MM_KB_REPORT_ID                   3
#endif


#ifdef _SUPPORT_NEXUS_7_VOICE
#define RMC_VENDOR_REPORT_ID_1                0xfd
#if SBC_ENC_EN
#define RMC_VENDOR_REPORT_ID_2                0x5B // input and output
#else
#define RMC_VENDOR_REPORT_ID_2                0x5A // input and output
#endif

#else
#define RMC_VENDOR_REPORT_ID_1                0xfc  // input
#define RMC_VENDOR_REPORT_ID_2                0xfb  // output
#endif

#endif
