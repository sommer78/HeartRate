#ifndef _CSC_CONFIG_H_
#define _CSC_CONFIG_H_

#define CSC_MULTIPLE_SENSOR_LOCATIONS_SUPPORT
#define CSC_SC_CONTROL_POINT_SUPPORT

/** @brief  Index of each characteristic in CSC service database. */
#define GATT_SVC_CSC_MEASUREMENT_INDEX      2
#define GATT_SVC_CSC_FEATURE_INDEX          5
#define GATT_SVC_CSC_SENS_LOC_INDEX         7
#define GATT_SVC_CSC_CTL_PNT_INDEX          9
#define GATT_SVC_CSC_MEAS_CCCD_INDEX        (GATT_SVC_CSC_MEASUREMENT_INDEX + 1)
#define GATT_SVC_CSC_CTL_PNT_CCCD_INDEX     (GATT_SVC_CSC_CTL_PNT_INDEX + 1)

#endif
