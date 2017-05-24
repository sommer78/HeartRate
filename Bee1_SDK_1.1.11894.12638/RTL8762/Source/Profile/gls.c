enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************

  * @file     gls.c
  * @brief    glucose service source file.
  * @details  Interface to access the glucose service.
  * @author   bill
  * @date     2015-6-8
  * @version  v1.0
  * *************************************************************************************
  */


#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "profileApi.h"
#include "gls.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**  Value of glucose feature characteristic. */
static uint16_t  Features;

/** glucose record access control point */
TGLCRACP GlcRACP;
PTPatientRecord pNewRecord;
/** flags of notification and indication for characteristic */
static TGLCNotifyIndicateFlag GLCNotifyIndicateFlag={0};

/** parameters used for splitting report records procedure */
uint16_t GlsNumRecordsToReport = 0;
uint16_t GlsCurrentRecordToReport = 0;
uint16_t GlsReportOffset = 0;
uint8_t GlsSendDataFlag = 0;
/** Flag used for aborting procedure */
bool GlsAbortFlag = FALSE;
bool GlsAbortByAppFlag = FALSE;

/**  Function pointer used to send event to application from GLS. Initiated in GLS_AddService. */
static pfnAPPHandleInfoCB_t pfnGLSCB = NULL;

/** @brief  profile/service definition.  */
const TAttribAppl GLSAttTbl[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_GLUCOSE),      /* service UUID */
            HI_WORD(GATT_UUID_GLUCOSE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- Glucose Measurement characteristic value ---*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_GLC_MEASUREMENT),
            HI_WORD(GATT_UUID_CHAR_GLC_MEASUREMENT)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
    
#if GLC_MEASUREMENT_CONTEXT_SUPPORT 
    ,
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY,                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- Glucose Measurement Context characteristic value ---*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_GLC_MEASUREMENT_CONTEXT),
            HI_WORD(GATT_UUID_CHAR_GLC_MEASUREMENT_CONTEXT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
#endif  /**< end of GLC_MEASUREMENT_CONTEXT_SUPPORT */
    
    ,
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- Glucose Features characteristic value ---*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_GLC_FEATURE),
            HI_WORD(GATT_UUID_CHAR_GLC_FEATURE),
        },
        2,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }

    ,
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                   /* characteristic properties */
            GATT_CHAR_PROP_INDICATE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- Glucose Record Access Control Point value ---*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_GLC_RACP),
            HI_WORD(GATT_UUID_CHAR_GLC_RACP)
        },
        0,                                          /* bValueLen, 0 : variable length */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ                  /* wPermissions */
    },
    /* client characteristic configuration */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
};

/**
  * @brief Prepare the records database for the new record, should be invoked before inserting a new record to the database.
  *
  * @return None
  */
void GLS_PrepareNewRecord()
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_PrepareNewRecord  Database head: %d, tail: %d, num: %d, seq_num: %d\n", 4, GlcRACP.record_db.head, GlcRACP.record_db.tail, GlcRACP.record_db.record_num, GlcRACP.record_db.seq_num);
    pNewRecord = &(GlcRACP.record_db.records[GlcRACP.record_db.tail]);
    memset(pNewRecord, 0, sizeof(TPatientRecord));
    GlcRACP.record_db.seq_num++;
    pNewRecord->glc_measurement_value.seq_num = GlcRACP.record_db.seq_num;
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    pNewRecord->glc_measurement_context.seq_num = GlcRACP.record_db.seq_num;
#endif 
}

/**
  * @brief Update the records database, should be invoked after inserting a new record to the database.
  *
  * @return None
  */
void GLS_PushNewRecord()
{
    if (((GlcRACP.record_db.head - GlcRACP.record_db.tail + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE) == 1)
    {
        GlcRACP.record_db.head = (GlcRACP.record_db.head + 1) % GLC_RACP_DATABASE_SIZE;
        GlcRACP.record_db.tail = (GlcRACP.record_db.tail + 1) % GLC_RACP_DATABASE_SIZE;
    }
    else
    {
        GlcRACP.record_db.tail = (GlcRACP.record_db.tail + 1) % GLC_RACP_DATABASE_SIZE;
        GlcRACP.record_db.record_num++;
    }
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_PushNewRecord  Database head: %d, tail: %d, num: %d, seq_num: %d\n", 4, GlcRACP.record_db.head, GlcRACP.record_db.tail, GlcRACP.record_db.record_num, GlcRACP.record_db.seq_num);
}

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref GLS_Application_Parameters
  * @param[in] length                value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    case GLS_PARAM_GLC_FEATURES:
        if (len == 2)
            Features = LE_EXTRN2WORD((uint8_t *) value_ptr);
        else
            ret = FALSE;
        break;
    case GLS_PARAM_CTL_PNT_PROG_CLR:
        GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_RESERVED;
        GlsNumRecordsToReport = 0;
        GlsCurrentRecordToReport = 0;
        GlsReportOffset = 0;
        GlsSendDataFlag = 0;
        GlsAbortFlag = FALSE; // Make sure Abort Flag is clear after RACP procedure is over!
        GlsAbortByAppFlag = FALSE; // Make sure Abort by App Flag is clear after RACP procedure is over!
        break;
    case GLS_PARAM_GLC_MS_FLAG:
        if (len == 1)
            memcpy(&(pNewRecord->glc_measurement_value.flags), value_ptr, 1);
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_BASE_TIME:
        if (len == sizeof(TIMESTAMP))
            memcpy(pNewRecord->glc_measurement_value.base_time, value_ptr, sizeof(TIMESTAMP));
        else
            ret = FALSE;
        break;
#if (GLC_INCLUDE_TIME_OFFSET)
    case GLS_PARAM_GLC_MS_TIME_OFFSET:
        if (len == 2)
        {
            pNewRecord->glc_measurement_value.time_offset = *(int16_t *)value_ptr;
            pNewRecord->glc_measurement_value.flags.time_offset = 1;
        }
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_CONC_TS_LOC)
    case GLS_PARAM_GLC_MS_CONCENTRATION:
        if (len == sizeof(SFLOAT))
        {
            memcpy(&(pNewRecord->glc_measurement_value.concentration), value_ptr, sizeof(SFLOAT));
            pNewRecord->glc_measurement_value.flags.con_ts_loc = 1;
        }
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_CONCENTRATION_UNITS:
        if (len == 1)
            pNewRecord->glc_measurement_value.flags.con_units = *(uint8_t *)value_ptr;
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_TYPE_SAMPLE_LOCATION:
        if (len == 1)
            pNewRecord->glc_measurement_value.type_sample_location = *(uint8_t *)value_ptr;
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_SS_ANNUNC)
    case GLS_PARAM_GLC_MS_SENSOR_STATUS_ANNUNCIATION:
        if (len == 2)
        {
            memcpy(&(pNewRecord->glc_measurement_value.ss_annunciation), value_ptr, 2);
            pNewRecord->glc_measurement_value.flags.ss_annuciation = 1;
        }
        else
            ret = FALSE;
        break;
