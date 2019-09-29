/**************************************************************************************************
 
  Phyplus Microelectronics Limited confidential and proprietary. 
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics 
  Limited ("Phyplus"). Your use of this Software is limited to those 
  specific rights granted under  the terms of the business contract, the 
  confidential agreement, the non-disclosure agreement and any other forms 
  of agreements as a customer or a partner of Phyplus. You may not use this 
  Software unless you agree to abide by the terms of these agreements. 
  You acknowledge that the Software may not be modified, copied, 
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy 
  (BLE) integrated circuit, either as a product or is integrated into your 
  products.  Other than for the aforementioned purposes, you may not use, 
  reproduce, copy, prepare derivative works of, modify, distribute, perform, 
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
  
**************************************************************************************************/


/**
 *  \file appl_sample_example_phylight.c
 *
 *  Source File for Mesh light Server, support SIG model: Generic OnOff, Light HSL, Light CTL, ... 
 *   
 */

/*
 *  Copyright (C) 2018. Phyplus Ltd.
 *  All rights reserved.
 */


/* ----------------------------------------- Header File Inclusion */
#include "MS_common.h"
#include "MS_access_api.h"
#include "MS_config_api.h"
#include "MS_health_server_api.h"
#include "MS_generic_onoff_api.h"
#include "MS_light_hsl_api.h"
#include "MS_light_ctl_api.h"
#include "MS_scene_api.h"

#include "MS_net_api.h"
#include "blebrr.h"
#include "nvsto.h"
#include "model_state_handler_pl.h"

#include "flash.h"

#include "led_light.h"

#define USE_HSL      // enable Light HSL server model
#undef  USE_CTL      // disable Light CTL server model


/* Console Input/Output */
#define CONSOLE_OUT(...)    printf(__VA_ARGS__)
#define CONSOLE_IN(...)     scanf(__VA_ARGS__)

#define VENDOR_PRODUCT_MAC_ADDR         0x4000


void appl_dump_bytes(UCHAR *buffer, UINT16 length);
void appl_mesh_sample (void);

API_RESULT UI_prov_callback
           (
               PROV_HANDLE * phandle,
               UCHAR         event_type,
               API_RESULT    event_result,
               void        * event_data,
               UINT16        event_datalen
           );
void UI_proxy_callback
     (
         NETIF_HANDLE       * handle,
         UCHAR                p_evt,
         UCHAR              * data_param,
         UINT16               data_len
     );
void UI_proxy_start_adv(MS_SUBNET_HANDLE subnet_handle, UCHAR proxy_adv_mode);
void UI_register_prov(void);
void UI_register_proxy(void);
void UI_sample_reinit(void);
void UI_gatt_iface_event_pl_cb
     (
         UCHAR  ev_name,
         UCHAR  ev_param
     );
API_RESULT UI_sample_check_app_key(void);
API_RESULT UI_set_brr_scan_rsp_data (void);
/* --------------------------------------------- Global Definitions */

typedef struct UI_state_scene_struct
{
    UINT32                          index;
    MS_STATE_LIGHT_HSL_STRUCT       light_hsl_state;
    MS_STATE_GENERIC_ONOFF_STRUCT   onoff_state;
} UI_STATE_SCENE_STRUCT;

/* ----------------------------------------- External Global Variables */

/* ----------------------------------------- Exported Global Variables */


/* ----------------------------------------- Static Global Variables */

/* Model Server - Foundation Models */

/* Health Server - Test Routines */
static void UI_health_self_test_00(UINT8 test_id, UINT16 company_id)
{
    MS_IGNORE_UNUSED_PARAM(test_id);
    MS_IGNORE_UNUSED_PARAM(company_id);
}

static void UI_health_self_test_01(UINT8 test_id, UINT16 company_id)
{
    MS_IGNORE_UNUSED_PARAM(test_id);
    MS_IGNORE_UNUSED_PARAM(company_id);
}

static void UI_health_self_test_FF(UINT8 test_id, UINT16 company_id)
{
    MS_IGNORE_UNUSED_PARAM(test_id);
    MS_IGNORE_UNUSED_PARAM(company_id);
}

/* List of Self Tests */
static MS_HEALTH_SERVER_SELF_TEST UI_health_server_self_tests[] =
{
        {
            0x00, /* Test ID: 0x00 */
            UI_health_self_test_00
        },
        {
            0x01, /* Test ID: 0x01 */
            UI_health_self_test_01
        },
        {
            0xFF, /* Test ID: 0xFF */
            UI_health_self_test_FF
        }
};

/**
 * \brief Health Server application Asynchronous Notification Callback.
 *
 * \par Description
 * Health Server calls the registered callback to indicate events occurred to the
 * application.
 *
 * \param handle        Model Handle.
 * \param event_type    Health Server Event type.
 * \param event_param   Parameter associated with the event if any or NULL.
 * \param param_len     Size of the event parameter data. 0 if event param is NULL.
 */
static API_RESULT UI_health_server_cb
                  (
                      MS_ACCESS_MODEL_HANDLE * handle,
                      UINT8                    event_type,
                      UINT8                  * event_param,
                      UINT16                   param_len
                  )
{
    CONSOLE_OUT(
    "Health Server Callback. Not handled. Returning\n");

    return API_SUCCESS;
}

 API_RESULT UI_register_foundation_model_servers
                  (
                      MS_ACCESS_ELEMENT_HANDLE element_handle
                  )
{
    /* Configuration Server */
    MS_ACCESS_MODEL_HANDLE   UI_config_server_model_handle;
    MS_ACCESS_MODEL_HANDLE   UI_health_server_model_handle;
    API_RESULT retval;

    /* Health Server */
    UINT16                       company_id;
    MS_HEALTH_SERVER_SELF_TEST * self_tests;
    UINT32                       num_self_tests;

    CONSOLE_OUT("In Model Server - Foundation Models\n");

    retval = MS_config_server_init(element_handle, &UI_config_server_model_handle);

    CONSOLE_OUT("Config Model Server Registration Status: 0x%04X\n", retval);

    /* Health Server */
    company_id = MS_DEFAULT_COMPANY_ID;
    self_tests = &UI_health_server_self_tests[0];
    num_self_tests = sizeof(UI_health_server_self_tests)/sizeof(MS_HEALTH_SERVER_SELF_TEST);

    retval = MS_health_server_init
             (
                 element_handle,
                 &UI_health_server_model_handle,
                 company_id,
                 self_tests,
                 num_self_tests,
                 UI_health_server_cb
             );

    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT(
        "Health Server Initialized. Model Handle: 0x%04X\n",
        UI_health_server_model_handle);
    }
    else
    {
        CONSOLE_OUT(
        "[ERR] Sensor Server Initialization Failed. Result: 0x%04X\n",
        retval);
    }

    return retval;
}



// ===========================  Generic OnOff Server Model Functions =============
/* ---- Generic OnOff States */
static MS_STATE_GENERIC_ONOFF_STRUCT UI_generic_onoff;

/* Generic OnOff Model state Initialization */
static void UI_generic_onoff_model_states_initialization(void)
{
    EM_mem_set(&UI_generic_onoff, 0, sizeof(UI_generic_onoff));
}

