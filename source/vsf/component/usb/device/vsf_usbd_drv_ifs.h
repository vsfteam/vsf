#ifndef __VSF_USBD_DRV_IFS_H__
#define __VSF_USBD_DRV_IFS_H__

#if defined(VSF_USBD_CFG_DRV_LV0)
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)

#   define vk_usbd_drv_func_name(__header, __func) TPASTE3(__header, _, __func)

// lv0 API used as usbd driver
#   define vk_usbd_drv_init(__cfg)                                              \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, init)                       \
            (&VSF_USBD_CFG_DRV_OBJ, (__cfg))

#   define vk_usbd_drv_fini()                                                   \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, fini)                       \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vk_usbd_drv_reset(__cfg)                                             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, reset)                      \
            (&VSF_USBD_CFG_DRV_OBJ, (__cfg))

#   define vk_usbd_drv_connect()                                                \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, connect)                    \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vk_usbd_drv_disconnect()                                             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, disconnect)                 \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vk_usbd_drv_wakeup()                                                 \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, wakeup)                     \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vk_usbd_drv_set_address(__addr)                                      \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, set_address)                \
            (&VSF_USBD_CFG_DRV_OBJ, (__addr))

#   define vk_usbd_drv_get_setup(__request)                                     \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, get_setup)                  \
            (&VSF_USBD_CFG_DRV_OBJ, (uint8_t *)(__request))

#   define vk_usbd_drv_status_stage(__is_in)                                    \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, status_stage)               \
            (&VSF_USBD_CFG_DRV_OBJ, (__is_in))

#   define vk_usbd_drv_ep_add(__addr, __attr, __size)                           \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_add)                     \
            (&VSF_USBD_CFG_DRV_OBJ, (__addr), (usb_ep_type_t)(__attr), (__size))

#   define vk_usbd_drv_ep_get_feature(__ep, __feature)                          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_get_feature)             \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__feature))

#   define vk_usbd_drv_ep_get_size(__ep)                                        \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_get_size)                \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_is_stalled(__ep)                                      \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_is_stalled)              \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_set_stall(__ep)                                       \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_set_stall)               \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_clear_stall(__ep)                                     \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_clear_stall)             \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_get_data_size(__ep)                                   \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_get_data_size)           \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_transaction_read_buffer(__ep, __buf, __size)          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transaction_read_buffer) \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size))

#   define vk_usbd_drv_ep_transaction_write_buffer(__ep, __buf, __size)         \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transaction_write_buffer)\
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size))

#   define vk_usbd_drv_ep_transaction_set_data_size(__ep, __size)               \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transaction_set_data_size)\
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__size))

#   define vk_usbd_drv_ep_transaction_enable_out(__ep)                          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transaction_enable_out)  \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vk_usbd_drv_ep_transfer_send(__ep, __buf, __size, __zlp)             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transfer_send)           \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size), (__zlp))

#   define vk_usbd_drv_ep_transfer_recv(__ep, __buf, __size)                    \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_transfer_recv)           \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size))

#elif defined(VSF_USBD_CFG_DRV_LV1)
// lv1 API used as usbd driver
#   error "not implemented now!!!"

#   define vk_usbd_drv_init(__cfg)
#   define vk_usbd_drv_fini()
#   define vk_usbd_drv_reset(__cfg)
#   define vk_usbd_drv_connect()
#   define vk_usbd_drv_disconnect()
#   define vk_usbd_drv_wakeup()
#   define vk_usbd_drv_set_address(__addr)
#   define vk_usbd_drv_get_setup(__request)
#   define vk_usbd_drv_status_stage(__is_in)
#   define vk_usbd_drv_ep_add(__addr, __attr, __size)

#   define vk_usbd_drv_ep_get_feature(__ep, __feature)
#   define vk_usbd_drv_ep_get_size(__ep)
#   define vk_usbd_drv_ep_is_stalled(__ep)
#   define vk_usbd_drv_ep_set_stall(__ep)
#   define vk_usbd_drv_ep_clear_stall(__ep)
#   define vk_usbd_drv_ep_get_data_size(__ep)
#   define vk_usbd_drv_ep_transaction_read_buffer(__ep, __buf, __size)
#   define vk_usbd_drv_ep_transaction_write_buffer(__ep, __buf, __size)
#   define vk_usbd_drv_ep_transaction_set_data_size(__ep, __size)
#   define vk_usbd_drv_ep_transaction_enable_out(__ep)
#   define vk_usbd_drv_ep_transfer_send(__ep, __buf, __size, __zlp)
#   define vk_usbd_drv_ep_transfer_recv(__ep, __buf, __size)

#else
// interface-based API used as usbd driver
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)                                          \
         const i_usb_dc_t *__drv = (__dev)->drv;

#   define vk_usbd_drv_init(__cfg)                     __drv->Init((__cfg))
#   define vk_usbd_drv_fini()                          __drv->Fini()
#   define vk_usbd_drv_reset(__cfg)                    __drv->Reset((__cfg))
#   define vk_usbd_drv_connect()                       __drv->Connect()
#   define vk_usbd_drv_disconnect()                    __drv->Disconnect()
#   define vk_usbd_drv_wakeup()                        __drv->Wakeup()
#   define vk_usbd_drv_set_address(__addr)             __drv->SetAddress((__addr))
#   define vk_usbd_drv_get_setup(__request)            __drv->GetSetup((uint8_t *)(__request))
#   define vk_usbd_drv_status_stage(__is_in)           __drv->StatusStage((__is_in))
#   define vk_usbd_drv_ep_add(__addr, __attr, __size)  __drv->Ep.Add((__addr), (usb_ep_type_t)(__attr), (__size))

#   define vk_usbd_drv_ep_get_feature(__ep, __feature) __drv->Ep.GetFeature((__ep), (__feature))
#   define vk_usbd_drv_ep_get_size(__ep)               __drv->Ep.GetSize((__ep))
#   define vk_usbd_drv_ep_is_stalled(__ep)             __drv->Ep.IsStalled((__ep))
#   define vk_usbd_drv_ep_set_stall(__ep)              __drv->Ep.SetStall((__ep))
#   define vk_usbd_drv_ep_clear_stall(__ep)            __drv->Ep.ClearStall((__ep))
#   define vk_usbd_drv_ep_get_data_size(__ep)          __drv->Ep.GetDataSize((__ep))
#   define vk_usbd_drv_ep_transaction_read_buffer(__ep, __buf, __size)          \
                __drv->Ep.Transaction.ReadBuffer((__ep), (__buf), (__size))
#   define vk_usbd_drv_ep_transaction_write_buffer(__ep, __buf, __size)         \
                __drv->Ep.Transaction.WriteBuffer((__ep), (__buf), (__size))
#   define vk_usbd_drv_ep_transaction_set_data_size(__ep, __size)               \
                __drv->Ep.Transaction.SetDataSize((__ep), (__size))
#   define vk_usbd_drv_ep_transaction_enable_out(__ep)                          \
                __drv->Ep.Transaction.EnableOut((__ep))
#   define vk_usbd_drv_ep_transfer_send(__ep, __buf, __size, __zlp)             \
                __drv->Ep.Transfer.Send((__ep), (__buf), (__size), (__zlp))
#   define vk_usbd_drv_ep_transfer_recv(__ep, __buf, __size)                    \
                __drv->Ep.Transfer.Recv((__ep), (__buf), (__size))

#endif

#endif      // __VSF_USBD_DRV_IFS_H__