#endif
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    case GLS_PARAM_GLC_MS_CT_FLAG:
        if (len == 1)
            pNewRecord->glc_measurement_context.flags = *(TGLCMsrCtxtFlag *)value_ptr;
        else
            ret = FALSE;
        break;
#if (GLC_INCLUDE_CARBOHYDRATE)
    case GLS_PARAM_GLC_MS_CT_CARBOHYDRATE_ID:
        if (len == 1)
        {
            pNewRecord->glc_measurement_context.carbohydrate_ID = *(uint8_t *)value_ptr;
            pNewRecord->glc_measurement_context.flags.carbohydrate = 1;
        }
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_CT_CARBOHYDRATE:
        if (len == sizeof(SFLOAT))
            memcpy(&(pNewRecord->glc_measurement_context.carbohydrate),value_ptr,sizeof(SFLOAT));
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_MEAL)
    case GLS_PARAM_GLC_MS_CT_MEAL:
        if (len == 1)
        {
            pNewRecord->glc_measurement_context.meal = *(uint8_t *)value_ptr;
            pNewRecord->glc_measurement_context.flags.meal = 1;
        }
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_TESTER_HEALTH)
    case GLS_PARAM_GLC_MS_CT_TESTER_HEALTH:
        if (len == 1)
        {
            pNewRecord->glc_measurement_context.tester_health = *(uint8_t *)value_ptr;
            pNewRecord->glc_measurement_context.flags.tester_health = 1;
        }
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_EXCERCISE)
    case GLS_PARAM_GLC_MS_CT_EXERCISE_DURATION:
        if (len == 2)
        {
            pNewRecord->glc_measurement_context.exercise_duration = LE_EXTRN2WORD((uint8_t *) value_ptr);
            pNewRecord->glc_measurement_context.flags.excercise = 1;
        }
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_CT_EXERCISE_INTENSITY:
        if (len == 1)
        {
            pNewRecord->glc_measurement_context.exercise_intensity = *(uint8_t *)value_ptr;
        }
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_MEDICATION)
    case GLS_PARAM_GLC_MS_CT_MEDICATION_ID:
        if (len == 1)
        {
            pNewRecord->glc_measurement_context.medication_ID = *(uint8_t *)value_ptr;
            pNewRecord->glc_measurement_context.flags.medication = 1;
        }
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_CT_MEDICATION:
        if (len == sizeof(SFLOAT))
            memcpy(&(pNewRecord->glc_measurement_context.medication),value_ptr,sizeof(SFLOAT));
        else
            ret = FALSE;
        break;
    case GLS_PARAM_GLC_MS_CT_MEDICATION_UNITS:
        if (len == 1)
            pNewRecord->glc_measurement_context.flags.medication_units = *(uint8_t *)value_ptr;
        else
            ret = FALSE;
        break;
#endif
#if (GLC_INCLUDE_HbA1c)
    case GLS_PARAM_GLC_MS_CT_HbA1c:
        if (len == sizeof(SFLOAT))
        {
            memcpy(&(pNewRecord->glc_measurement_context.HbA1c),value_ptr,sizeof(SFLOAT));
            pNewRecord->glc_measurement_context.flags.HbA1c = 1;
        }
        else
            ret = FALSE;
        break;
#endif
#endif      
    default:
        ret = FALSE;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_SetParameter  GLS parameter set failed: %d\n", 1, param_type);

    return ret;
}