/* Generic OnOff Model Get Handler */
static API_RESULT UI_generic_onoff_model_state_get(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    API_RESULT retval;

    retval = API_SUCCESS;

    switch(state_t)
    {
        case MS_STATE_GENERIC_ONOFF_T:
        {
            MS_STATE_GENERIC_ONOFF_STRUCT * param_p;

            param_p = (MS_STATE_GENERIC_ONOFF_STRUCT *)param;

            /* Ignoring Instance and direction right now */
            *param_p = UI_generic_onoff;
        }
        break;

        default:
        break;
    }

    return retval;
}

/* Generic OnOff Model Set Handler */
static API_RESULT UI_generic_onoff_model_state_set(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    API_RESULT retval;

    retval = API_SUCCESS;
    switch (state_t)
    {
        case MS_STATE_GENERIC_ONOFF_T:
        {
            MS_STATE_GENERIC_ONOFF_STRUCT * param_p;

            param_p = (MS_STATE_GENERIC_ONOFF_STRUCT *)param;

            /* Instantaneous Change */
            UI_generic_onoff.onoff = param_p->onoff;

            *param_p = UI_generic_onoff;
            CONSOLE_OUT("[state] current: 0x%02X\n", UI_generic_onoff.onoff);
            CONSOLE_OUT("[state] target: 0x%02X\n", UI_generic_onoff.target_onoff);
            CONSOLE_OUT("[state] remaining_time: 0x%02X\n", UI_generic_onoff.transition_time);

            generic_onoff_set_pl(param_p->onoff);

            /* Ignoring Instance and direction right now */
        }
        break;

        default:
        break;
    }

    return retval;
}

/* Generic OnOff Model Server */
/**
 * \brief Server Application Asynchronous Notification Callback.
 *
 * \par Description
 * Generic_Onoff server calls the registered callback to indicate events occurred to the application.
 *
 * \param [in] ctx           Context of message received for a specific model instance.
 * \param [in] msg_raw       Uninterpreted/raw received message.
 * \param [in] req_type      Requested message type.
 * \param [in] state_params  Model specific state parameters.
 * \param [in] ext_params    Additional parameters.
 */
