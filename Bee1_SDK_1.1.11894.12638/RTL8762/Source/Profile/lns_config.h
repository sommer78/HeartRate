#ifndef _LNS_CONFIG_H_
#define _LNS_CONFIG_H_

#define LNS_POSITION_QUALITY_CHAR_SUPPORT
#define LNS_LN_CONTROL_POINT_CHAR_SUPPORT
#define LNS_LN_NAVIGATION_CHAR_SUPPORT



#define LNS_CP_PARA_NAME_OF_ROUTE_MAX_LEN                   10

#define LNS_LN_FEATURE_VALUE_INDEX                          2//read
#define LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX               4//notify
#define LNS_LN_POSITION_QUALITY_VALUE_INDEX                 7//read
#define LNS_LN_CP_VALUE_INDEX                               9 //write, indicate 
#define LNS_LN_NAVIGATION_VALUE_INDEX                       12//notify


#define LNS_LN_LOCATION_AND_SPEED_CCCD_INDEX                5
#define LNS_LN_CP_CCCD_INDEX                                10
#define LNS_LN_NAVIGATION_CCCD_INDEX                        13

#endif