/**
  * @brief  Get service related data from application.
  *
  * @param[in] param_type            parameter type to get: @ref GLS_Application_Parameters
  * @param[out] length                value length to be get.
  * @param[out] value_ptr             value to get.
  * @return parameter get result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_GetParameter( uint8_t param_type, uint8_t *len, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    case GLS_PARAM_GLC_FEATURES:
        *len = sizeof(uint16_t);
        LE_WORD2EXTRN(value_ptr, Features);
        break;
    case GLS_PARAM_RECORD_NUM:
        *len = sizeof(int);
        memcpy(value_ptr, &GlcRACP.record_db.record_num, sizeof(int));
        break;
    case GLS_PARAM_RECORD_SEQ_NUM:
        *len = sizeof(uint16_t);
        memcpy(value_ptr, &GlcRACP.record_db.seq_num, sizeof(uint16_t));
        break;
    default:
        *len = 0;
        ret = FALSE;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_GetParameter  GLS parameter get failed: %d\n", 1, param_type);

    return ret;
}

/**
  * @brief Notify glucose measurement value to client.
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] index             Index of the record in the database, denoting the relative position of the record.
  * @return Result of notify.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_GlcMeasurementNotify( uint8_t ServiceId, int index )
{
    int current = (GlcRACP.record_db.head + index) % GLC_RACP_DATABASE_SIZE;
    
    GlsSendDataFlag = 1;
    
    PGLCMeasurementValue pmeasurement_value = &GlcRACP.record_db.records[current].glc_measurement_value;
    uint8_t temp_glc_measurement[sizeof(TGLCMeasurementValue)];
    uint8_t offset = 0;
    if (index >= GlcRACP.record_db.record_num)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_GlcMeasurementNotify  Glucose Measurement Value Access Overflow!", 0 );
        return FALSE;
    }
    if (current == GlcRACP.record_db.tail)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_GlcMeasurementNotify  Glucose Measurement DataBase: Empty!", 0 );
        return FALSE;
    }
    
    memcpy(&temp_glc_measurement[offset], &pmeasurement_value->flags, 1);
    offset += 1;
    memcpy(&temp_glc_measurement[offset], &pmeasurement_value->seq_num, 2);
    offset += 2;
    memcpy(&temp_glc_measurement[offset], pmeasurement_value->base_time, 7);
    offset += 7;

#if (GLC_INCLUDE_TIME_OFFSET)    
    if (pmeasurement_value->flags.time_offset)
    {
        memcpy(&temp_glc_measurement[offset], &pmeasurement_value->time_offset, 2);
        offset += 2;
    }
#endif
#if (GLC_INCLUDE_CONC_TS_LOC)
    if (pmeasurement_value->flags.con_ts_loc)
    {
        memcpy(&temp_glc_measurement[offset], pmeasurement_value->concentration, 2);
        offset += 2;
        memcpy(&temp_glc_measurement[offset], &pmeasurement_value->type_sample_location, 1);
        offset += 1;
    }
#endif
#if (GLC_INCLUDE_SS_ANNUNC)
    if (pmeasurement_value->flags.ss_annuciation)
    {
        memcpy(&temp_glc_measurement[offset], &pmeasurement_value->ss_annunciation, 2);
        offset += 2;
    }
#endif
   DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_GlcMeasurementNotify  Glucose Measurement Notification: index = %d \n", 1, index);
    // send notification to client
    return ProfileAPI_SendData(ServiceId, GLS_CHAR_GLC_MEASUREMENT_INDEX, temp_glc_measurement, offset);
}

#if GLC_MEASUREMENT_CONTEXT_SUPPORT
/**
  * @brief Notify glucose measurement context value to client.
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] index             Index of the record in the database, denoting the relative position of the record.
  * @return Result of notify.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_GlcMeasurementContextNotify(uint8_t ServiceId, int index)
{
    int current = (GlcRACP.record_db.head + index) % GLC_RACP_DATABASE_SIZE;
    
    GlsSendDataFlag = 2;
    
    PGLCMeasurementContext pmeasurement_context = &GlcRACP.record_db.records[current].glc_measurement_context;
    uint8_t temp_glc_measurement_ctxt[sizeof(TGLCMeasurementContext)];
    uint8_t offset = 0;
    if (index >= GlcRACP.record_db.record_num)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_GlcMeasurementContextNotify  Glucose Measurement Context Access Overflow!", 0 );
        return FALSE;
    }
    if (current == GlcRACP.record_db.tail)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_GlcMeasurementContextNotify  Glucose Measurement DataBase: Empty!", 0 );
        return FALSE;
    }
    
    memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->flags, 1);
    offset += 1;
    memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->seq_num, 2);
    offset += 2;
#if (GLC_INCLUDE_EXT_FLAGS)    
    if (pmeasurement_context->flags.ext_flags)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->ext_flags, 1);
        offset += 1;
    }
#endif 
#if (GLC_INCLUDE_CARBOHYDRATE)    
    if (pmeasurement_context->flags.carbohydrate)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->carbohydrate_ID, 1);
        offset += 1;
        memcpy(&temp_glc_measurement_ctxt[offset], pmeasurement_context->carbohydrate, 2);
        offset += 2;
    }
#endif
#if (GLC_INCLUDE_MEAL)
    if (pmeasurement_context->flags.meal)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->meal, 1);
        offset += 1;
    }
#endif
#if (GLC_INCLUDE_TESTER_HEALTH)
    if (pmeasurement_context->flags.tester_health)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->tester_health, 1);
        offset += 1;
    }
#endif
#if (GLC_INCLUDE_EXCERCISE)
    if (pmeasurement_context->flags.excercise)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->exercise_duration, 2);
        offset += 2;
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->exercise_intensity, 1);
        offset += 1;
    }
#endif
#if (GLC_INCLUDE_MEDICATION)
    if (pmeasurement_context->flags.medication)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], &pmeasurement_context->medication_ID, 1);
        offset += 1;
        memcpy(&temp_glc_measurement_ctxt[offset], pmeasurement_context->medication, 2);
        offset += 2;
    }
#endif
#if (GLC_INCLUDE_HbA1c)
    if (pmeasurement_context->flags.HbA1c)
    {
        memcpy(&temp_glc_measurement_ctxt[offset], pmeasurement_context->HbA1c, 2);
        offset += 2;
    }
#endif
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_GlcMeasurementContextNotify  Glucose Measurement Context Notification: index = %d \n", 1, index);
    // send notification to client
    return ProfileAPI_SendData(ServiceId, GLS_CHAR_GLC_MEASUREMENT_CONTEXT_INDEX, temp_glc_measurement_ctxt, offset);
}
#endif

/**
  * @brief Indicate glucose racp procedure result to client.
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] rsp_code          racp respondence code.
  * @return Result of indication.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_RACPResponse( uint8_t service_ID, uint8_t rsp_code)
{
    bool ret;
    
    GlsSendDataFlag = 3;
    
    uint16_t attrib_index = GLS_CHAR_GLC_RACP_INDEX;

    GlcRACP.ctrl_point.operand[0] = GlcRACP.ctrl_point.op_code;
    GlcRACP.ctrl_point.operand[1] = rsp_code;
    GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_RESP_CODE;
    GlcRACP.ctrl_point.op = GLC_RACP_OPERATOR_NULL;

    GlcRACP.cp_length = sizeof(TGlcCtrlPointOpcode) + sizeof(TGlcCtrlPointOperator) + sizeof(TGlcCtrlPointOpcode) + sizeof(TGlcCtrlPointRespCodes);

    // send indication to client
    ret = ProfileAPI_SendData(service_ID, attrib_index, (uint8_t *) &GlcRACP.ctrl_point, GlcRACP.cp_length);
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_RACPResponse  Glucose RACP Resp: %d \n", 1, rsp_code);
    
    //GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_RESERVED;
        
    return ret;
}

/**
  * @brief Indicate number of glucose records to client.
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] num               Number of glucose records.
  * @return Result of indication.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_RACPNumResponse( uint8_t service_ID, uint16_t num)
{
    bool ret;
    
    GlsSendDataFlag = 3;
    
    uint16_t attrib_index = GLS_CHAR_GLC_RACP_INDEX;

    LE_WORD2EXTRN(GlcRACP.ctrl_point.operand, num);
    GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_NBR_OF_RECS_RESP;
    GlcRACP.ctrl_point.op = GLC_RACP_OPERATOR_NULL;

    GlcRACP.cp_length = sizeof(TGlcCtrlPointOpcode) + sizeof(TGlcCtrlPointOperator) + sizeof(uint16_t);

    // send indication to client
    ret = ProfileAPI_SendData(service_ID, attrib_index, (uint8_t *) &GlcRACP.ctrl_point, GlcRACP.cp_length);
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_RACPNumResponse  Glucose RACP Num Response: %d \n", 1, num);
    
    //GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_RESERVED;
    
    return ret;
}

/**
  * @brief Indication in abort procedure.
  *
  * @param[in] ServiceId         Service ID to notify.
  * @return Result of indication.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool GLS_AbortSuccessResponse(uint8_t serviceID)
{
    GlcRACP.ctrl_point.op_code = GLC_RACP_OPCODE_ABORT_OPERATION;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_AbortSuccessResponse  GLS RACP procedure has been aborted!!!", 0);
    
    return GLS_RACPResponse(serviceID, GLC_RACP_RESP_SUCCESS);
}

/**
  * @brief Compare two timestamps.
  *
  * @param[in] time1         Time to compare.
  * @param[in] time2         Time to compare.
  * @return Result of comparison.
  * @retval 1   Time1 > Time2
  * @retval 0   Time1 = Time2
  * @retval -1  Time1 < Time2
  */