static API_RESULT UI_generic_onoff_server_cb
                  (
                      /* IN */ MS_ACCESS_MODEL_REQ_MSG_CONTEXT    * ctx,
                      /* IN */ MS_ACCESS_MODEL_REQ_MSG_RAW        * msg_raw,
                      /* IN */ MS_ACCESS_MODEL_REQ_MSG_T          * req_type,
                      /* IN */ MS_ACCESS_MODEL_STATE_PARAMS       * state_params,
                      /* IN */ MS_ACCESS_MODEL_EXT_PARAMS         * ext_params
                  )
{
    MS_STATE_GENERIC_ONOFF_STRUCT    param;
    MS_ACCESS_MODEL_STATE_PARAMS     current_state_params;
    API_RESULT                       retval;

    retval = API_SUCCESS;

    /* Check message type */
    if (MS_ACCESS_MODEL_REQ_MSG_T_GET == req_type->type)
    {
        CONSOLE_OUT("[GENERIC_ONOFF] GET Request.\n");

        UI_generic_onoff_model_state_get(state_params->state_type, 0, &param, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = &param;

        /* Using same as target state and remaining time as 0 */
    }
    else if (MS_ACCESS_MODEL_REQ_MSG_T_SET == req_type->type)
    {
        CONSOLE_OUT("[GENERIC_ONOFF] SET Request.\n");

        retval = UI_generic_onoff_model_state_set(state_params->state_type, 0, (MS_STATE_GENERIC_ONOFF_STRUCT *)state_params->state, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = (MS_STATE_GENERIC_ONOFF_STRUCT *)state_params->state;
    }

    /* See if to be acknowledged */
    if (0x01 == req_type->to_be_acked)
    {
        CONSOLE_OUT("[GENERIC_ONOFF] Sending Response.\n");

        /* Parameters: Request Context, Current State, Target State (NULL: to be ignored), Remaining Time (0: to be ignored), Additional Parameters (NULL: to be ignored) */
        retval = MS_generic_onoff_server_state_update(ctx, &current_state_params, NULL, 0, NULL);
    }

    return retval;
}
API_RESULT UI_register_generic_onoff_model_server
           (
               MS_ACCESS_ELEMENT_HANDLE element_handle
           )
{
    /* Generic OnOff Server */
    MS_ACCESS_MODEL_HANDLE   UI_generic_onoff_server_model_handle;
    API_RESULT retval;

    CONSOLE_OUT("In Generic OnOff Model Server\n");

    retval = MS_generic_onoff_server_init
             (
                 element_handle,
                 &UI_generic_onoff_server_model_handle,
                 UI_generic_onoff_server_cb
             );

    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT(
        "Generic Onoff Server Initialized. Model Handle: 0x%04X\n",
        UI_generic_onoff_server_model_handle);
    }
    else
    {
        CONSOLE_OUT(
        "[ERR] Generic Onoff Server Initialization Failed. Result: 0x%04X\n",
        retval);
    }

    return retval;
}


// ===================== Light HSL Server Model Functions =====================
/** -- Light - HSL states*/
static MS_STATE_LIGHT_HSL_STRUCT UI_light_hsl;
static MS_STATE_LIGHT_HSL_RANGE_STRUCT UI_light_hsl_range;
static MS_STATE_LIGHT_HSL_DEFAULT_STRUCT UI_light_hsl_default;

void UI_light_hsl_model_states_initialization(void)
{
    /* Light HSL States */
    EM_mem_set (&UI_light_hsl, 0, sizeof(UI_light_hsl));
    EM_mem_set(&UI_light_hsl_range, 0, sizeof(UI_light_hsl_range));
    EM_mem_set(&UI_light_hsl_default, 0, sizeof(UI_light_hsl_default));
}



API_RESULT UI_light_hsl_model_state_get(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    switch(state_t)
    {
        case MS_STATE_LIGHT_HSL_T:
        {
            MS_STATE_LIGHT_HSL_STRUCT * param_p;

            param_p = (MS_STATE_LIGHT_HSL_STRUCT *)param;

            /* Ignoring Instance and direction right now */
            *param_p = UI_light_hsl;
        }
        break;

        default:
        break;
    }

    return API_SUCCESS;
}

void UI_light_hsl_set_actual(UINT16 state_inst, UINT16 lightness, UINT16 hue, UINT16 saturation, UCHAR forced_publish)
{

    light_hsl_set_pl(hue,saturation,lightness);
    
}

API_RESULT UI_light_hsl_model_state_set(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    API_RESULT retval;

    retval = API_SUCCESS;

    switch(state_t)
    {
        case MS_STATE_LIGHT_HSL_T:
        {
            MS_STATE_LIGHT_HSL_STRUCT * param_p;

            param_p = (MS_STATE_LIGHT_HSL_STRUCT *)param;

         
            {
                UI_light_hsl.hsl_lightness = param_p->hsl_lightness;
                UI_light_hsl.hsl_hue = param_p->hsl_hue;
                UI_light_hsl.hsl_saturation = param_p->hsl_saturation;
                UI_light_hsl.tid = param_p->tid;
                /*
                appl_light_hsl[0].hsl_lightness = param_p->hsl_lightness;
                appl_light_hsl[0].hsl_lightness = param_p->hsl_lightness;
                appl_light_hsl[0].hsl_lightness = param_p->hsl_lightness;
                */
                //appl_light_lightness[state_inst].light_lightness_actual.lightness_actual = param_p->hsl_lightness;
            }

            UI_light_hsl_set_actual(0,UI_light_hsl.hsl_lightness,UI_light_hsl.hsl_hue,UI_light_hsl.hsl_saturation,0);
            *param_p = UI_light_hsl;
            CONSOLE_OUT("[state] current Hue: 0x%04X\n", UI_light_hsl.hsl_hue);
            CONSOLE_OUT("[state] current Saturation: 0x%04X\n", UI_light_hsl.hsl_saturation);
            CONSOLE_OUT("[state] current Lightness: 0x%04X\n", UI_light_hsl.hsl_lightness);
            CONSOLE_OUT("[state] target Hue: 0x%04X\n", UI_light_hsl.target_hsl_hue);
            CONSOLE_OUT("[state] target Saturation: 0x%04X\n", UI_light_hsl.target_hsl_saturation);
            CONSOLE_OUT("[state] target Lightness: 0x%04X\n", UI_light_hsl.target_hsl_lightness);
            CONSOLE_OUT("[state] remaining_time: 0x%02X\n", UI_light_hsl.transition_time);

            /* Ignoring Instance and direction right now */
        }
        break;

   

        default:
        break;
    }

    return retval;
}

/**
 * \brief Server Application Asynchronous Notification Callback.
 *
 * \par Description
 * Light_Hsl server calls the registered callback to indicate events occurred to the application.
 *
 * \param [in] ctx           Context of message received for a specific model instance.
 * \param [in] msg_raw       Uninterpreted/raw received message.
 * \param [in] req_type      Requested message type.
 * \param [in] state_params  Model specific state parameters.
 * \param [in] ext_params    Additional parameters.
 */
API_RESULT UI_light_hsl_server_cb
           (
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_CONTEXT    * ctx,
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_RAW        * msg_raw,
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_T          * req_type,
               /* IN */ MS_ACCESS_MODEL_STATE_PARAMS       * state_params,
               /* IN */ MS_ACCESS_MODEL_EXT_PARAMS         * ext_params
           )
{
    MS_ACCESS_MODEL_STATE_PARAMS         current_state_params;
    MS_STATE_LIGHT_HSL_STRUCT            param;
    MS_STATE_LIGHT_HSL_RANGE_STRUCT      param_range;
    MS_STATE_LIGHT_HSL_DEFAULT_STRUCT    param_default;
    void                               * param_p;
    API_RESULT                           retval;

    MS_IGNORE_UNUSED_PARAM(msg_raw);
    MS_IGNORE_UNUSED_PARAM(ext_params);

    retval = API_SUCCESS;

    /* Check message type */
    if (MS_ACCESS_MODEL_REQ_MSG_T_GET == req_type->type)
    {
        CONSOLE_OUT(
        "[LIGHT_HSL] GET Request.\n");

        switch (state_params->state_type)
        {
            case MS_STATE_LIGHT_HSL_T:
            case MS_STATE_LIGHT_HSL_HUE_T:
            case MS_STATE_LIGHT_HSL_SATURATION_T:
            case MS_STATE_LIGHT_HSL_TARGET_T:
            {
                param_p = &param;
            }
            break;

            case MS_STATE_LIGHT_HSL_DEFAULT_T:
            {
                param_p = &param_default;
            }
            break;

            case MS_STATE_LIGHT_HSL_RANGE_T:
            {
                param_p = &param_range;
            }
            break;

            default:
            break;
        }

        UI_light_hsl_model_state_get(state_params->state_type, 0, param_p, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = param_p;
    }
    else if (MS_ACCESS_MODEL_REQ_MSG_T_SET == req_type->type)
    {
        CONSOLE_OUT(
        "[LIGHT_HSL] SET Request.\n");

        UI_light_hsl_model_state_set(state_params->state_type, 0, state_params->state, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = state_params->state;
    }

    /* See if to be acknowledged */
    if (0x01 == req_type->to_be_acked)
    {
        CONSOLE_OUT(
        "[LIGHT_HSL] Sending Response.\n");

        /* Parameters: Request Context, Current State, Target State (NULL: to be ignored), Remaining Time (0: to be ignored), Additional Parameters (NULL: to be ignored) */
        retval = MS_light_hsl_server_state_update(ctx, &current_state_params, NULL, 0, NULL);
    }

    return retval;
}


API_RESULT UI_register_light_hsl_model_server
           (
               MS_ACCESS_ELEMENT_HANDLE element_handle
           )
{
    /* Generic OnOff Server */
    MS_ACCESS_MODEL_HANDLE   UI_light_hsl_server_model_handle;
    MS_ACCESS_MODEL_HANDLE   UI_light_hsl_setup_server_model_handle;
    
    API_RESULT retval;

    retval = MS_light_hsl_server_init
             (
                 element_handle,
                 &UI_light_hsl_server_model_handle,
                 &UI_light_hsl_setup_server_model_handle,
                 UI_light_hsl_server_cb
             );

    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT(
        "Light Hsl Server Initialized. Model Handle: 0x%04X\n",
        UI_light_hsl_server_model_handle);
    }
    else
    {
        CONSOLE_OUT(
        "[ERR] Light Hsl Server Initialization Failed. Result: 0x%04X\n",
        retval);
    }

    return retval;
}

// ==================== Light CTL Server Model Functions ====================
/** -- Light - CTL states*/
static MS_STATE_LIGHT_CTL_STRUCT UI_light_ctl;
void UI_light_ctl_model_states_initialization(void)
{
    /* Light Lightness States */
    EM_mem_set (&UI_light_ctl, 0, sizeof(UI_light_ctl));

}



API_RESULT UI_light_ctl_model_state_get(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    switch(state_t)
    {
        case MS_STATE_LIGHT_CTL_T:
        {
            MS_STATE_LIGHT_CTL_STRUCT * param_p;

            param_p = (MS_STATE_LIGHT_CTL_STRUCT *)param;

            /* Ignoring Instance and direction right now */
            *param_p = UI_light_ctl;
        }
        break;

        default:
        break;
    }

    return API_SUCCESS;
}

void UI_light_ctl_set_actual(UINT16 state_inst, UINT16 lightness, UINT16 temperature, UINT16 deltaUv)
{

    // TODO: convert CTL to RGB and set light
    
}

API_RESULT UI_light_ctl_model_state_set(UINT16 state_t, UINT16 state_inst, void * param, UINT8 direction)
{
    API_RESULT retval;

    retval = API_SUCCESS;

    switch(state_t)
    {
        case MS_STATE_LIGHT_CTL_T:
        {
            MS_STATE_LIGHT_CTL_STRUCT * param_p;

            param_p = (MS_STATE_LIGHT_CTL_STRUCT *)param;

         
            {
                UI_light_ctl.ctl_lightness = param_p->ctl_lightness;
                UI_light_ctl.ctl_temperature = param_p->ctl_temperature;
                UI_light_ctl.ctl_delta_uv = param_p->ctl_delta_uv;
                UI_light_ctl.delay = param_p->delay;
                UI_light_ctl.transition_time = param_p->transition_time;
                UI_light_ctl.target_ctl_lightness = param_p->target_ctl_lightness;
                UI_light_ctl.target_ctl_temperature = param_p->target_ctl_temperature;

                //appl_light_lightness[state_inst].light_lightness_actual.lightness_actual = param_p->hsl_lightness;
            }

            UI_light_ctl_set_actual(0, UI_light_ctl.ctl_lightness, UI_light_ctl.ctl_temperature, UI_light_ctl.ctl_delta_uv);
            *param_p = UI_light_ctl;
            CONSOLE_OUT("[state] current Lightness: 0x%04X\n", UI_light_ctl.ctl_lightness);
            CONSOLE_OUT("[state] current Temperature: 0x%04X\n", UI_light_ctl.ctl_temperature);
            CONSOLE_OUT("[state] current Delta UV: 0x%04X\n",  UI_light_ctl.ctl_delta_uv);

			// below parameters are not set
            CONSOLE_OUT("[state] target Lightness: 0x%04X\n",  UI_light_ctl.target_ctl_lightness);
            CONSOLE_OUT("[state] target Temperature: 0x%04X\n", UI_light_ctl.target_ctl_temperature);
            CONSOLE_OUT("[state] delay time: 0x%04X\n", UI_light_ctl.delay);
            CONSOLE_OUT("[state] remaining_time: 0x%02X\n", UI_light_hsl.transition_time);

            /* Ignoring Instance and direction right now */
        }
        break;
  
        default:
        break;
    }
    
    return retval;
}

/**
 * \brief Server Application Asynchronous Notification Callback.
 *
 * \par Description
 * Light_Hsl server calls the registered callback to indicate events occurred to the application.
 *
 * \param [in] ctx           Context of message received for a specific model instance.
 * \param [in] msg_raw       Uninterpreted/raw received message.
 * \param [in] req_type      Requested message type.
 * \param [in] state_params  Model specific state parameters.
 * \param [in] ext_params    Additional parameters.
 */
API_RESULT UI_light_ctl_server_cb
           (
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_CONTEXT    * ctx,
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_RAW        * msg_raw,
               /* IN */ MS_ACCESS_MODEL_REQ_MSG_T          * req_type,
               /* IN */ MS_ACCESS_MODEL_STATE_PARAMS       * state_params,
               /* IN */ MS_ACCESS_MODEL_EXT_PARAMS         * ext_params
           )
{
    MS_ACCESS_MODEL_STATE_PARAMS                   current_state_params;

    MS_STATE_LIGHT_CTL_STRUCT                      light_ctl_params;
    MS_STATE_LIGHT_CTL_TEMPERATURE_RANGE_STRUCT    light_ctl_temperature_range_params;
    MS_STATE_LIGHT_CTL_DEFAULT_STRUCT              light_ctl_default_params;
    MS_STATE_LIGHT_CTL_TEMPERATURE_STRUCT          light_ctl_temperature_params;
    void                                         * param_p;

    API_RESULT retval;

    retval = API_SUCCESS;

    /* Check message type */
    if (MS_ACCESS_MODEL_REQ_MSG_T_GET == req_type->type)
    {
        CONSOLE_OUT(
        "[LIGHT_CTL] GET Request.\n");

        switch (state_params->state_type)
        {
            case MS_STATE_LIGHT_CTL_DEFAULT_T:
            {
                param_p = &light_ctl_default_params;
            }
            break;

            case MS_STATE_LIGHT_CTL_T:
            {
                param_p = &light_ctl_params;
            }
            break;

            case MS_STATE_LIGHT_CTL_TEMPERATURE_RANGE_T:
            {
                param_p = &light_ctl_temperature_range_params;
            }
            break;

            case MS_STATE_LIGHT_CTL_TEMPERATURE_T:
            {
                param_p = &light_ctl_temperature_params;
            }
            break;

            default:
            break;
        }

        UI_light_ctl_model_state_get(state_params->state_type, 0, param_p, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = param_p;
    }
    else if (MS_ACCESS_MODEL_REQ_MSG_T_SET == req_type->type)
    {
        CONSOLE_OUT(
        "[LIGHT_CTL] SET Request.\n");

        UI_light_ctl_model_state_set(state_params->state_type, 0, state_params->state, 0);

        current_state_params.state_type = state_params->state_type;
        current_state_params.state = state_params->state;
    }

    /* See if to be acknowledged */
    if (0x01 == req_type->to_be_acked)
    {
        CONSOLE_OUT(
        "[LIGHT_CTL] Sending Response.\n");

        /* Parameters: Request Context, Current State, Target State (NULL: to be ignored), Remaining Time (0: to be ignored), Additional Parameters (NULL: to be ignored) */
        retval = MS_light_ctl_server_state_update(ctx, &current_state_params, NULL, 0, NULL);
    }

    return retval;
}


API_RESULT UI_register_light_ctl_model_server
           (
               MS_ACCESS_ELEMENT_HANDLE element_handle
           )
{
    /* Light CTL Server */
    MS_ACCESS_MODEL_HANDLE   UI_light_ctl_server_model_handle;
    MS_ACCESS_MODEL_HANDLE   UI_light_ctl_setup_server_model_handle;
    
    API_RESULT retval;

    retval = MS_light_ctl_server_init
             (
                 element_handle,
                 &UI_light_ctl_server_model_handle,
                 &UI_light_ctl_setup_server_model_handle,
                 UI_light_ctl_server_cb
             );

    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT(
        "Light Ctl Server Initialized. Model Handle: 0x%04X\n",
        UI_light_ctl_server_model_handle);
    }
    else
    {
        CONSOLE_OUT(
        "[ERR] Light Ctl Server Initialization Failed. Result: 0x%04X\n",
        retval);
    }

    return retval;
}

// ==================== Scene Server Model Functions ====================
void UI_scene_model_states_initialization(void)
{
    // nothing now

}

// TODO: update below comments
/**
 * \brief Server Application Asynchronous Notification Callback.
 *
 * \par Description
 * Scene server calls the registered callback to indicate events occurred to the application.
 *
 * \param [in] ctx           Context of message received for a specific model instance.
 * \param [in] msg_raw       Uninterpreted/raw received message.
 * \param [in] req_type      Requested message type.
 * \param [in] state_params  Model specific state parameters.
 * \param [in] ext_params    Additional parameters.
 *
 */
void * UI_scene_server_cb
       (
           /* IN */ MS_ACCESS_MODEL_HANDLE             * handle,
           /* IN */ UINT8                                event_type,
           /* IN */ void                               * event_param,
           /* IN */ UINT16                               event_length,
           /* IN */ void                               * context
       )
{
    void       *param_p;
    UINT32     index;
	UI_STATE_SCENE_STRUCT   *state_p;
	MS_STATE_LIGHT_HSL_STRUCT *state_hsl;

    param_p = NULL;
    (void)handle;

    switch(event_type)
    {
        case MS_SCENE_EVENT_STORE:
        {
            if (event_length != 4)           // UINT32
                return NULL;
            index = *(UINT32 *)event_param;
            printf("store scene, index = %d \r\n", index);
			state_p = (UI_STATE_SCENE_STRUCT *)EM_alloc_mem(sizeof(UI_STATE_SCENE_STRUCT));
			if (NULL == state_p)
            {
                printf("Allocate memory fail, size = %d\r\n", sizeof(UI_STATE_SCENE_STRUCT));
				return NULL;
			}

            // save current states
            state_p->index = index;
			EM_mem_copy((void *)&(state_p->light_hsl_state), (void *)&UI_light_hsl, sizeof(MS_STATE_LIGHT_HSL_STRUCT));
			EM_mem_copy((void *)&(state_p->onoff_state), (void *)&UI_generic_onoff, sizeof(MS_STATE_GENERIC_ONOFF_STRUCT));			
            param_p = (void *)state_p;

        }
        break;

        case MS_SCENE_EVENT_DELETE:
        {
            if (event_length != 4)           // UINT32
                return NULL;            
            index = *(UINT32 *)event_param;
            printf("delete scene, index = %d \r\n", index);
            state_p = (UI_STATE_SCENE_STRUCT *)context;

            if (state_p != NULL  
              && (state_p->index == index)) 
            {
                EM_free_mem(state_p);
                printf("scene %d deleted\r\n", index);
            }
        }
        break;

        case MS_SCENE_EVENT_RECALL_START:
        {
            if (event_length != 4)           // UINT32
                return NULL;            
            index = *(UINT32 *)event_param;
            printf("start recalling scene, index = %d \r\n", index);
            state_p = (UI_STATE_SCENE_STRUCT *)context;

            if (state_p != NULL  
              && (state_p->index == index)) 
            {
                state_hsl = &(state_p->light_hsl_state);
				light_hsl_set_pl(state_hsl->hsl_hue, state_hsl->hsl_saturation, state_hsl->hsl_lightness);
            }
        }
        break;

        case MS_SCENE_EVENT_RECALL_COMPLETE:
        {
            printf("MS_SCENE_EVENT_RECALL_COMPLETE \r\n");            
            if (event_length != 4)           // UINT32
                return NULL;            
            index = *(UINT32 *)event_param;
            printf("complete recalling scene, index = %d \r\n", index);
            state_p = (UI_STATE_SCENE_STRUCT *)context;

            if (state_p != NULL  
              && (state_p->index == index)) 
            {
                // nothing to do here

            }            

        }
        break;

        case MS_SCENE_EVENT_RECALL_IMMEDIATE:
        {
            printf("MS_SCENE_EVENT_RECALL_IMMEDIATE \r\n");
            if (event_length != 4)           // UINT32
                return NULL;            
            index = *(UINT32 *)event_param;
            printf("complete recalling scene, index = %d \r\n", index);
            state_p = (UI_STATE_SCENE_STRUCT *)context;

            if (state_p != NULL  
              && (state_p->index == index)) 
            {
                state_hsl = &(state_p->light_hsl_state);
				light_hsl_set_pl(state_hsl->hsl_hue, state_hsl->hsl_saturation, state_hsl->hsl_lightness);
            }            

        }
        break;
    }

    return param_p;
}


API_RESULT UI_register_scene_model_server
           (
               MS_ACCESS_ELEMENT_HANDLE element_handle
           )
{
    /* Generic OnOff Server */
    MS_ACCESS_MODEL_HANDLE   UI_scene_server_model_handle;
    MS_ACCESS_MODEL_HANDLE   UI_scene_setup_server_model_handle;
    
    API_RESULT retval;

    retval = MS_scene_server_init
             (
                 element_handle,
                 &UI_scene_server_model_handle,
                 &UI_scene_setup_server_model_handle,
                 UI_scene_server_cb
             );

    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT(
        "Scene Server Initialized. Model Handle: 0x%04X\n",
        UI_scene_server_model_handle);
    }
    else
    {
        CONSOLE_OUT(
        "[ERR] Scene Server Initialization Failed. Result: 0x%04X\n",
        retval);
    }

    return retval;
}



/* Model state Initialization */
static void UI_model_states_initialization(void)
{
    /* Generic OnOff States */
    UI_generic_onoff_model_states_initialization();

#ifdef USE_HSL   
    /* Light HSL States */
    UI_light_hsl_model_states_initialization();
#endif

#ifdef USE_CTL    
    /* Light CTL States */
	UI_light_ctl_model_states_initialization();
#endif    
		   
    /* Scene states */
    UI_scene_model_states_initialization();
			   
    //	 /* Vendor Defined States */
    //	 UI_vendor_defined_model_states_initialization();
}

/* Provisionee */
#define UI_PROV_OUTPUT_OOB_ACTIONS            0x00
/** Public Key OOB Flag */
#define UI_PROV_PUBKEY_OOBINFO                0x00

/** Static OOB Flag */
#define UI_PROV_STATIC_OOBINFO                0x00

     

/** Output OOB Maximum size supported */
#define UI_PROV_OUTPUT_OOB_SIZE               0x00

/** Input OOB Actions supported */
#define UI_PROV_INPUT_OOB_ACTIONS             0x00

/** Input OOB Maximum size supported */
#define UI_PROV_INPUT_OOB_SIZE                0x00

/** Beacon setup timeout in seconds */
#define UI_PROV_SETUP_TIMEOUT_SECS            30

/** Attention timeout for device in seconds */
#define UI_PROV_DEVICE_ATTENTION_TIMEOUT      30

#define PROV_AUTHVAL_SIZE_PL                  16

/** Authentication values for OOB Display - To be made random */
#define UI_DISPLAY_AUTH_DIGIT                 3
#define UI_DISPLAY_AUTH_NUMERIC               35007
#define UI_DISPLAY_AUTH_STRING                "F00L"

/** Provisioning capabilities of local device */
DECL_STATIC PROV_CAPABILITIES_S UI_prov_capab =
{
    /** Number of Elements */
    0x01,

    /** Supported algorithms */
    PROV_MASK_ALGO_EC_FIPS_P256,

    /** Public key type */
    UI_PROV_PUBKEY_OOBINFO,

    /** Static OOB type */
    UI_PROV_STATIC_OOBINFO,

    /** Output OOB information */
    { UI_PROV_OUTPUT_OOB_ACTIONS, UI_PROV_OUTPUT_OOB_SIZE },

    /** Input OOB information */
    { UI_PROV_INPUT_OOB_ACTIONS, UI_PROV_INPUT_OOB_SIZE },
};


/** Unprovisioned device identifier */
//DECL_STATIC PROV_DEVICE_S UI_lprov_device =
PROV_DEVICE_S UI_lprov_device =
{
    /** UUID */
    {0x05, 0x04, 0x62, 0x12, 0x00, 0x01, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00},

    /** OOB Flag */
    0x00,

    /**
     * Encoded URI Information
     * For example, to give a web address, "https://www.abc.com"
     * the URI encoded data would be -
     * 0x17 0x2F 0x2F 0x77 0x77 0x77 0x2E 0x61 0x62 0x63 0x2E 0x63 0x6F 0x6D
     * where 0x17 is the URI encoding for https:
     */
    NULL
};

/** Current role of application - Provisioner/Device */
DECL_STATIC UCHAR UI_prov_role;

/** Provisioning Handle */
DECL_STATIC PROV_HANDLE UI_prov_handle;

static API_RESULT UI_prov_callback
                  (
                      PROV_HANDLE * phandle,
                      UCHAR         event_type,
                      API_RESULT    event_result,
                      void        * event_data,
                      UINT16        event_datalen
                  )
{
//    PROV_DEVICE_S * rdev;
//    PROV_CAPABILITIES_S * rcap;
    PROV_DATA_S * rdata;
    PROV_OOB_TYPE_S * oob_info;
    API_RESULT retval;

    UCHAR authstr[PROV_AUTHVAL_SIZE_PL << 1];
    UINT32 authnum;
    UCHAR authtype;
    UCHAR * pauth;
    UINT16 authsize;
//    UCHAR  pdata[(MS_DEVICE_UUID_SIZE * 2) + 1];
//    UCHAR  * t_data;

    switch (event_type)
    {

       case PROV_EVT_PROVISIONING_SETUP:
            CONSOLE_OUT("Recvd PROV_EVT_PROVISIONING_SETUP\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);

            /* Display the attention timeout */
            CONSOLE_OUT("Attention TImeout - %d\n", *((UCHAR *)event_data));

            LIGHT_ONLY_BLUE_ON;
            
            break;

        case PROV_EVT_OOB_DISPLAY:
            CONSOLE_OUT("Recvd PROV_EVT_OOB_DISPLAY\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);

            /* Reference the Authenticatio Type information */
            oob_info = (PROV_OOB_TYPE_S *)event_data;

            CONSOLE_OUT("Authenticaion Action - 0x%02X\n", oob_info->action);
            CONSOLE_OUT("Authenticaion Size - 0x%02X\n", oob_info->size);

            /* If role is Device, the action is of Output OOB, else Input OOB */
            if (PROV_ROLE_DEVICE == UI_prov_role)
            {
                if (PROV_OOOB_ACTION_ALPHANUMERIC == oob_info->action)
                {
                    authtype = 1;
                }
                else if (PROV_OOOB_ACTION_NUMERIC == oob_info->action)
                {
                    authtype = 2;
                }
                else
                {
                    authtype = 0;
                }
            }
            else
            {
                if (PROV_IOOB_ACTION_ALPHANUMERIC == oob_info->action)
                {
                    authtype = 1;
                }
                else if (PROV_IOOB_ACTION_NUMERIC == oob_info->action)
                {
                    authtype = 2;
                }
                else
                {
                    authtype = 0;
                }
            }

            if (1 == authtype)
            {
                EM_str_copy (authstr, UI_DISPLAY_AUTH_STRING);

                CONSOLE_OUT("\n\n>>> AuthVal - %s <<<\n\n", authstr);

                pauth = authstr;
                authsize = (UINT16)EM_str_len(authstr);
            }
            else if (2 == authtype)
            {
                authnum = (UINT32)UI_DISPLAY_AUTH_NUMERIC;

                CONSOLE_OUT("\n\n>>> AuthVal - %d <<<\n\n", authnum);

                pauth = (UCHAR *)&authnum;
                authsize = sizeof(UINT32);
            }
            else
            {
                authnum = (UINT32)UI_DISPLAY_AUTH_DIGIT;

                CONSOLE_OUT("\n\n>>> AuthVal - %d <<<\n\n", authnum);

                pauth = (UCHAR *)&authnum;
                authsize = sizeof(UINT32);
            }

            /* Call to input the oob */
            CONSOLE_OUT("Setting the Authval...\n");
            retval = MS_prov_set_authval(&UI_prov_handle, pauth, authsize);
            CONSOLE_OUT("Retval - 0x%04X\n", retval);
            break;

        case PROV_EVT_OOB_ENTRY:
            CONSOLE_OUT("Recvd PROV_EVT_OOB_ENTRY\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);

            /* Reference the Authenticatio Type information */
            oob_info = (PROV_OOB_TYPE_S *)event_data;

            CONSOLE_OUT("Authenticaion Action - 0x%02X\n", oob_info->action);
            CONSOLE_OUT("Authenticaion Size - 0x%02X\n", oob_info->size);
            break;

        case PROV_EVT_DEVINPUT_COMPLETE:
            CONSOLE_OUT("Recvd PROV_EVT_DEVINPUT_COMPLETE\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);
            break;

        case PROV_EVT_PROVDATA_INFO:
            CONSOLE_OUT("Recvd PROV_EVT_PROVDATA_INFO\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);

            /* Reference the Provisioning Data */
            rdata = (PROV_DATA_S *)event_data;

            CONSOLE_OUT("NetKey  : "); appl_dump_bytes(rdata->netkey, PROV_KEY_NETKEY_SIZE);
            CONSOLE_OUT("Key ID  : 0x%04X\n", rdata->keyid);
            CONSOLE_OUT("Flags   : 0x%02X\n", rdata->flags);
            CONSOLE_OUT("IVIndex : 0x%08X\n", rdata->ivindex);
            CONSOLE_OUT("UAddr   : 0x%04X\n", rdata->uaddr);

            /* Provide Provisioning Data to Access Layer */
            MS_access_cm_set_prov_data
            (
                 rdata
            );

            break;

        case PROV_EVT_PROVISIONING_COMPLETE:
            CONSOLE_OUT("Recvd PROV_EVT_PROVISIONING_COMPLETE\n");
            CONSOLE_OUT("Status - 0x%04X\n", event_result);

            if (API_SUCCESS == event_result)
            {
                /* Already Set while handling PROV_EVT_PROVDATA_INFO */

                /* LED ON/OFF for Provisioning Indication Abstraction Call */
                mesh_model_device_provisioned_ind_pl();
                light_blink_set(LIGHT_GREEN, LIGHT_BLINK_SLOW,3);
            }
            else
            {
                light_blink_set(LIGHT_RED, LIGHT_BLINK_SLOW,3);
            }
            break;

        default:
            CONSOLE_OUT("Unknown Event - 0x%02X\n", event_type);
    }

    return API_SUCCESS;
}

static void UI_register_prov(void)
{
    API_RESULT retval;

    CONSOLE_OUT("Registering with Provisioning layer...\n");
    retval = MS_prov_register(&UI_prov_capab, UI_prov_callback);
    CONSOLE_OUT("Retval - 0x%04X\n", retval);
}


static void UI_setup_prov(UCHAR role, UCHAR brr)
{
    API_RESULT retval;
    
    if (PROV_BRR_GATT == brr)
    {
        blebrr_gatt_mode_set(BLEBRR_GATT_PROV_MODE);
    }
    
    if (PROV_ROLE_PROVISIONER != role)
    {     
        CONSOLE_OUT("Setting up Device for Provisioning ...\n");
              
        retval = MS_prov_setup
                 (
                     brr,
                     role,
                     &UI_lprov_device,
                     UI_PROV_SETUP_TIMEOUT_SECS
                 );

        UI_prov_role = PROV_ROLE_DEVICE;
    }
    else
    {
        CONSOLE_OUT("Setting up Provisioner for Provisioning ...\n");
        retval = MS_prov_setup
                 (
                     brr,
                     role,
                     NULL,
                     UI_PROV_SETUP_TIMEOUT_SECS
                 );

        UI_prov_role = PROV_ROLE_PROVISIONER;
    }

    CONSOLE_OUT("Retval - 0x%04X\n", retval);
}

static void UI_prov_bind(UCHAR brr, UCHAR index)
{
    API_RESULT retval;

    /* Call to bind with the selected device */
    CONSOLE_OUT("Binding with the selected device...\n");
    retval = MS_prov_bind(brr, &UI_lprov_device, UI_PROV_DEVICE_ATTENTION_TIMEOUT, &UI_prov_handle);

    CONSOLE_OUT("Retval - 0x%04X\n", retval);
}


void UI_proxy_start_adv(MS_SUBNET_HANDLE subnet_handle, UCHAR proxy_adv_mode)
{
#ifdef MS_PROXY_SERVER
    API_RESULT retval;
    DECL_STATIC UINT8 first_time = 0;

    if (0 == first_time)
    {
        /**
         * Register with Proxy Module as Device is going to be a Proxy.
         * This is typically a one-time-event, and hence registering the
         * PROXY when Proxy ADV is being initiated!
         */
        UI_register_proxy();

        first_time = 1;
    }

    /* Set the role to Proxy with bearer */
    blebrr_gatt_mode_set(BLEBRR_GATT_PROXY_MODE);

    CONSOLE_OUT("Start Proxy Advertisements with %s for Subnet 0x%04X\n",
    (proxy_adv_mode == MS_PROXY_NET_ID_ADV_MODE) ? "Network ID" : "Node Identity",
    subnet_handle);

    retval = MS_proxy_server_adv_start
             (
                 subnet_handle,
                 proxy_adv_mode
             );

    CONSOLE_OUT("Retval - 0x%04X\n", retval);
#else /* MS_PROXY_SERVER */
    CONSOLE_OUT("\n [** ERR **] MS_PROXY_SERVER feature is DISABLED!\n");
    return;
#endif /* MS_PROXY_SERVER */
}

void UI_proxy_callback
     (
         NETIF_HANDLE       * handle,
         UCHAR                p_evt,
         UCHAR              * data_param,
         UINT16               data_len
     )
{
    UCHAR             role;

    MS_IGNORE_UNUSED_PARAM(data_len);

    switch(p_evt)
    {
        case MS_PROXY_UP_EVENT:
            CONSOLE_OUT(
            "\n\n[PROXY APPL]: MS_PROXY_UP_EVENT Received for NETIF Handle 0x%02X\n\n", *handle);

            if (NULL != data_param)
            {
                /* Catch the current role into a local */
                role = data_param[0];

                if (BRR_SERVER_ROLE == role)
                {
                    /* Send Secure Network Beacons */
                    /* MS_net_broadcast_secure_beacon(0x0000); */
                }
            }
            break;

        case MS_PROXY_DOWN_EVENT:
            CONSOLE_OUT(
            "\n\n[PROXY APPL]: MS_PROXY_DOWN_EVENT Received for NETIF Handle 0x%02X\n\n", *handle);
            break;

        default:
            CONSOLE_OUT(
            "\n\n[PROXY APPL ERR]: Unknown Event Received for NETIF Handle 0x%02X!!\n\n", *handle);
            break;
    }
}

void UI_register_proxy(void)
{
    API_RESULT retval;

    CONSOLE_OUT("Registering with Proxy layer...\n");
    retval =  MS_proxy_register(UI_proxy_callback);
    CONSOLE_OUT("Retval - 0x%04X\n", retval);
}

API_RESULT UI_set_brr_scan_rsp_data (void)
{
    /**
     * Currently setting MT-MESH-SAMPLE-8 as Complete Device Name!
     * This can be updated to each individual devices as per requirement.
     */
    UCHAR UI_brr_scanrsp_data[] =
          {
           0x0D, 0x09, 'P', 'H', 'Y', '-', 'M', 'S', 'H', 'L', 'I', 'G', 'H', 'T'
          };
    
    CONSOLE_OUT("\n Setting PHY MSH LIGHT as Complete Device Name!\n");

    /* Set the Scan Response Data at the Bearer Layer */
    blebrr_set_adv_scanrsp_data_pl
    (
        UI_brr_scanrsp_data,
        sizeof(UI_brr_scanrsp_data)
    );

    return API_SUCCESS;
}

EM_timer_handle thandle;
void timeout_cb (void * args, UINT16 size)
{
    thandle = EM_TIMER_HANDLE_INIT_VAL;
    UI_sample_reinit();
}

void UI_gatt_iface_event_pl_cb
     (
         UCHAR  ev_name,
         UCHAR  ev_param
     )
{
    switch(ev_name)
    {
        /* GATT Bearer BLE Link Layer Disconnected */
        case BLEBRR_GATT_IFACE_DOWN:
            CONSOLE_OUT("\r\n >> GATT Bearer BLE Link Layer Disconnection Event Received!\r\n");
            //UI_sample_reinit();
            EM_start_timer (&thandle, 3, timeout_cb, NULL, 0);
            break;

        /* GATT Bearer BLE Link Layer Connected */
        case BLEBRR_GATT_IFACE_UP:
            CONSOLE_OUT("\r\n >> GATT Bearer BLE Link Layer Connection Event Received!\r\n");
            /* Do Nothing! */
            if (BLEBRR_GATT_PROV_MODE == blebrr_gatt_mode_get())
            {
                MS_brr_bcast_end(BRR_BCON_TYPE_UNPROV_DEVICE, BRR_BCON_ACTIVE);
            }
            else if (BLEBRR_GATT_PROXY_MODE == blebrr_gatt_mode_get())
            {
                MS_proxy_server_adv_stop();
            }
            break;

        case BLEBRR_GATT_IFACE_ENABLE:
            CONSOLE_OUT("\r\n >> GATT Bearer Active Event Received!\r\n");
            {
                if (BLEBRR_GATT_PROV_MODE == ev_param)
                {
                    /* Call to bind with the selected device */
                    UI_prov_bind(PROV_BRR_GATT, 0);
                }
            }
            break;

        case BLEBRR_GATT_IFACE_DISABLE:
            CONSOLE_OUT("\r\n >> GATT Bearer Inactive Event Received!\r\n");
            break;

        /* Unknown Event! */
        default:
            CONSOLE_OUT("\r\n >> GATT Bearer BLE Link Layer Unknown Event 0x%02X Received!\r\n", ev_name);
            /* Do Nothing! */
            break;
    }
}

void appl_mesh_sample (void)
{ 
    MS_ACCESS_NODE_ID node_id;
    MS_ACCESS_ELEMENT_DESC   element;
    MS_ACCESS_ELEMENT_HANDLE element_handle;
    API_RESULT retval;

    MS_CONFIG * config_ptr;

#ifdef MS_HAVE_DYNAMIC_CONFIG
    MS_CONFIG  config;

    /* Initialize dynamic configuration */
    MS_INIT_CONFIG(config);

    config_ptr = &config;
#else
    config_ptr = NULL;
#endif /* MS_HAVE_DYNAMIC_CONFIG */

    /* Initialize OSAL */
    EM_os_init();

    /* Initialize Debug Module */
    EM_debug_init();

    /* Initialize Timer Module */
    EM_timer_init();
    timer_em_init();

    /* Initialize utilities */
    nvsto_init();

    /* Initialize Mesh Stack */
    MS_init(config_ptr);

    /* Register with underlying BLE stack */
    blebrr_register();

    /* Register GATT Bearer Connection/Disconnection Event Hook */
    blebrr_register_gatt_iface_event_pl(UI_gatt_iface_event_pl_cb);

    /* Enable LED Port */
    /* Platform Abstraction Initializations of GPIOs/LEDs etc. */
    mesh_model_platform_init_pl();

    /* LED ON */
    /* LED ON/OFF for BOOT UP Indication Abstraction Call */
    mesh_model_device_bootup_ind_pl();

    /* Create Node */
    retval = MS_access_create_node(&node_id);

    /* Register Element */
    /**
     * TBD: Define GATT Namespace Descriptions from
     * https://www.bluetooth.com/specifications/assigned-numbers/gatt-namespace-descriptors
     *
     * Using 'main' (0x0106) as Location temporarily.
     */
    element.loc = 0x0106;

    retval = MS_access_register_element
             (
                 node_id,
                 &element,
                 &element_handle
             );

    if (API_SUCCESS == retval)
    {
        /* Register foundation model servers */
        retval = UI_register_foundation_model_servers(element_handle);
    }

//    retval = MS_access_register_element
//             (
//                 node_id,
//                 &element,
//                 &element_handle
//             );	

    if (API_SUCCESS == retval)
    {
        /* Register Generic OnOff model server */
        retval = UI_register_generic_onoff_model_server(element_handle);
    }

#ifdef  USE_HSL    
    if (API_SUCCESS == retval)
    {
        /* Register Light Lightness model server */
        retval = UI_register_light_hsl_model_server(element_handle);
    }
#endif    

#ifdef  USE_CTL    
    if (API_SUCCESS == retval)
    {
        /* Register Light Lightness model server */
        retval = UI_register_light_ctl_model_server(element_handle);
    }
#endif    

    if (API_SUCCESS == retval)
    {
        /* Register Light Lightness model server */
        retval = UI_register_scene_model_server(element_handle);
    }

//
//    if (API_SUCCESS == retval)
//    {
//        /* Register Vendor Defined model server */
//        retval = UI_register_vendor_defined_model_server(element_handle);
//    }

    if (API_SUCCESS == retval)
    {
        /* Initialize model states */
        UI_model_states_initialization();
    }

    /* Configure as provisionee/device */
    UI_register_prov();


    /**
     * Set Scan Response Data Before Starting Provisioning.
     * This is optional/additional set of Data that the device can
     * set to enhance the User Experience.
     * For Example, set a specific device name or URL as part of the
     * Scan Response Data when awaiting connections over GATT bearer.
     */
    UI_set_brr_scan_rsp_data();

    /* Enable Friend feature */
    MS_ENABLE_FRIEND_FEATURE();
  
    uint32 address = VENDOR_PRODUCT_MAC_ADDR;
        
    UI_lprov_device.uuid[10] = (uint8_t)ReadFlash(address ++);
    UI_lprov_device.uuid[11] = (uint8_t)ReadFlash(address ++);
    UI_lprov_device.uuid[12] = (uint8_t)ReadFlash(address ++);
    UI_lprov_device.uuid[13] = (uint8_t)ReadFlash(address ++);
    UI_lprov_device.uuid[8]  = (uint8_t)ReadFlash(address ++);
    UI_lprov_device.uuid[9]  = (uint8_t)ReadFlash(address ++);
    
    EM_start_timer (&thandle, 5, timeout_cb, NULL, 0);
    //EM_start_timer (&thandle, 5, timeout_cb, NULL, 0);

    return;
}
API_RESULT UI_sample_check_app_key(void)
{
    MS_APPKEY_HANDLE  handle;
    UINT8             * key;
    UINT8             aid;
    DECL_CONST UINT8  t_key[16] = {0};

    API_RESULT retval;

    CONSOLE_OUT("Fetching App Key for Handle 0x0000\n");

    handle = 0x0000;
    retval = MS_access_cm_get_app_key
             (
                 handle,
                 &key,
                 &aid
             );

    /* Check Retval. Print App Key */
    if (API_SUCCESS == retval)
    {
        CONSOLE_OUT("App Key[0x%02X]: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
        handle, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
        key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);

        if (0 == EM_mem_cmp(key, t_key, 16))
        {
            /* NO AppKey Bound */
            retval = API_FAILURE;
        }
        else
        {
            /* Found a Valid App Key */
            /* Keeping the retval as API_SUCCESS */
        }
    }

    return retval;
}


/**
 * On GATT Bearer Disconnection or on Startup:
 * 1. If device not provisioned, start unprovisioned beacons over GATT bearer
 * 2. If device is provisioned and App Key is bound i.e. Device is Configured
 *     - Check if Proxy Feature is enabled then Start ADV as Proxy,
 *       Else,
 *       Do nothing!
 *    Else,
 *    If device is provisioned and App Key is not bound i.e. Device is not Configured
 *       Start ADV as Proxy.
 */
void UI_sample_reinit(void)
{
    API_RESULT  retval;
    MS_NET_ADDR addr;
    UCHAR       is_prov_req;
    UCHAR       role, brr;
    UCHAR       state;

    retval      = API_SUCCESS;
    is_prov_req = MS_TRUE;

    //re provision
//    if(hal_gpio_read(14)==0)
//    {
        retval = MS_access_cm_get_primary_unicast_address(&addr);

        if (API_SUCCESS == retval)
        {
            if (MS_NET_ADDR_UNASSIGNED != addr)
            {
                /* Set Provisioning is not Required */
                is_prov_req = MS_FALSE;
            }
        }
//    }

    if (MS_TRUE == is_prov_req)
    {
        /* Start Provisioning over GATT here */
        /**
         * setup <role:[1 - Device, 2 - Provisioner]> <bearer:[1 - Adv, 2 - GATT]
         */
        role = PROV_ROLE_DEVICE;
        brr  = PROV_BRR_GATT;

        /**
         * Setting up an Unprovisioned Device over GATT
         */
        UI_setup_prov(role, brr);

        CONSOLE_OUT("\r\n Setting up as an Unprovisioned Device\r\n");
    }
    else
    {
        /* Fetch PROXY feature state */
        MS_access_cm_get_features_field(&state, MS_FEATURE_PROXY);

        /**
         * Check if the Device is Configured.
         * If not Configured, Start Proxy ADV.
         * If it is Configured,
         *      Check if the Proxy Feature is Enabled.
         *      If not enabled, then Do Nothing!
         *      If it is, Start Proxy ADV.
         */
        if (API_SUCCESS == UI_sample_check_app_key())
        {
            if (MS_ENABLE == state)
            {

                light_blink_set(LIGHT_GREEN, LIGHT_BLINK_FAST,5);
            
                CONSOLE_OUT("\r\n Provisioned Device - Starting Proxy with NetID on Subnet 0x0000!\r\n");

                /* Start Proxy ADV with Network ID here */
                UI_proxy_start_adv(0x0000, MS_PROXY_NET_ID_ADV_MODE);
            }
            else
            {
                light_blink_set(LIGHT_GREEN, LIGHT_BLINK_SLOW,3);
                MS_access_cm_set_features_field(MS_ENABLE,MS_FEATURE_PROXY);

                 CONSOLE_OUT("\r\n Provisioned Device - Enable PROXY !!!\r\n");
                /**
                 * Do Nothing!
                 * Already Scaning is Enabled at Start Up
                 */
                //blebrr_scan_enable();
                EM_start_timer (&thandle, 5, timeout_cb, NULL, 0);
            }
        }
        else
        {

            light_blink_set(LIGHT_BLUE, LIGHT_BLINK_FAST,5);
            /**
             * Provisioned but not configured device.
             * Still checking if the PROXY Feature is enabled or not.
             * Depending on the state of Proxy Feature:
             *   - If enabled, Start Proxy ADV with Network ID
             *   - Else, Start Proxy ADV with Node Identity.
             */
             (MS_ENABLE == state) ?
             UI_proxy_start_adv(0x0000, MS_PROXY_NET_ID_ADV_MODE):
             UI_proxy_start_adv(0x0000, MS_PROXY_NODE_ID_ADV_MODE);
        }
    }
}



