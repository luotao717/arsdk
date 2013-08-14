/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup fal_qos FAL_QOS
 * @{
 */ 
#include "sw.h"
#include "fal_qos.h"
#include "hsl_api.h"


static sw_error_t
_fal_qos_sch_mode_set(a_uint32_t dev_id,
                     fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_sch_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_sch_mode_set(dev_id, mode, weight);
    return rv;
}


static sw_error_t
_fal_qos_sch_mode_get(a_uint32_t dev_id,
                     fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_sch_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_sch_mode_get(dev_id, mode, weight);
    return rv;
}


static sw_error_t
_fal_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_tx_buf_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_tx_buf_status_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_tx_buf_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_tx_buf_status_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_tx_buf_nr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_tx_buf_nr_set(dev_id, port_id, queue_id, number);
    return rv;
}


static sw_error_t
_fal_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_tx_buf_nr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_tx_buf_nr_get(dev_id, port_id, queue_id, number);
    return rv;
}


static sw_error_t
_fal_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_tx_buf_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_tx_buf_status_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_tx_buf_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_tx_buf_status_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_tx_buf_nr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_tx_buf_nr_set(dev_id, port_id, number);
    return rv;
}


static sw_error_t
_fal_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_tx_buf_nr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_tx_buf_nr_get(dev_id, port_id, number);
    return rv;
}

static sw_error_t
_fal_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_rx_buf_nr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_rx_buf_nr_set(dev_id, port_id, number);
    return rv;
}


static sw_error_t
_fal_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_rx_buf_nr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_rx_buf_nr_get(dev_id, port_id, number);
    return rv;
}


static sw_error_t
_fal_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up, fal_queue_t queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_queue_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_queue_set(dev_id, up, queue);
    return rv;
}


static sw_error_t
_fal_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                        fal_queue_t * queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_queue_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_queue_get(dev_id, up, queue);
    return rv;
}


static sw_error_t
_fal_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp, fal_queue_t queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_queue_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_queue_set(dev_id, dscp, queue);
    return rv;
}


static sw_error_t
_fal_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                          fal_queue_t * queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_queue_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_queue_get(dev_id, dscp, queue);
    return rv;
}


static sw_error_t
_fal_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_mode_set(dev_id, port_id, mode, enable);
    return rv;
}


static sw_error_t
_fal_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_mode_get(dev_id, port_id, mode, enable);
    return rv;
}


static sw_error_t
_fal_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_mode_pri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_mode_pri_set(dev_id, port_id, mode, pri);
    return rv;
}


static sw_error_t
_fal_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t * pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_mode_pri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_mode_pri_get(dev_id, port_id, mode, pri);
    return rv;
}


static sw_error_t
_fal_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t up)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_up_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_up_set(dev_id, port_id, up);
    return rv;
}


static sw_error_t
_fal_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * up)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_up_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_up_get(dev_id, port_id, up);
    return rv;
}

static sw_error_t
_fal_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                     fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_sch_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_sch_mode_set(dev_id, port_id, mode, weight);
    return rv;
}


static sw_error_t
_fal_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                     fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_sch_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_sch_mode_get(dev_id, port_id, mode, weight);
    return rv;
}

static sw_error_t
_fal_qos_port_default_spri_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t spri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_spri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_spri_set(dev_id, port_id, spri);
    return rv;
}

static sw_error_t
_fal_qos_port_default_spri_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * spri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_spri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_spri_get(dev_id, port_id, spri);
    return rv;
}

static sw_error_t
_fal_qos_port_default_cpri_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t cpri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_cpri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_cpri_set(dev_id, port_id, cpri);
    return rv;
}

static sw_error_t
_fal_qos_port_default_cpri_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * cpri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_port_default_cpri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_port_default_cpri_get(dev_id, port_id, cpri);
    return rv;
}

static sw_error_t
_fal_qos_queue_remark_table_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_uint32_t tbl_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_remark_table_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_remark_table_set(dev_id, port_id, queue_id, tbl_id, enable);
    return rv;
}

static sw_error_t
_fal_qos_queue_remark_table_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_uint32_t * tbl_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->qos_queue_remark_table_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->qos_queue_remark_table_get(dev_id, port_id, queue_id, tbl_id, enable);
    return rv;
}