int TimeCmp(const TIMESTAMP time1, const TIMESTAMP time2)
{
    uint16_t year1,year2;
    year1 = LE_EXTRN2WORD((uint8_t *) time1);
    year2 = LE_EXTRN2WORD((uint8_t *) time2);
    if (year1 < year2)
    {
        return -1;
    }
    else if (year1 > year2)
    {
        return 1;
    }
    else
    {
        int length;
        for (length = 0; length <5; length++)
        {
            if (time1[2+length] < time2[2+length])
                return -1;
            else if (time1[2+length] > time2[2+length])
                return 1;
            else
                ;
        }
        return 0;
    }
}

/**
  * @brief Calculate the User-Face Time by adding BaseTime and TimeOffset.
  *
  * @param[in] time_in          the pointer of BaseTime
  * @param[in] time_offset      the TimeOffset in minute
  * @param[out] time_out        the pointer of User-Face Time
  * @return None
  */
void UserFaceTime(TIMESTAMP time_in, int16_t time_offset, TIMESTAMP time_out)
{
    uint8_t carry;
    uint16_t year;
    year = LE_EXTRN2WORD((uint8_t *) time_in);      
    
    TIMESTAMP time_tmp = {0};
    time_tmp[4] = time_offset / 60; //hour
    time_tmp[5] = time_offset % 60; //minute
    
    time_out[6] = time_in[6] + time_tmp[6]; //second
    carry = 0;
    
    time_out[5] = time_in[5] + time_tmp[5] + carry; //minute
    if(time_out[5] > 59)
    {
        time_out[5] -= 60;
        carry = 1;
    }
    else
    {
        carry =0;
    }
    
    time_out[4] = time_in[4] + time_tmp[4] + carry; //hour
    if(time_out[4] > 23)
    {
        time_out[4] -= 24;
        carry = 1;
    }
    else
    {
        carry = 0;
    }
    
    time_out[3] = time_in[3] + time_tmp[3] + carry; //day
    switch (time_out[2])
    {
        case 2:
            if(((year % 400) == 0) || (((year % 4) == 0) &&((year % 100) != 0))) //leap year
            {
                if(time_out[3] > 29)
                {
                    time_out[3] -= 29;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }
            else
            {
                if(time_out[3] > 28)
                {
                    time_out[3] -= 28;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(time_out[3] > 31)
            {
                time_out[3] -= 31;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if(time_out[3] > 30)
            {
                time_out[3] -= 30;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            break;
        default:
            break;
    }
    
    time_out[2] = time_in[2] + time_tmp[2] + carry; //month
    if (time_out[2] > 12)
    {
        time_out[2] -= 12;
        carry = 1;
    }
    else
    {
        carry = 0;
    }
    
    LE_WORD2EXTRN(time_out, LE_EXTRN2WORD((uint8_t *) time_in) + LE_EXTRN2WORD((uint8_t *) time_tmp) + carry); //year
}

/**
  * @brief Check the write parameters of RACP, used in report/report num/delete opcode
  * @returen None
  */
TGlcCtrlPointRespCodes GLS_RACPcheck()
{
    TGlcCtrlPointRespCodes ret = GLC_RACP_RESP_SUCCESS;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_RACPcheck  Glucose RACP: opcode = %d, operator = %d, length = %d \n", 3, GlcRACP.ctrl_point.op_code, GlcRACP.ctrl_point.op, GlcRACP.cp_length);
    
    if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_NULL)
    {
        ret = GLC_RACP_RESP_INVALID_OPERATOR;
    }
    else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_ALL_RECS 
            || GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_FIRST
            || GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LAST)
    {
        if (GlcRACP.cp_length != 2)
            ret = GLC_RACP_RESP_INVALID_OPERAND;
    }
    else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LT_EQ 
            || GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_GT_EQ
            || GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_RANGE)
    {
        if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_RESERVED)
        {
            ret = GLC_RACP_RESP_INVALID_OPERAND;
        }
        else if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_SEQ_NBR)
        {
            if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_RANGE)
            {
                uint16_t min_seq = LE_EXTRN2WORD (&GlcRACP.ctrl_point.operand[1]);
                uint16_t max_seq = LE_EXTRN2WORD (&GlcRACP.ctrl_point.operand[1+sizeof(uint16_t)]);
                if (min_seq > max_seq)
                    ret = GLC_RACP_RESP_INVALID_OPERAND;
            }
        }
        else if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_TIME)
        {
            if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_RANGE)
            {
                if (TimeCmp(&GlcRACP.ctrl_point.operand[1], &GlcRACP.ctrl_point.operand[1+sizeof(TIMESTAMP)])>0)
                    ret = GLC_RACP_RESP_INVALID_OPERAND;
            }
        }
        else
        {
            /** GlcRACP.ctrl_point.operand[0] > GLC_RACP_FILTER_TYPE_TIME */
            ret = GLC_RACP_RESP_OPERAND_NOT_SUPPORTED;
        }
    }
    else 
    {
        /** GlcRACP.ctrl_point.operator > GLC_RACP_OPERATOR_LAST */
        ret = GLC_RACP_RESP_OPERATOR_NOT_SUPPORTED;
    }
    return ret;
}

/**
  * @brief Find record by sequence number
  * @param[in] op           operator of the procedure.
  * @param[in] set_seq      sequence number referenced to.
  * @return Absolute position of the found record
  */
