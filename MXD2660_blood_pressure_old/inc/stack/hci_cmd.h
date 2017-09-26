/**
****************************************************************************
* @file     hci_cmd.h
* @brief
* @author   xiaoyu
* @version  V0.0.0
* @date     2017/06/29
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#ifndef __HCI_CMD_H__
#define __HCI_CMD_H__

/*
0 3 slot packets 0 0
1 5 slot packets 0 1
2Encryption 02
3 Slot offset 0 3
4 Timing accuracy 0 4
5 Role switch 0 5
6 Hold mode 0 6
7 Sniff mode 0 7
8 Park state 1 0
9 Power control requests 1 1
10 Channel quality driven data rate (CQDDR) 1 2
11 SCO link 1 3
12 HV2 packets 1 4
13 HV3 packets 1 5
14 ¦Ì-law log synchronous data 1 6
15 A-law log synchronous data 1 7
16 CVSD synchronous data 2 0
17 Paging parameter negotiation 2 1
18 Power control 2 2
19 Transparent synchronous data 2 3
20 Flow control lag (least significant bit) 2 4
21 Flow control lag (middle bit) 2 5
22 Flow control lag (most significant bit) 2 6
23 Broadcast Encryption 2 7
24 Reserved 3 0
25 Enhanced Data Rate ACL 2 Mbps mode 3 1
26 Enhanced Data Rate ACL 3 Mbps mode 3 2
27 Enhanced inquiry scan 3 3
28 Interlaced inquiry scan 3 4
29 Interlaced page scan 3 5
30 RSSI with inquiry results 3 6
31 Extended SCO link (EV3 packets) 3 7
32 EV4 packets 4 0
33 EV5 packets 4 1
34 Reserved 4 2
35 AFH capable slave  4 3
36 AFH classification slave 4 4
37 BR/EDR Not Supported 4 5
38 LE Supported (Controller) 4 6
39 3-slot Enhanced Data Rate ACL packets 4 7
40 5-slot Enhanced Data Rate ACL packets 5 0
41 Sniff subrating 5 1
42 Pause encryption 5 2
43 AFH capable master  5 3
44 AFH classification master 5 4
45 Enhanced Data Rate eSCO 2 Mbps mode 5 5
46 Enhanced Data Rate eSCO 3 Mbps mode 5 6
47 3-slot Enhanced Data Rate eSCO packets 5 7
48 Extended Inquiry Response 6 0
49 Simultaneous LE and BR/EDR to Same Device Capable (Controller) 6 1
50 Reserved 6 2
51 Secure Simple Pairing 6 3
52 Encapsulated PDU 6 4
53 Erroneous Data Reporting 6 5
54 Non-flushable Packet Boundary Flag 6 6
55 Reserved 6 7
56 Link Supervision Timeout Changed Event 7 0
57 Inquiry TX Power Level 7 1
58 Enhanced Power Control 7 2
59 Reserved 7 3
60 Reserved 7 4
61 Reserved 7 5
62 Reserved 7 6
63 Extended features 7 7
*/
extern uint16 hci_cmd_dispatch(uint8 * data, uint16 opcode, uint16 payload_length, uint16 tail_length);

#endif