/**
 * @brief Set traffic scheduling mode on particular one device.
 *   @details   Comments:
 *   Particular device may only support parts of input options. Such as
 *    GARUDA doesn't support variable weight in wrr mode.
 *    When scheduling mode is sp the weight is meaningless usually it's zero 
 * @param[in] dev_id device id
 * @param[in] fal_sch_mode_t traffic scheduling mode
 * @param[in] weight[] weight value for each queue when in wrr mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_sch_mode_set(a_uint32_t dev_id,
                     fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_sch_mode_set(dev_id, mode, weight);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get traffic scheduling mode on particular device.
 * @param[in] dev_id device id
 * @param[in] fal_sch_mode_t traffic scheduling mode
 * @param[out] weight weight value for wrr mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_sch_mode_get(a_uint32_t dev_id,
                     fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_sch_mode_get(dev_id, mode, weight);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set buffer aggsinment status of transmitting queue on one particular port.
 *   @details  Comments:
 *    If enable queue tx buffer on one port that means each queue of this port
 *    will have fixed number buffers when transmitting packets. Otherwise they
 *    share the whole buffers with other queues in device.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_tx_buf_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get buffer aggsinment status of transmitting queue on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_tx_buf_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max occupied buffer number of transmitting queue on one particular port.
 *   @details  Comments:
 *    Because different device has differnet hardware granularity
 *    function will return actual buffer numbers in hardware. 
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_tx_buf_nr_set(dev_id, port_id, queue_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max occupied buffer number of transmitting queue on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_tx_buf_nr_get(dev_id, port_id, queue_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set buffer aggsinment status of transmitting port on one particular port.
 *   @details   Comments:
 *     If enable tx buffer on one port that means this port will have fixed
 *    number buffers when transmitting packets. Otherwise they will
 *    share the whole buffers with other ports in device.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_tx_buf_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get buffer aggsinment status of transmitting port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_tx_buf_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max occupied buffer number of transmitting port on one particular port.
 *   @details   Comments:
 *    Because different device has differnet hardware granularity
 *    function will return actual buffer number in hardware.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_tx_buf_nr_set(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max occupied buffer number of transmitting port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_tx_buf_nr_get(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max reserved buffer number of receiving port on one particular port.
 *   @details   Comments:
 *    Because different device has differnet hardware granularity
 *    function will return actual buffer number in hardware.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_rx_buf_nr_set(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max reserved buffer number of receiving port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_rx_buf_nr_get(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set user priority to queue mapping.
 * @param[in] dev_id device id
 * @param[in] up 802.1p
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up, fal_queue_t queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_queue_set(dev_id, up, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get user priority to queue mapping.
 * @param[in] dev_id device id
 * @param[in] dot1p 802.1p
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                        fal_queue_t * queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_queue_get(dev_id, up, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cos map dscp_2_queue item on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp, fal_queue_t queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_queue_set(dev_id, dscp, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cos map dscp_2_queue item on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                          fal_queue_t * queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_queue_get(dev_id, dscp, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port qos mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[in] enable A_TRUE of A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_mode_set(dev_id, port_id, mode, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port qos mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[out] enable A_TRUE of A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_mode_get(dev_id, port_id, mode, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set priority of one particular qos mode on one particular port.
 *   @details   Comments:
 *    If the priority of a mode is more small then the priority is more high.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[in] pri priority of one particular qos mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_mode_pri_set(dev_id, port_id, mode, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get priority of one particular qos mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[out] pri priority of one particular qos mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t * pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_mode_pri_get(dev_id, port_id, mode, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default user priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] up 802.1p
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t up)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_up_set(dev_id, port_id, up);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default user priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] up 802.1p
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * up)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_up_get(dev_id, port_id, up);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set traffic scheduling mode on particular one port.
 *   @details   Comments:
 *   Particular device may only support parts of input options. Such as
 *    GARUDA doesn't support variable weight in wrr mode.
 *    When scheduling mode is sp the weight is meaningless usually it's zero 
 * @param[in] dev_id device id
 * @param[in] fal_sch_mode_t traffic scheduling mode
 * @param[in] weight[] weight value for each queue when in wrr mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                     fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_sch_mode_set(dev_id, port_id, mode, weight);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get traffic scheduling mode on particular port.
 * @param[in] dev_id device id
 * @param[in] fal_sch_mode_t traffic scheduling mode
 * @param[out] weight weight value for wrr mode
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                     fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_sch_mode_get(dev_id, port_id, mode, weight);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default stag priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] spri vlan priority
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_spri_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t spri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_spri_set(dev_id, port_id, spri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default stag priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] spri vlan priority
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_spri_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * spri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_spri_get(dev_id, port_id, spri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default ctag priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cpri vlan priority
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_cpri_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t cpri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_cpri_set(dev_id, port_id, cpri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default ctag priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cpri vlan priority
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_port_default_cpri_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * cpri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_port_default_cpri_get(dev_id, port_id, cpri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set egress queue based CoS remark on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[in] tbl_id CoS remark table id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_remark_table_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_uint32_t tbl_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_remark_table_set(dev_id, port_id, queue_id, tbl_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get egress queue based CoS remark on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] tbl_id CoS remark table id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_qos_queue_remark_table_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_uint32_t * tbl_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_qos_queue_remark_table_get(dev_id, port_id, queue_id, tbl_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @}
 */