int GLS_FindRecordsBySeqNum(TGlcCtrlPointOperator op, uint16_t set_seq)
{
    int find;
    if (op == GLC_RACP_OPERATOR_LT_EQ)
    {
        find = GlcRACP.record_db.head;
        while ((find != GlcRACP.record_db.tail) && (GlcRACP.record_db.records[find].glc_measurement_value.seq_num <= set_seq))
        {
            find = (find + 1) % GLC_RACP_DATABASE_SIZE;
        }
        if (find == GlcRACP.record_db.head) /**< no record found */
            find = GlcRACP.record_db.tail;
        else
            find -= 1; /**< record large than set_seq is found */
    }
    else if (op == GLC_RACP_OPERATOR_GT_EQ)
    {
        find = GlcRACP.record_db.head;
        while ((find != GlcRACP.record_db.tail) && (GlcRACP.record_db.records[find].glc_measurement_value.seq_num < set_seq))
        {
            find = (find + 1) % GLC_RACP_DATABASE_SIZE;
        }
    }
    else
    {
        find = GlcRACP.record_db.tail;
    }
    return find;
}

/**
  * @brief Find record by timestamp
  * @brief Should time_offset be taken into consideration when comparing user-interface time?
  *     PTS test shows that when writing the RACP using the User Facing Time filter type there is no timeoffset part. 
  * @param[in] op           operator of the procedure.
  * @param[in] set_time     time referenced to.
  * @return Absolute position of the found record
  */
int GLS_FindRecordsByTime(TGlcCtrlPointOperator op, TIMESTAMP set_time)
{
    int find;
    //TIMESTAMP user_time;
    if (op == GLC_RACP_OPERATOR_LT_EQ)
    {
        find = GlcRACP.record_db.head;
        while (find != GlcRACP.record_db.tail)
        {
            //UserFaceTime(GlcRACP.record_db.records[find].glc_measurement_value.base_time, TimeOffset, user_time);
            //UserFaceTime(GlcRACP.record_db.records[find].glc_measurement_value.base_time, GlcRACP.record_db.records[find].glc_measurement_value.time_offset, user_time);
            //if (TimeCmp(user_time, set_time) <= 0)
            if (TimeCmp(GlcRACP.record_db.records[find].glc_measurement_value.base_time, set_time) <= 0)
            {
                find = (find + 1) % GLC_RACP_DATABASE_SIZE;
            }
            else
                break;
        }
        if (find == GlcRACP.record_db.head) /**< no record found */
            find = GlcRACP.record_db.tail;
        else
            find -= 1; /**< record large than set_seq is found */
    }
    else if (op == GLC_RACP_OPERATOR_GT_EQ)
    {
        find = GlcRACP.record_db.head;
        while (find != GlcRACP.record_db.tail)
        {
            //UserFaceTime(GlcRACP.record_db.records[find].glc_measurement_value.base_time, TimeOffset, user_time);
            //UserFaceTime(GlcRACP.record_db.records[find].glc_measurement_value.base_time, GlcRACP.record_db.records[find].glc_measurement_value.time_offset, user_time);
            //if (TimeCmp(user_time, set_time) < 0)
            if (TimeCmp(GlcRACP.record_db.records[find].glc_measurement_value.base_time, set_time) < 0)
            {
                find = (find + 1) % GLC_RACP_DATABASE_SIZE;
            }
            else
                break;
        }
    }
    else
    {
        find = GlcRACP.record_db.tail;
    }
    return find;
}

/**
  * @brief Find records that meets the conditions.
  * @param[out] pnum    num of records
  * @param[out] pfirst  absolute position of the first record
  * @param[out] plast   absolute position of the last record
  * @return The check result of the procedure.
  */
TGlcCtrlPointRespCodes GLS_FindRecords(uint16_t *pnum, int *pfirst, int *plast)
{
    uint16_t num_of_records;
    int find1, find2;
    TGlcCtrlPointRespCodes ret;
    ret = GLS_RACPcheck();
    if (ret == GLC_RACP_RESP_SUCCESS)
    {
        if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_ALL_RECS)
        {
            find1 = GlcRACP.record_db.head;
            find2 = (GlcRACP.record_db.tail - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
            num_of_records = GlcRACP.record_db.record_num;
        }
        else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LT_EQ)
        {
            find1 = GlcRACP.record_db.head;
            if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_SEQ_NBR)
            {
                find2 = GLS_FindRecordsBySeqNum(GLC_RACP_OPERATOR_LT_EQ, LE_EXTRN2WORD(&GlcRACP.ctrl_point.operand[1]));
            }
            else if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_TIME)
            {
                find2 = GLS_FindRecordsByTime(GLC_RACP_OPERATOR_LT_EQ, &GlcRACP.ctrl_point.operand[1]);
            }
            else
            {
                find2 = GlcRACP.record_db.tail;
            }
            if (find2 == GlcRACP.record_db.tail)
                num_of_records = 0;
            else
                num_of_records = (find2 - GlcRACP.record_db.head + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE + 1;
        }
        else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_GT_EQ)
        {
            if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_SEQ_NBR)
            {
                find1 = GLS_FindRecordsBySeqNum(GLC_RACP_OPERATOR_GT_EQ, LE_EXTRN2WORD(&GlcRACP.ctrl_point.operand[1]));
            }
            else if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_TIME)
            {
                find1 = GLS_FindRecordsByTime(GLC_RACP_OPERATOR_GT_EQ, &GlcRACP.ctrl_point.operand[1]);
            }
            else
            {
                find1 = GlcRACP.record_db.tail;
            }
            find2 = (GlcRACP.record_db.tail - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
            num_of_records = (GlcRACP.record_db.tail - find1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
        }
        else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_RANGE)
        {
            if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_SEQ_NBR)
            {
                find1 = GLS_FindRecordsBySeqNum(GLC_RACP_OPERATOR_GT_EQ, LE_EXTRN2WORD(&GlcRACP.ctrl_point.operand[1]));
                find2 = GLS_FindRecordsBySeqNum(GLC_RACP_OPERATOR_LT_EQ, LE_EXTRN2WORD(&GlcRACP.ctrl_point.operand[1+2]));
            }
            else if (GlcRACP.ctrl_point.operand[0] == GLC_RACP_FILTER_TYPE_TIME)
            {
                find1 = GLS_FindRecordsByTime(GLC_RACP_OPERATOR_GT_EQ, &GlcRACP.ctrl_point.operand[1]);
                find2 = GLS_FindRecordsByTime(GLC_RACP_OPERATOR_LT_EQ, &GlcRACP.ctrl_point.operand[1+sizeof(TIMESTAMP)]);
            }
            else
            {
                find1 = GlcRACP.record_db.tail;
                find2 = GlcRACP.record_db.tail;
            }
            if ( find1 == GlcRACP.record_db.tail || find2 == GlcRACP.record_db.tail)
                num_of_records = 0;
            else
                num_of_records = (find2 - find1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE + 1;
        }
        else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_FIRST)
        {
            find1 = GlcRACP.record_db.head;
            find2 = find1;
            if (GlcRACP.record_db.record_num == 0)
                num_of_records = 0;
            else
                num_of_records = 1; 
        }
        else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LAST)
        {
            find1 = (GlcRACP.record_db.tail - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
            find2 = find1;
            if (GlcRACP.record_db.record_num == 0)
                num_of_records = 0;
            else
                num_of_records = 1; 
        }
        else
        {
            ret = GLC_RACP_RESP_OPERATOR_NOT_SUPPORTED;
        }
        
        if (pnum != 0)
        {
            *pnum = num_of_records;
        }
        if (pfirst != 0)
        {
            *pfirst = find1;
        }
        if (plast != 0)
        {
            *plast = find2;
        }
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_FindRecords  Glucose Find Records: num = %d, start = %d, end = %d\n", 3, num_of_records, find1, find2);
    }
    return ret;
}

