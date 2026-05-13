/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: oal_skbuff.c.
 * Author: Hisilicon
 * Create: 2018-08-04
 */

/******************************************************************************
  1 头文件包含
******************************************************************************/
#include "oal_skbuff.h"
#include "oal_ext_if.h"

#define  TIME_100_US                   100
#define  RETRY_TIMES                   100

#define  DEFAULT_MEM_POOL              m_aucSysMem1
/******************************************************************************
  2 全局变量
******************************************************************************/
/******************************************************************************
  3 函数实现
******************************************************************************/

static hi_pvoid los_alloc_mem(const hi_pvoid pool, hi_u32 size, hi_u32 boundary, hi_s32 try_times)
{
    LOS_MEM_POOL_STATUS status = { 0 };
    hi_s32 times = try_times;
    while (times > 0) {
        LOS_MemInfoGet(pool, &status);
        if (status.uwMaxFreeNodeSize < size + boundary) {
            times--;
            udelay(TIME_100_US);
            continue;
        }

        hi_pvoid buf = LOS_MemAllocAlign(pool, size, boundary);
        if (buf != NULL) {
            return buf;
        }
        times--;
    }

    return HI_NULL;
}
/*
 * 功能描述:分配skb内部函数
 */
struct sk_buff *_alloc_skb(hi_u32 size)
{
    struct sk_buff *skb = NULL;
    hi_u8 *data = HI_NULL;

    skb = (struct sk_buff *)los_alloc_mem(DEFAULT_MEM_POOL, skb_data_align(sizeof(struct sk_buff)),
        USB_CACHE_ALIGN_SIZE, RETRY_TIMES);
    if (skb == NULL) {
        return HI_NULL;
    }
    data = (hi_u8 *)los_alloc_mem(DEFAULT_MEM_POOL, size, USB_CACHE_ALIGN_SIZE, RETRY_TIMES);
    if (data == NULL) {
        LOS_MemFree(DEFAULT_MEM_POOL, skb);
        return HI_NULL;
    }

    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(skb, sizeof(struct sk_buff), 0, sizeof(struct sk_buff));
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(data, size, 0, size);
    skb->truesize = skb_truesize(size);
    hi_atomic_set(&skb->users, 1);
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
    skb->end = skb->tail + size;

    return skb;
}

/*
 * 功能描述:分配skb
 */
struct sk_buff *alloc_skb(hi_u32 size)
{
    return _alloc_skb(size);
}

/*
 * 功能描述:dev分配skb
 */
struct sk_buff *dev_alloc_skb(hi_u32 length)
{
    /*
     * There is more code here than it seems:
     * __dev_alloc_skb is an inline
     */
    return _dev_alloc_skb(length);
}

/*
 * 功能描述:链表尾部插入skb
 */
hi_u8 *skb_put(struct sk_buff *skb, hi_u32 len)
{
    hi_u8 *tmp = skb_tail_pointer(skb);
    skb->tail += len;
    skb->len  += len;
    if (skb->tail > skb->end) {
        dprintf("!!skb_put overflow!!\n");
    }
    return tmp;
}

/*
 * 功能描述:skb出队
 */
struct sk_buff *skb_dequeue(struct sk_buff_head *list)
{
    hi_u32 flags;
    struct sk_buff *result = HI_NULL;

    oal_spin_lock_irq_save(&list->lock, &flags);
    result = _skb_dequeue(list);
    oal_spin_unlock_irq_restore(&list->lock, &flags);
    return result;
}

struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list)
{
    unsigned long flags;
    struct sk_buff *result = HI_NULL;

    oal_spin_lock_irq_save(&list->lock, &flags);
    result = _skb_dequeue_tail(list);
    oal_spin_unlock_irq_restore(&list->lock, &flags);
    return result;
}

/*
 * 功能描述:释放skb
 */
hi_void kfree_skb(struct sk_buff *skb)
{
    if (skb == HI_NULL)
        return;
    LOS_MemFree(DEFAULT_MEM_POOL, skb->head);
    LOS_MemFree(DEFAULT_MEM_POOL, skb);
}

/*
 * 功能描述:skb队列尾部
 */
hi_void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
    hi_u32 flags;

    oal_spin_lock_irq_save(&list->lock, &flags);
    _skb_queue_tail(list, newsk);
    oal_spin_unlock_irq_restore(&list->lock, &flags);
}

/*
 * 功能描述:skb trim
 */
hi_void skb_trim(struct sk_buff *skb, hi_u32 len)
{
    if (skb->len > len)
        _skb_trim(skb, len);
}

/*
 * 功能描述:skb扩展头
 */
hi_s32 pskb_expand_head(struct sk_buff *skb, hi_u32 nhead, hi_u32 ntail, gfp_t gfp_mask)
{
    hi_unref_param(gfp_mask);
    hi_u8 *data = HI_NULL;
    hi_u32 size = nhead + (hi_u32)(skb_end_pointer(skb) - skb->head) + ntail;
    hi_u32 off;
    hi_u32 data_off;

    size = skb_data_align(size);
    data = (hi_u8 *)los_alloc_mem(m_aucSysMem0, size, CACHE_ALIGNED_SIZE, RETRY_TIMES);
    if (!data) {
        return -ENOMEM;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(data, size, 0, size);
    if (memcpy_s(data + nhead, size - nhead, skb->head, (hi_u32)(skb_tail_pointer(skb) - skb->head)) != EOK) {
        dprintf("pskb_expand_head :: memcpy_s failed\n");
    }

    data_off = (hi_u32)(skb->data - skb->head);

    kfree(skb->head);
    skb->head = data;
    skb->data = skb->head + data_off + nhead;
    skb->end = size;
    off = nhead;
    skb->tail += off;

    return 0;
}

/*
 * 功能描述:dev释放skb
 */
hi_void dev_kfree_skb(struct sk_buff *skb)
{
    if (oal_unlikely(skb == HI_NULL)) {
        return;
    }
    kfree_skb(skb);
}

void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk)
{
    unsigned long flags;

    oal_spin_lock_irq_save(&list->lock, &flags);
    _skb_queue_head(list, newsk);
    oal_spin_unlock_irq_restore(&list->lock, &flags);
}

/*
 * 功能描述:skb unshare
 */
struct sk_buff *skb_unshare(struct sk_buff *skb)
{
    return skb;
}

void skb_queue_purge(struct sk_buff_head *list)
{
    struct sk_buff *skb;
    while ((skb = skb_dequeue(list)) != NULL)
        kfree_skb(skb);
}
