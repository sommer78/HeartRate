#ifndef _HIDS_CONFIG_H
#define _HIDS_CONFIG_H

#define RMC_WITH_VOICE                              0

#define _SUPPORT_NEXUS_7_VOICE

#define HIDS_MOUSE_REPORT_ID                        3
#define HIDS_KB_REPORT_ID                           1

#ifdef _SUPPORT_NEXUS_7_VOICE
#define RMC_VENDOR_REPORT_ID_1                      0xfd
#define RMC_VENDOR_REPORT_ID_2                      0x5A // input and output
#else
#define RMC_VENDOR_REPORT_ID_1                      0xfc  // input
#define RMC_VENDOR_REPORT_ID_2                      0xfb  // output
#endif

#endif