/**
  * @brief Report number of records that meet the conditions.
  * @param[in] service_ID   The service ID of glucose service
  * @return None
  */
void GLS_ReportNumOfRecords(uint8_t service_ID)
{
    TGlcCtrlPointRespCodes ret;
    uint16_t num_of_records;
    int find1, find2;
    ret = GLS_FindRecords(&num_of_records, &find1, &find2);
    if (ret != GLC_RACP_RESP_SUCCESS)
    {
        GLS_RACPResponse(service_ID, ret);  
    }
    else
    {
        GLS_RACPNumResponse(service_ID, num_of_records);
    }
}

/**
  * @brief Report record
  * @param[in] service_ID   The service ID of glucose service
  * @param[in] index        The index of record
  * Attention: For the purpose of good user experience, the input parameter index of this function is the offset of the head, not the pointer of data record.
  */
void GLS_ReportRecord(uint8_t service_ID, int index)
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_ReportRecord: %d\n", 1, index);
    GLS_GlcMeasurementNotify(service_ID, index);
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    int current = (GlcRACP.record_db.head + index) % GLC_RACP_DATABASE_SIZE;
    if (GlcRACP.record_db.records[current].glc_measurement_value.flags.ctxt_info_follows == 1)
    {
        GLS_GlcMeasurementContextNotify(service_ID, index);
    }
#endif
}

/**
  * @brief Report records that meet the conditions.
  * @param[in] service_ID   The service ID of glucose service
  * @return None
  */
void GLS_ReportRecords(uint8_t service_ID)
{
    TGlcCtrlPointRespCodes ret;
    uint16_t num_of_records;
    int find1, find2;
    ret = GLS_FindRecords(&num_of_records, &find1, &find2);
    if (ret == GLC_RACP_RESP_SUCCESS)
    {
        if (num_of_records == 0)
        {
            ret = GLC_RACP_RESP_NO_RECS_FOUND;
            GLS_RACPResponse(service_ID, ret);
        }
        else 
        {
//            int offset = (find1 - GlcRACP.record_db.head + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
//            for (int index = 0; index < num_of_records; index++)
//            {
//                GLS_ReportRecord(service_ID, offset + index); // attention! 
//                VoidCheckAbortFlag();
//            }
            GlsReportOffset = (find1 - GlcRACP.record_db.head + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
            GlsNumRecordsToReport = num_of_records;
            GlsCurrentRecordToReport = 0;
            if (FALSE == GLS_ReportRecordsTask(service_ID))
            {
                GLS_SetParameter(GLS_PARAM_CTL_PNT_PROG_CLR, 0, NULL);
            }
        }
    }
    else
    {
        GLS_RACPResponse(service_ID, ret);
    }
}

/**
  * @brief Sub task of GLS report records procedure, should be invoked after previous data has been send to make sure that flow control and abort procedure is correctly executed.
  *
  * @param[in] service_ID         Service ID to notify.
  * @return result of report records
  * @retval TRUE success
  * @retval False failed
  */
bool GLS_ReportRecordsTask(uint8_t service_ID)
{
    bool ret = TRUE;
    if(GlsAbortFlag == TRUE)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_ReportRecordsTask  Glucose current record = %d, total = %d, procedure abort successfully!\n", 2, GlsCurrentRecordToReport, GlsNumRecordsToReport);
        GlsCurrentRecordToReport = GlsNumRecordsToReport; // stop transmitting any data
        ret = GLS_AbortSuccessResponse(service_ID);
        GlsAbortFlag = FALSE;
        GlsAbortByAppFlag = FALSE; // Abort procedure first, prior to abort by application
        return ret;
    }

    if(GlsAbortByAppFlag == TRUE)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_ReportRecordsTask  Glucose current record = %d, total = %d, procedure abort by app successfully!\n", 2, GlsCurrentRecordToReport, GlsNumRecordsToReport);
        GlsCurrentRecordToReport = GlsNumRecordsToReport; // stop transmitting any data
        ret = GLS_RACPResponse(service_ID, GLC_RACP_RESP_PROC_NOT_COMPLETED);
        GlsAbortByAppFlag = FALSE;
        return ret;
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_ReportRecordsTask  Glucose report records, current = %d, total = %d, GlsSendDataFlag = %d\n", 3, GlsCurrentRecordToReport, GlsNumRecordsToReport, GlsSendDataFlag);

    if(GlsSendDataFlag == 1)
    {
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
        int current = (GlcRACP.record_db.head + GlsReportOffset + GlsCurrentRecordToReport) % GLC_RACP_DATABASE_SIZE;
        if (GlcRACP.record_db.records[current].glc_measurement_value.flags.ctxt_info_follows == 1)
        {
            ret = GLS_GlcMeasurementContextNotify(service_ID, GlsReportOffset + GlsCurrentRecordToReport);
        }
#endif    
        GlsCurrentRecordToReport += 1;
        if (GlsSendDataFlag == 2) // Glucose measurement context has been sent.
        {
            return ret;
        }
    }
    
    if (GlsCurrentRecordToReport < GlsNumRecordsToReport)
    {
        ret = GLS_GlcMeasurementNotify(service_ID, GlsReportOffset + GlsCurrentRecordToReport);
    }
    else
    {
        if(GlsSendDataFlag != 3)
        {
            ret = GLS_RACPResponse(service_ID, GLC_RACP_RESP_SUCCESS);
        }
        else // clear control point
        {
            GLS_SetParameter(GLS_PARAM_CTL_PNT_PROG_CLR, 0, NULL);
        }
    }
    return ret;
}

/**
  * @brief Delete records that meet the conditions.
  * @param[in] service_ID   The service ID of glucose service
  * @return None
  */
void GLS_DeleteRecords(uint8_t service_ID)
{
    TGlcCtrlPointRespCodes ret;
    uint16_t num_of_records;
    int find1, find2;
    ret = GLS_FindRecords(&num_of_records, &find1, &find2);
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_DeleteRecords  Glucose delete records: num = %d, start = %d, end = %d\n", 3, num_of_records, find1, find2);
    if (ret == GLC_RACP_RESP_SUCCESS)
    {
        if (num_of_records >0)
        {
            if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_ALL_RECS)
            {
                GlcRACP.record_db.head = GlcRACP.record_db.tail;
                GlcRACP.record_db.record_num = 0;
            }
            else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LT_EQ)
            {
                GlcRACP.record_db.head = (find2 + 1) % GLC_RACP_DATABASE_SIZE;
                GlcRACP.record_db.record_num -= num_of_records;
            }
            else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_GT_EQ)
            {
                GlcRACP.record_db.tail = find1;
                GlcRACP.record_db.record_num -= num_of_records;
            }
            else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_RANGE)
            {
                int front = (find1 - GlcRACP.record_db.head + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                int back = (GlcRACP.record_db.tail - find2 - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                int num2move, move_from, move_to, loop;
                if (front > back)
                {
                    num2move = back;
                    move_from = (find2 + 1) % GLC_RACP_DATABASE_SIZE;
                    move_to = find1;
                    for (loop = 0; loop < num2move; loop++)
                    {
                        GlcRACP.record_db.records[move_to] = GlcRACP.record_db.records[move_from];
                        move_from = (move_from + 1) % GLC_RACP_DATABASE_SIZE;
                        move_to = (move_to + 1) % GLC_RACP_DATABASE_SIZE;
                    }
                    GlcRACP.record_db.tail = (GlcRACP.record_db.tail - num_of_records + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                    GlcRACP.record_db.record_num -= num_of_records;
                }
                else
                {
                    num2move = front;
                    move_from = (find1 - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                    move_to = find2;
                    for (loop = 0; loop < num2move; loop++)
                    {
                        GlcRACP.record_db.records[move_to] = GlcRACP.record_db.records[move_from];
                        move_from = (move_from - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                        move_to = (move_to - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                    }
                    GlcRACP.record_db.head = (GlcRACP.record_db.head + num_of_records) % GLC_RACP_DATABASE_SIZE;
                    GlcRACP.record_db.record_num -= num_of_records;
                }
            }
            else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_FIRST)
            {
                GlcRACP.record_db.head = (GlcRACP.record_db.head + 1) % GLC_RACP_DATABASE_SIZE;
                GlcRACP.record_db.record_num -= 1;
            }
            else if (GlcRACP.ctrl_point.op == GLC_RACP_OPERATOR_LAST)
            {
                GlcRACP.record_db.tail = (GlcRACP.record_db.tail - 1 + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE;
                GlcRACP.record_db.record_num -= 1;
            }
            else
            {
                ret = GLC_RACP_RESP_OPERAND_NOT_SUPPORTED;
            }
        }
        else
        {
            ret = GLC_RACP_RESP_NO_RECS_FOUND;
        }
    }
    GLS_RACPResponse(service_ID, ret);
}

/**
  * @brief read characteristic data from service.
  *
  * @param ServiceId          ServiceID of characteristic data.
  * @param iAttribIndex       Attribute index of getting characteristic data.
  * @param iOffset            Used for Blob Read.
  * @param piLength           length of getting characteristic data.
  * @param ppValue            data got from service.
  * @return Profile procedure result
  */
TProfileResult  GlsAttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, 
                              uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue )
{
    TProfileResult wCause = ProfileResult_Success;
    TGLS_CALLBACK_DATA GLS_upsteam_msg;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GlsAttrReadCb  iAttribIndex = %d iOffset %x", 2, iAttribIndex, iOffset);
    *piLength = 0;
    switch ( iAttribIndex )
    {
        case GLS_CHAR_GLC_FEATURE_INDEX:
        {
            /* Notify Application. */
            if (pfnGLSCB)
            {
                GLS_upsteam_msg.msg_data.read_value_index = GLS_EVT_READ_FEATURE;
                GLS_upsteam_msg.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                pfnGLSCB(ServiceId, (void*)&GLS_upsteam_msg);
            }
            *ppValue = (uint8_t *) &Features;
            *piLength = sizeof(Features);
        }
        break;
        default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- GLS_AttrRead, Attr not found, index=%d",
                   1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
    }
    return ( wCause );
}

/**
  * @brief  handle control point write (request).
  *
  * @param serviceID
  * @param write_length      write request data length.
  * @param value_ptr         pointer to write request data.
  * @return none
  * @retval  void
  */
static void  GLS_CtlPntWriteIndPostProc( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    TGLS_CALLBACK_DATA GLS_upsteam_msg;
    /** check if there is any operation running */
    if (write_length > sizeof(TGLCControlPoint))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_CtlPntWriteIndPostProc  GLS Control Point request error: OpCode=0x%x, invalid write length!", 1, value_ptr[0]);
        return;
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLS_CtlPntWriteIndPostProc  Write GLS Control Point: OpCode = %d, operator = %d, length = %d, filter type = %d", 4,
               value_ptr[0], value_ptr[1], write_length, value_ptr[2]);
    
    /* Notify Application. */
    if (pfnGLSCB)
    {
        GLS_upsteam_msg.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
        memset(&GLS_upsteam_msg.msg_data.write, 0, sizeof(TGLCControlPoint));
        memcpy(&GLS_upsteam_msg.msg_data.write, value_ptr, write_length);
        pfnGLSCB(serviceID, (void*)&GLS_upsteam_msg);
    }

    if (value_ptr[0] != GLC_RACP_OPCODE_ABORT_OPERATION)
    {
        memset(&GlcRACP.ctrl_point, 0, sizeof(TGLCControlPoint));
        memcpy(&GlcRACP.ctrl_point, value_ptr, write_length );
        GlcRACP.cp_length = write_length;
        switch (GlcRACP.ctrl_point.op_code)
        {
            case GLC_RACP_OPCODE_REPORT_NBR_OF_RECS:
                GLS_ReportNumOfRecords(serviceID);
                break;
            case GLC_RACP_OPCODE_REPORT_RECS:
                GLS_ReportRecords(serviceID);
                break;
            case GLC_RACP_OPCODE_DELETE_RECS:
                GLS_DeleteRecords(serviceID);
                break;
            default:
                GLS_RACPResponse(serviceID, GLC_RACP_RESP_OPCODE_NOT_SUPPORTED);
                break;
        }
    }
    else
    {
        if ((GlcRACP.ctrl_point.op_code != GLC_RACP_OPCODE_RESERVED) && (GlsSendDataFlag != 3))
        {
            GlsAbortFlag = TRUE;
            //while (GlsAbortFlag == TRUE) ; /* halt until AbortFlag is disabled when procedure in progress has been aborted */
        }
        else
        {
            GLS_AbortSuccessResponse(serviceID);
        }
    }
}

/**
  * @brief Check CCCD configuration 
  * @return Result of check
  * @retval TRUE
  * @retval FALSE
  */
bool GLS_CheckCCCD()
{
    if (0 == GLCNotifyIndicateFlag.GLC_measurement_notify_enable)
    {
        return FALSE;
    }
    else if (0 == GLCNotifyIndicateFlag.GLC_measurement_context_notify_enable)
    {
        return FALSE;
    }
    else if (0 == GLCNotifyIndicateFlag.GLC_RACP_indicate_enable)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/**
  * @brief write characteristic data from service.
  *
  * @param ServiceID          ServiceID to be written.
  * @param iAttribIndex          Attribute index of characteristic.
  * @param wLength            length of value to be written.
  * @param pValue            value to be written.
  * @return Profile procedure result
  */
TProfileResult GlsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause = ProfileResult_Success;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GlsAttrWriteCb  iAttribIndex = %d, wLength = %x", 2, iAttribIndex, wLength);
    switch (iAttribIndex)
    {
        case GLS_CHAR_GLC_RACP_INDEX:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GLSAttrWriteCb opcode: old = %d, new = %d\n", 2, GlcRACP.ctrl_point.op_code, pValue[0]);
            /* Attribute value has variable size, make sure written value size is valid. */
            if ( (wLength > sizeof(TGLCControlPoint)) || (pValue == NULL) )
            {
                wCause = ProfileResult_InvalidValueSize;
            }
            /* Make sure Control Point is not "Process already in progress". */
            else if ( GLC_RACP_OPERATION_ACTIVE( GlcRACP.ctrl_point.op_code ) && (pValue[0] != GLC_RACP_OPCODE_ABORT_OPERATION) )
            {
                wCause = (TProfileResult) (ATT_ERR | GLC_ERR_PROC_ALREADY_IN_PROGRESS);
            }
            /* Make sure Control Point is configured indication enable. */
            else if (!GLS_CheckCCCD())
            {
                wCause = (TProfileResult) (ATT_ERR | GLC_ERR_CCCD_IMPROPERLY_CONFIGURED);
            }
            else
            {
                //wCause = GLS_Hanlde_CtlPntProc(ServiceId, wLength, pValue);
                /** handle RACP request after sending write response */
                *pWriteIndPostProc = GLS_CtlPntWriteIndPostProc;
            }

        }
        break;

        default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLSAttrWriteCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
    }
    return wCause;
}

/**
  * @brief update CCCD bits from stack.
  *
  * @param ServiceId          Service ID.
  * @param Index          Attribute index of characteristic data.
  * @param wCCCBits         CCCD bits from stack.
  * @return None
  */
void GlsCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TGLS_CALLBACK_DATA GLS_upsteam_msg;
    bool bHandle = TRUE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GlsCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    { 
    case GLS_CHAR_GLC_MEASUREMENT_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                GLCNotifyIndicateFlag.GLC_measurement_notify_enable=1;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_MEASUREMENT_NOTIFY_ENABLE;
            }
            else
            {
                // Disable Notification
                GLCNotifyIndicateFlag.GLC_measurement_notify_enable=0;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_MEASUREMENT_NOTIFY_DISABLE;
            }
        }
        break;
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    case GLS_CHAR_GLC_MEASUREMENT_CONTEXT_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                GLCNotifyIndicateFlag.GLC_measurement_context_notify_enable=1;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_MEASUREMENT_CONTEXT_NOTIFY_ENABLE;
            }
            else
            {
                // Disable Notification
                GLCNotifyIndicateFlag.GLC_measurement_context_notify_enable=0;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_MEASUREMENT_CONTEXT_NOTIFY_DISABLE;
            }
        }
        break;
#endif
    case GLS_CHAR_GLC_RACP_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Notification
                GLCNotifyIndicateFlag.GLC_RACP_indicate_enable=1;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_RACP_INDICATE_ENABLE;
            }
            else
            {
                // Disable Notification
                GLCNotifyIndicateFlag.GLC_RACP_indicate_enable=0;
                GLS_upsteam_msg.msg_data.notify_indicate_index = GLS_EVT_GLC_RACP_INDICATE_DISABLE;
            }
        }
        break;      
    default:
        {
            bHandle = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLSCccdUpdateCb  Index = %d not found", 1, Index);
        }
        break;
    }
    /* Notify Application. */
    if (pfnGLSCB && (bHandle == TRUE))
    {
        GLS_upsteam_msg.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
        pfnGLSCB(serviceId, (void*)&GLS_upsteam_msg);
    }
}

/**
  * @brief Simple Profile Service Callbacks.
  */
CONST gattServiceCBs_t GlsCBs =
{
    GlsAttrReadCb,  // Read callback function pointer
    GlsAttrWriteCb, // Write callback function pointer
    GlsCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add BAS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t GLS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)GLSAttTbl,
                                       sizeof(GLSAttTbl),
                                       GlsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GLS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
    GlcRACP.record_db.record_num = 0;
    GlcRACP.record_db.head = 0;
    GlcRACP.record_db.tail = 0;
    GlcRACP.record_db.seq_num = GLC_RACP_INIT_SEQ_NBR_DEFAULT;
    pfnGLSCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

/**
  * @brief Abort RACP procedure by application
  *
  * @return None
  */
void GLS_AbortRACPProcedure(void)
{
    if ((GlcRACP.ctrl_point.op_code != GLC_RACP_OPCODE_RESERVED) && (GlsSendDataFlag != 3)) 
    {
        GlsAbortByAppFlag = TRUE;
    }
}
