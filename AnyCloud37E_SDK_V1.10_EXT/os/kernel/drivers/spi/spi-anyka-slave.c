/*
 *  spi_anyka_slave.c - Anyka SPI slave controller driver
 *  based on spi_anyka.c
 *
 *  Copyright (C) Anyka 2012
 *  Wangsheng Gao <gao_wangsheng@anyka.oa>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */

/*
 * Note:
 *
 * Supports interrupt programmed transfers.
 * 
 */

 /*
  * Usage:
  * 	1. set one device as master:
  *		1. register spidev board info in platform file;
  *		2. select spidev and anyka spi controler in menuconfig.
  *	2. set one device as slave: select anyka spi slave controler in menuconfig
  *	3. run make command in Documentation/spi/ directory
  *	4. copy Documentation/spi/spi_master_test and Documentation/spi/spi_slave_test to Your_rootfs_dir
  *	5. firstly run spi_slave_test in slave and then run spi_master_test in master
  */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <asm/uaccess.h>
#include <mach/anyka_types.h>
#include <mach/ak_l2.h>
#include <linux/dma-mapping.h>
#include "ak39_spi.h"
#include <linux/printk.h>


#define DEBUG



//用于记录中断的状态
volatile u32 spi_intr_flag= 0; 

/**
*  @brief       	slave data: devices, lock, wait_queue, io resource...
*  @author     gao wangsheng
*  @date        2012-10-16
*  @param[out]  void
*  @param[in]   void
*  @param[in]   void
*  @return      void
*/
struct spi_anyka_slave {
	/* Driver model hookup */
	struct platform_device *pdev;
	struct ak_spi_info *pdata;
	struct cdev cdev;
	struct fasync_struct *async_queue; /* asynchronous readers */
   
	struct clk		*clk;

	/* Lock */
	struct mutex	slave_lock;
	spinlock_t regs_lock;

	/* wait queue */
	wait_queue_head_t readq;
	wait_queue_head_t writeq;
	/*-----------------dma--------------------*/
	struct completion	 done;
	struct device		*dev;
	/*use for dma or cpu*/
	int 			xfer_mode; 
	/* spi controller need a DMA memory.*/
		void *txbuffer;
		dma_addr_t txdma_buffer;
	
		void *rxbuffer;
		dma_addr_t rxdma_buffer;

		int xfer_dir; 
        int tmp1; 
		u8 			l2buf_tid;
        int tmp2; 
		u8 			l2buf_rid;
		s16			bus_num;
		int			 		count;			/*dma-have transferred len*/
	/*---------------dma----------------------*/	
	/* read and write buffer */
	char *rdbuf, *rdend;
	char *recvp, *readp;
	int rdsize;                         /*dam-need transfer len*/
	char *wrbuf, *wrend;
	char *sentp, *writep;
	int wrsize;
	
	/* Spi controler register addresses */
	void *ioarea;
	void *regs;
	int irq;

	/* flag */
	u8 mode;
	u8 bits_per_word;
	u32 max_speed_hz;
	int openers;
};

static struct class *slave_class;
static int slave_major;
static int slave_minor = 0;
#define SLAVE_MAX_MINOR	(8)
#define DFT_DIV				(255)
#define DFT_CON 			AK_SPICON_EN
#define DCNT				(0xffff)
#define SET_CLK				(1 << 0)
#define SET_MODE			(1 << 1)
#define SPI_MODE_MASK (SPI_CPOL | SPI_CPHA )
#define SLAVE_MASK	(AK_SPIINT_LESSWORD|AK_SPIINT_THRESHOLD|AK_SPIINT_TIMEOUT|AK_SPIINT_RXOVER)
#define sdbug(fmt...)	//printk(fmt)




/*-------dma----------*/

#define TRANS_TIMEOUT 			(10000)
#define SPI_TRANS_TIMEOUT 		(50000)
#define MAX_XFER_LEN 			SPI_DMA_MAX_LEN
#define SPI_DMA_MAX_LEN		    (8192)
#define SPI_L2_TXADDR(m)   \
	((m->bus_num == AKSPI_BUS_NUM1) ? ADDR_SPI0_TX : ((m->bus_num == AKSPI_BUS_NUM2) ? ADDR_SPI1_TX : ADDR_SPI2_TX))
#define SPI_L2_RXADDR(m)   \
	((m->bus_num == AKSPI_BUS_NUM1) ? ADDR_SPI0_RX : ((m->bus_num == AKSPI_BUS_NUM2) ? ADDR_SPI1_RX : ADDR_SPI2_RX))


//中断标志
#define SPI_SLV_LESS_WORD_FLAG			(1<<10)	 
#define SPI_SLV_STA_TRANSF_FLAG			(1<<9)	
#define SPI_SLV_CS_END_FLAG				(1<<11)
//控制寄存器偏移量
#define SPI_THRES_FUNCTION_ENA (1<<19)//threshold function enable
#define SPI_LESS_WORD_ENA (1<<20) //less word function enable
#define SPI_AUTO_LOAD_ENA (1<<7)  //enable auto load
#define SPI_SET_FLAG(a,b)           ((a) |= (b))
#define SPI_CLEAR_FLAG(a,b)			((a) &= ~(b))



enum spi_xfer_dir {
	SPI_DIR_TX,
	SPI_DIR_RX,
	SPI_DIR_TXRX, 
	SPI_DIR_XFER_NUM,
};

/*---dma-*/
/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		Enable or disable irq
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @param[in]   	is enable?
*  @return      	void
*/
static void akspi_slave_set_irq(struct spi_anyka_slave *slave, int enable)
{
	u32 spiint = SLAVE_MASK; 
	if (enable)
		__raw_writel(spiint, slave->regs + AK_SPIINT);
		
	else
		__raw_writel(0, slave->regs + AK_SPIINT);
}


/*--dma-*/
static inline bool ak_spi_use_dma(struct spi_anyka_slave *slave)
{
	return (slave->xfer_mode == AKSPI_XFER_MODE_DMA);
}
static inline u32 disable_imask(struct spi_anyka_slave *slave, u32 imask)
{
	u32 newmask;

	newmask = readl(slave->regs + AK_SPIINT);
	newmask &= ~imask;

	writel(newmask, slave->regs + AK_SPIINT);

	return newmask;
}


static inline u32 enable_imask(struct spi_anyka_slave *slave, u32 imask)
{
	u32 newmask;

	newmask = readl(slave->regs + AK_SPIINT);
	newmask |= imask;

	writel(newmask, slave->regs + AK_SPIINT);

	return newmask;
}
/*
static void dbg_dumpregs(struct spi_anyka_slave *slave)
{
	pr_debug("\n");
	pr_debug("CON: \t0x%x\n", ioread32(slave->regs + AK_SPICON));
	pr_debug("STA: \t0x%x\n", ioread32(slave->regs + AK_SPISTA));
	pr_debug("INT: \t0x%x\n", ioread32(slave->regs + AK_SPIINT));
	pr_debug("CNT: \t0x%x\n", ioread32(slave->regs + AK_SPICNT));
	pr_debug("DOUT: \t0x%x\n", ioread32(slave->regs + AK_SPIOUT));
	pr_debug("DIN: \t0x%x\n", ioread32(slave->regs + AK_SPIIN));
}
*/
static inline int wait_for_spi_cnt_to_zero(struct spi_anyka_slave *slave, u32 timeout)
{
	do {			 
		if (readl(slave->regs + AK_SPICNT) == 0)
			break;
		udelay(1);
	}while(timeout--);
	
	return (timeout > 0) ? 0 : -EBUSY;
}

/*
//字符以16进制显示，用于调试
static void hex_dump1(const void *src, size_t length, size_t line_size)
{
	int i;
    const unsigned char *address = src;
	while (length-- > 0) {
		pr_err("%02X ", *address++);
			if (!(++i % line_size) || (length == 0 && i % line_size)) { 
			pr_err("\n");
		}
	}
}
*/


/**
*  @brief		spi read data function by l2dma mode.
*				data from DMA buffer to l2 buffer.
*  @author		luo minjie
*  @email		luo_minjie@anyka.oa
*  @date		2022-07-13
*  @param[in]	*slave
*  @param[in]	*buf
*  @param[in]	int count
*  @return		transfer success result count
*/
static int spi_dma_write(struct spi_anyka_slave *slave, unsigned char *buf, int count)
{
	int ret = 0;
	bool flags = false;
	u32 val;
	int remain,i,loop;
    
    pr_info("%s  count:%d  slave->l2buf_tid:%d\n", __func__,count, slave->l2buf_tid);
    
	init_completion(&slave->done);	
	enable_imask(slave, AK_SPIINT_TRANSF);
	val = AK_SPIEXTX_BUFEN|AK_SPIEXTX_DMAEN;
	iowrite32(val, slave->regs + AK_SPIEXTX);
	iowrite32(count, slave->regs + AK_SPICNT);
    
	/*use for dma mode: greater than 256 bytes, align 4bytes of buf addr,
		align 64 bytes of data count*/
	if((count < 256) || ((unsigned long)buf & 0x3) || (count & (64 - 1))) {
        
        pr_info("count < 256 so write way cpu transfer ----\n");
        
	loop = count / L2_BUF_STATUS_MULTIPLY_RATIO;
	remain = count % L2_BUF_STATUS_MULTIPLY_RATIO;

	for (i = 0; i < loop; i++) {
			
			while (l2_get_status(slave->l2buf_tid) == (L2_BUFFER_SIZE / L2_BUF_STATUS_MULTIPLY_RATIO))
				;	/* Waiting for L2 buffer to NOT full(means writable) */
			
			l2_cpu((unsigned long)buf + i * L2_BUF_STATUS_MULTIPLY_RATIO, slave->l2buf_tid,
				(i % 8) * L2_BUF_STATUS_MULTIPLY_RATIO, L2_BUF_STATUS_MULTIPLY_RATIO, MEM2BUF);
	}
	if (remain > 0) {
			while (l2_get_status(slave->l2buf_tid) == (L2_BUFFER_SIZE / L2_BUF_STATUS_MULTIPLY_RATIO))
				;	/* Waiting for L2 buffer to NOT full(means writable) */

			l2_cpu((unsigned long)buf + loop * L2_BUF_STATUS_MULTIPLY_RATIO, slave->l2buf_tid,
				(loop % 8) * L2_BUF_STATUS_MULTIPLY_RATIO, remain, MEM2BUF);
	}
		
		//l2_combuf_cpu((unsigned long)buf, slave->l2buf_tid, count, MEM2BUF);
		//hex_dump1(buf,count,32);	
	} 
    else {
         pr_info("count > 256 so write way dma transfer ----\n");
         
		/*data from mtd cache to dma buffer */
		memcpy(slave->txbuffer, buf, count);
		/*start l2 dma transmit*/
		l2_combuf_dma(slave->txdma_buffer, slave->l2buf_tid, count, MEM2BUF, AK_FALSE);
		flags = true;
	}
     pr_info("---%s wait spi-master read --LINE:%d----\n", __func__,__LINE__);
    
 #if 0	
        ret = wait_for_completion_timeout(&slave->done, msecs_to_jiffies(SPI_TRANS_TIMEOUT));
        if(ret <= 0) {
            pr_err("wait for spi transfer interrupt timeout(%s).\n", __func__);
            dbg_dumpregs(slave);
            ret = -EINVAL;
            goto xfer_fail;
        }
#else
         wait_for_completion(&slave->done);
#endif

    pr_info("%s  slave->l2buf_tid:%d\n", __func__, slave->l2buf_tid);

	if (flags && (l2_combuf_wait_dma_finish(slave->l2buf_tid) == AK_FALSE))	{
		pr_err("%s: l2_combuf_wait_dma_finish failed!\n", __func__);
		ret = -EINVAL;
		goto xfer_fail;
	}
	ret = wait_for_spi_cnt_to_zero(slave, TRANS_TIMEOUT);
	if(ret)	{
		pr_err("%s: wait_for_spi_cnt_to_zero failed!\n", __func__);
		ret = -EINVAL;
		goto xfer_fail;
	}
    
     pr_info("------%s -success------\n",__func__);
    
	//清除传输完成标志位
	SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_STA_TRANSF_FLAG);

	ret = count;
xfer_fail:

	//disable l2 dma
	iowrite32(0, slave->regs + AK_SPIEXTX);
	l2_clr_status(slave->l2buf_tid);
	return ret;
}



/**
*  @brief		spi read data function by l2dma mode.
*				data from l2 buffer to DMA buffer.
*  @author		luo minjie
*  @email		luo_minjie@anyka.oa
*  @date		2022-07-13
*  @param[in]	*slave
*  @param[in]	*buf
*  @param[in]	int count
*  @return		transfer success result count
*/
static int spi_dma_read(struct spi_anyka_slave *slave, unsigned char *buf, int count)
{
	int ret = 0;
	bool flags = false;
	u32 val;
	u32 value = 0;
	int remain,i,loop;

    pr_info("%s  count:%d  slave->l2buf_rid:%d\n", __func__,count, slave->l2buf_rid);
    
	//使能LESS_WORD控制寄存器//使能阀值
	value = __raw_readl(slave->regs + AK_SPICON);
	value &= ~(SPI_THRES_FUNCTION_ENA);
	value |= SPI_THRES_FUNCTION_ENA|SPI_LESS_WORD_ENA|SPI_AUTO_LOAD_ENA;
	__raw_writel(value, slave->regs + AK_SPICON);
	
	/*prepare spi read*/
	init_completion(&slave->done);
	//使能传输完成中断，当传输操作完成时产生中断
	enable_imask(slave, AK_SPIINT_TRANSF);
	
	val = __raw_readl(slave->regs + AK_SPIEXRX);
	val = AK_SPIEXRX_DMAEN|AK_SPIEXRX_BUFEN;
	iowrite32(val, slave->regs + AK_SPIEXRX);
	
	iowrite32(count, slave->regs + AK_SPICNT);
			
	if(count < 256 ) {
		 pr_info("count < 256 so read way cpu transfer ----\n");

	loop = count / L2_BUF_STATUS_MULTIPLY_RATIO;
	remain = count % L2_BUF_STATUS_MULTIPLY_RATIO;

		for (i = 0; i < loop; i++) {
			while (l2_get_status(slave->l2buf_rid) == 0)
				;	/* Waiting for L2 buffer to be not empty (means readable)等待缓冲区不为空（表示可读） */
			l2_cpu((unsigned long)buf +  i * L2_BUF_STATUS_MULTIPLY_RATIO, slave->l2buf_rid,
				(i % 8) * L2_BUF_STATUS_MULTIPLY_RATIO, L2_BUF_STATUS_MULTIPLY_RATIO, BUF2MEM);		
		}
		if (remain > 0)
			{
			/*
			//轮询中断标志位
			while(!(spi_intr_flag & SPI_SLV_CS_END_FLAG));
			SPI_CLEAR_FLAG(spi_intr_flag,SPI_SLV_CS_END_FLAG);
			*/
			if(remain%4)
			{	
					pr_info("wait less intr \n");  
					while(1) 
					{
						if(spi_intr_flag & SPI_SLV_LESS_WORD_FLAG)
						{
							SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_LESS_WORD_FLAG);
							break;	
						}
					}
					
			}
			while(1)
					{
						if(spi_intr_flag & SPI_SLV_STA_TRANSF_FLAG)
						{
							
							//pr_err("---spi_intr_flag:%#x----LINE:%d--\n", spi_intr_flag, __LINE__);
							SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_STA_TRANSF_FLAG);
							//pr_err("---spi_intr_flag:%#x----LINE:%d--\n", spi_intr_flag, __LINE__);
							break;
						}
					}
			
			l2_cpu((unsigned long)buf + loop * L2_BUF_STATUS_MULTIPLY_RATIO,slave->l2buf_rid,
				(loop % 8) * L2_BUF_STATUS_MULTIPLY_RATIO, remain, BUF2MEM);
			//打印接收的取余的字节
			//hex_dump1((unsigned long)buf+ loop * L2_BUF_STATUS_MULTIPLY_RATIO,
				//strlen((unsigned long)buf + loop * L2_BUF_STATUS_MULTIPLY_RATIO),32);
			}

			
	} 
	else {
            loop = count / L2_BUF_STATUS_MULTIPLY_RATIO;
            remain = count % L2_BUF_STATUS_MULTIPLY_RATIO;
            #if 0
            if (loop != 0)
             l2_combuf_dma(slave->rxdma_buffer, slave->l2buf_rid, loop*64, BUF2MEM, AK_FALSE);
            #endif
            if (remain > 0)
            {
                /*
                //轮询中断标志位
                while(!(spi_intr_flag & SPI_SLV_CS_END_FLAG));
                SPI_CLEAR_FLAG(spi_intr_flag,SPI_SLV_CS_END_FLAG);
                */
                if(remain%4)
                {   
                        pr_info("---%s --LINE:%d----\n", __func__,__LINE__);
                        while(1) 
                        {
                            if(spi_intr_flag & SPI_SLV_LESS_WORD_FLAG)
                            {
                                SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_LESS_WORD_FLAG);
                                break;  
                            }
                        }
                         pr_info("---%s --LINE:%d----\n", __func__,__LINE__);
                        while(1)
                        {
                            if(spi_intr_flag & SPI_SLV_STA_TRANSF_FLAG)
                            {
                                
                                //pr_err("---spi_intr_flag:%#x----LINE:%d--\n", spi_intr_flag, __LINE__);
                                SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_STA_TRANSF_FLAG);
                                //pr_err("---spi_intr_flag:%#x----LINE:%d--\n", spi_intr_flag, __LINE__);
                                break;
                            }
                        }
                        
                }
               l2_cpu(slave->rxdma_buffer,slave->l2buf_rid,0, remain, BUF2MEM);
                
            }
            /* data from l2 buffer to dma buffer*/
            l2_combuf_dma(slave->rxdma_buffer, slave->l2buf_rid, count, BUF2MEM, AK_FALSE);
            flags = true;
        }
     pr_info("---%s wait spi-master write --LINE:%d----\n", __func__,__LINE__);
#if 0	
	ret = wait_for_completion_timeout(&slave->done, msecs_to_jiffies(SPI_TRANS_TIMEOUT));
	if(ret <= 0) {
		pr_err("wait for spi transfer interrupt timeout(%s).\n", __func__);
		dbg_dumpregs(slave);
		ret = -EINVAL;
		goto xfer_fail;
	}
#else
     wait_for_completion(&slave->done);
#endif

    pr_info("%s  slave->l2buf_rid:%d\n", __func__, slave->l2buf_rid);

	/*wait L2 dma finish, if need frac dma,start frac dma*/
	if (flags && l2_combuf_wait_dma_finish(slave->l2buf_rid) ==  AK_FALSE)	{
		pr_err("%s: l2_combuf_wait_dma_finish failed!\n", __func__);
		ret = -EINVAL;
		goto xfer_fail;
	}


	/*wait for spi count register value to zero.*/
	ret = wait_for_spi_cnt_to_zero(slave, TRANS_TIMEOUT);
	if(ret)	{
		pr_err("%s: wait for spi count to zero failed!\n", __func__);
		ret = -EINVAL;
		goto xfer_fail;
	}


	if(flags == true){
		//hex_dump1(slave->rxbuffer,count,32);
		memcpy(buf, slave->rxbuffer, count);
		
	}
    
    pr_info("------%s -success------\n",__func__);
    
	SPI_CLEAR_FLAG(spi_intr_flag, SPI_SLV_STA_TRANSF_FLAG);
	ret = count;
xfer_fail:
	//disable l2 dma
	iowrite32(0, slave->regs + AK_SPIEXRX);
	l2_clr_status(slave->l2buf_rid);
	return ret;
}


/**
*  @brief       spi transfer function by l2dma/l2cpu mode, actual worker 
*  				spi_dma_read()/spi_dma_write() to be call.
*  @author   	luo minjie
*  @email		luo_minjie@anyka.oa
*  @date        2022-07-13
*  @param[in]   *slave
*  @return      transfer success result 0, otherwise result a negative value
*/
static int ak_spi_dma_txrx(struct spi_anyka_slave *slave)
{
	int ret = 0;
	int retlen;
	u32 count;

	pr_info("%s  line:%d\n", __func__, __LINE__);
	
	switch(slave->xfer_dir) {
		/*
		case SPI_DIR_TXRX:
		{	
			//申请L2的空间
			//这里申请的只有spi总线0（AKSPI_BUS_NUM1）、1（AKSPI_BUS_NUM2）
			slave->l2buf_tid = l2_alloc(SPI_L2_TXADDR(slave));
			slave->l2buf_rid = l2_alloc(SPI_L2_RXADDR(slave));
			//出错处理
			if ((BUF_NULL == slave->l2buf_tid) || (BUF_NULL == slave->l2buf_rid))
			{
				printk("%s: l2_alloc failed!\n", __func__);
				pr_err("---------no!---------%d\n",__LINE__);
				ret = -EBUSY;
				goto txrx_ret;
			}
			
			while(count > 0) {
				hw->count = 0;
				hw->len = (count > MAX_XFER_LEN) ?	MAX_XFER_LEN : count;
				//spi同时读写数据函数--》L2dma 模式
				retlen = spi_dma_duplex(hw, hw->tx, hw->rx, hw->len);
				if(unlikely(retlen < 0)) {
					printk("spi master transfer data error!\n");
					ret = -EBUSY;
					goto txrx_ret;
				}
				hw->tx += retlen;
				hw->rx += retlen;
				count -= retlen;
			}
			
			break;
		}*/
		case SPI_DIR_TX:
		{
		    pr_info(" spi slave switch TX \n");
			slave->l2buf_tid = BUF_NULL;//无效的	L2缓冲区 ID
			count = slave->wrsize;
			//alloc L2 buffer
			slave->l2buf_tid = l2_alloc(SPI_L2_TXADDR(slave));
			if (unlikely(BUF_NULL == slave->l2buf_tid)) {
				pr_err("%s: l2_alloc failed!\n", __func__);
				return -EBUSY;
			}
			while(count > 0) {
				slave->count = 0;
				slave->wrsize = (count > MAX_XFER_LEN) ? MAX_XFER_LEN : count;
				retlen = spi_dma_write(slave, slave->sentp,slave->wrsize);
				if(unlikely(retlen < 0)) {
					pr_err("spi master read data error!\n");	
					ret = -EBUSY;
                
                    if(slave->l2buf_tid != BUF_NULL)
                     l2_free(SPI_L2_TXADDR(slave));
				}
				slave->sentp += retlen;
				count -= retlen;
			}

            if(slave->l2buf_tid != BUF_NULL)
             l2_free(SPI_L2_TXADDR(slave));

			break;
		}
		case SPI_DIR_RX:
		{
		    pr_info(" spi slave switch RX\n");
			slave->l2buf_rid = BUF_NULL;//无效的	L2缓冲区 ID
			count = slave->rdsize;
			//alloc L2 buffer
			slave->l2buf_rid = l2_alloc(SPI_L2_RXADDR(slave));
			if (unlikely(BUF_NULL == slave->l2buf_rid))
			{
				return -EBUSY;
			}	
			while(count > 0) {
				slave->count = 0;
				slave->rdsize = (count > MAX_XFER_LEN) ? MAX_XFER_LEN : count;
				slave->rdsize = count;
				retlen = spi_dma_read(slave, slave->recvp, slave->rdsize);
				if(unlikely(retlen < 0)) {					
					pr_err("spi slave read data error!\n");
					ret = -EBUSY;

                if(slave->l2buf_rid != BUF_NULL)
                 l2_free(SPI_L2_RXADDR(slave));
				}
				slave->recvp += retlen;
				count -= retlen;
			
			}	
            
            if(slave->l2buf_rid != BUF_NULL)
		       l2_free(SPI_L2_RXADDR(slave));
            
			break;
		}
	}
	pr_info("finish the spi dma transfer success.\n");

    return ret ? ret : slave->rdsize;// need optimization

}


/**
*  @brief       configure the slave register when start transfer data.
*  @author   	luo minjie
*  @email		luo_minjie@anyka.oa
*  @date        2022-07-13
*  @param[in]  *slave 
*  @return      void
*/
static void ak_spi_start_txrx(struct spi_anyka_slave *slave)
{
    u32 reg_value;
	enum spi_xfer_dir dir = slave->xfer_dir;
		
	sdbug("the spi transfer mode is %s.\n", (dir == SPI_DIR_TXRX) ? 
				"txrx" : (dir == SPI_DIR_RX)? "rx":"tx");
    reg_value = readl(slave->regs + AK_SPICON);
	switch(dir) {
		case SPI_DIR_TX:
		    reg_value &= ~AK_SPICON_TGDM;
   			reg_value |= AK_SPICON_ARRM;
			break;
		case SPI_DIR_RX:
			reg_value &= ~AK_SPICON_ARRM;
		    reg_value |= AK_SPICON_TGDM;
			break;
		/*
		case SPI_DIR_TXRX:
			reg_value &= ~AK_SPICON_TGDM;
			reg_value &= ~AK_SPICON_ARRM;
			break;
		*/
		default:
			break;
	}

	//slave 不需要配置,使用默认00
	/*configure the data wire*/
	/*配置数据的写*/

	//reg_value |= AK_SPICON_WIRE;
	/*
	//如果发送接收时4bit
	if(t->tx_nbits == SPI_NBITS_QUAD || t->rx_nbits == SPI_NBITS_QUAD)
	{
		reg_value |= 0x2<<16;//AK_SPICON_WIRE位为：10 :use 4-write data mode for master
	}
	//如果发送接收时2bit
	else if(t->tx_nbits == SPI_NBITS_DUAL || t->rx_nbits == SPI_NBITS_DUAL)
	{
		reg_value |= 0x1<<16;//AK_SPICON_WIRE位为：01 :use 2-write data mode for master
	}
	else
	{
		reg_value |= 0x0<<16;//AK_SPICON_WIRE位为：00 :use 1-write data mode for master
	}
	*/
    writel(reg_value, slave->regs + AK_SPICON);
}


/**
*  @brief       configure the slave register when stop transfer data.
*  @author   	luo minjie
*  @email		luo_minjie@anyka.oa
*  @date        2022-07-13
*  @param[in]   *slave
*  @return      void
*/
static void ak_spi_stop_txrx(struct spi_anyka_slave *slave)
{
	u32 reg_value;

    reg_value = readl(slave->regs + AK_SPICON);
	reg_value &= ~AK_SPICON_WIRE;	
    writel(reg_value, slave->regs + AK_SPICON);
}
	
/**
*  @brief       transfer a message
*  call proper function to complete the transefer
*  @author   		luo minjie
*  @email			luo_minjie@anyka.oa
*  @date        	2022-07-13
*  @param[in]   *slave
*  @return      int
*/
static int ak_spi_txrx(struct spi_anyka_slave *slave)
{
    int ret;
	
	slave->xfer_dir = (slave->wrbuf && slave->rdbuf) ? SPI_DIR_TXRX : 
				slave->wrbuf ? SPI_DIR_TX : SPI_DIR_RX;

	pr_info("%s xfer_dir 0:tx; 1:rx ; 2:txrx xfer_dir:%d\n",\
                                    __func__,slave->xfer_dir);

	ak_spi_start_txrx(slave);
	//使能中断配置
	akspi_slave_set_irq(slave, 1);
		
	if(ak_spi_use_dma(slave)) {
		ret = ak_spi_dma_txrx(slave);// need optimization
	}
	
	ak_spi_stop_txrx(slave);
	return 0; //return ret; need optimization
}


/*-------dma----------*/

static int akspi_slave_fasync(int fd, struct file *filp, int mode);


/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		How much space is free?
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @return      	free space in buffer
*/
/*
static int akspi_slave_space_free(struct spi_anyka_slave *slave)
{
	if (slave->readp == slave->recvp){
		return slave->rdsize - 1;
	}
	return ((slave->readp + slave->rdsize - slave->recvp) % slave->rdsize) - 1;
}
*/
/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		store data into buffer
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	slave->recvp
*  @param[in]   	*slave
*  @param[in]   	*buf
*  @param[in]   	count
*  @return      	How much data has been stored
*/
/*
static int akspi_slave_reveive_data(struct spi_anyka_slave *slave,
			char *buf, size_t count)
{
//	sdbug("%s\n", __func__);
	

	count = min(count, (size_t)akspi_slave_space_free(slave));
	if (slave->recvp >= slave->readp){
	
		count = min(count, (size_t)(slave->rdend - slave->recvp)); 
	}
	else {
		
		count = min(count, (size_t)(slave->readp - slave->recvp - 1));
	}

	memcpy(slave->recvp, buf, count);
	slave->recvp += count;
	if (slave->recvp == slave->rdend){
		slave->recvp = slave->rdbuf; 
	}

	return count;
}

*/

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		receive data int interupt
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @param[in]   	irq
*  @return      	irqreturn_t
*/
static irqreturn_t akspi_slave_int(int irq, void *dev_id)
{	
	u32 status_reg;
	u32 intren_reg;
	u32 value1;
	struct spi_anyka_slave *slave = (struct spi_anyka_slave *)dev_id;
	status_reg = __raw_readl(slave->regs + AK_SPISTA);
	intren_reg = __raw_readl(slave->regs + AK_SPIINT);
	value1 = __raw_readl(slave->regs + AK_SPICON);

	//less_word_int
	if((status_reg & (1<<10)) && (intren_reg & (1<<10)) )	
	{
		//pr_err("---|sla_less_word|----\n");
		SPI_SET_FLAG(spi_intr_flag,SPI_SLV_LESS_WORD_FLAG);	
	}
	
	/*--------dma-------*/
	if(ak_spi_use_dma(slave)) {
		if((status_reg & AK_SPISTA_TRANSF) == AK_SPISTA_TRANSF ) {
			//sdbug("spi transfer data have been finish.\n"); 
			SPI_SET_FLAG(spi_intr_flag,SPI_SLV_STA_TRANSF_FLAG);
			disable_imask(slave, AK_SPIINT_TRANSF);
			complete(&slave->done);		
		}
	}
	return IRQ_HANDLED;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		prepare receive data from master
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @return      	void
*/
/*
static  void akspi_slave_prepare_read(struct spi_anyka_slave *slave)
{
	u32 val;
 	unsigned long flags; 
	spin_lock_irqsave(&slave->regs_lock, flags);
	
	val = __raw_readl(slave->regs + AK_SPICON);
	val &= ~(AK_SPICON_ARRM);
	val |= AK_SPICON_TGDM;
	
	__raw_writel(val, slave->regs + AK_SPICON);
	
	akspi_slave_set_irq(slave, 1);

	spin_unlock_irqrestore(&slave->regs_lock, flags);
}
*/
/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		open slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*node
*  @param[in]   	*filp
*  @return      	fail or not
*/
static int akspi_slave_open(struct inode *node, struct file *filp)
{
	struct spi_anyka_slave *slave = container_of(node->i_cdev, struct spi_anyka_slave, cdev);

	if (mutex_lock_interruptible(&slave->slave_lock)){
		return -ERESTARTSYS;
	}

	nonseekable_open(node, filp);

	
	slave->openers++;
	filp->private_data = slave;
	mutex_unlock(&slave->slave_lock);
	sdbug("%s: openers=%d\n", __func__, slave->openers);
	return 0;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		relese slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*node
*  @param[in]   	*filp
*  @return      	fail or not
*/
static int akspi_slave_release(struct inode *node, struct file *filp)
{
	struct spi_anyka_slave *slave = container_of(node->i_cdev, struct spi_anyka_slave, cdev);

	akspi_slave_fasync(-1, filp, 0);
	mutex_lock(&slave->slave_lock);

	slave->openers--;
	if (!slave->openers){
		/* close spi controler */
		akspi_slave_set_irq(slave, 0);
		kfree(slave->rdbuf);
		slave->rdbuf = NULL;
	}

	filp->private_data = NULL;
	mutex_unlock(&slave->slave_lock);
	sdbug("%s: openers=%d\n", __func__, slave->openers);
	return 0;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		setup slave controler
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @param[in]   	*void
*  @return      	fail or not
*/
static int akspi_slave_setmode(struct spi_anyka_slave *slave)
{
	u16 spicon;

	sdbug("%s: set mode-------------.\n", __func__);
	spin_lock(&slave->regs_lock);
	spicon = __raw_readl(slave->regs + AK_SPICON);
	
	if (slave->mode & SPI_CPHA)
		spicon |= AK_SPICON_CPHA;
	else
		spicon &= ~AK_SPICON_CPHA;
	if (slave->mode & SPI_CPOL)
		spicon |= AK_SPICON_CPOL;
	else
		spicon &= ~AK_SPICON_CPOL;
		
	__raw_writel(spicon, slave->regs + AK_SPICON);
	spin_unlock(&slave->regs_lock);
	return 0;		
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		setup slave controler
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*slave
*  @param[in]   	*void
*  @return      	fail or not
*/
/*
static int akspi_slave_setclk(struct spi_anyka_slave *slave)
{
	unsigned int div;
	unsigned int hz = slave->max_speed_hz;
	unsigned long clk = clk_get_rate(slave->clk);
	pr_err("------clk:%ld----LINE:%d\n",clk,__LINE__);
	u16 spicon;
	
	sdbug("%s: set clk-------------.\n", __func__);
	spin_lock(&slave->regs_lock);
	spicon = __raw_readl(slave->regs + AK_SPICON);
	
	div = clk / (hz*2) - 1;
	if (div > 255){
		div = 255;
	}
	else if (div < 3){
		div = 3;
	}

	spicon &=~(0xff << 8);
	spicon |= div << 8;
	__raw_writel(spicon, slave->regs + AK_SPICON);
	spin_unlock(&slave->regs_lock);
	return 0;
}
*/


/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		ioctl slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*node
*  @param[in]   	*filp, cmd, arg
*  @return      	fail or not
*/
static long akspi_slave_ioctl(struct file *filp,	unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	int err = 0;
	u32	tmp;
	struct spi_anyka_slave *slave;

	sdbug("%s: cmd=%u\n", __func__, cmd);
	
	/* Check type and command number */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC)
		return -ENOTTY;

	/* Check access direction once here; don't repeat below.
	 * IOC_DIR is from the user perspective, while access_ok is
	 * from the kernel perspective; so they look reversed.
	 */
	if (_IOC_DIR(cmd) & _IOC_READ){
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	}
	
	if (err == 0 && _IOC_DIR(cmd) & _IOC_WRITE){
		err = !access_ok(VERIFY_READ,	(void __user *)arg, _IOC_SIZE(cmd));
	}
	
	if (err){
		return -EFAULT;
	}

	slave = filp->private_data;
	if (!slave){
		return -ESHUTDOWN;
	}

	mutex_lock(&slave->slave_lock);

	switch (cmd){
	 /* read requests */
	 case SPI_IOC_RD_MODE32:
		 retval = __put_user(slave->mode & SPI_MODE_MASK, (__u8 __user *)arg);
		 break;
	 case SPI_IOC_RD_LSB_FIRST:
		 retval = __put_user((slave->mode & SPI_LSB_FIRST) ?	1 : 0, (__u8 __user *)arg);
		 break;
	 case SPI_IOC_RD_BITS_PER_WORD:
		 retval = __put_user(slave->bits_per_word, (__u8 __user *)arg);
		 break;
	 case SPI_IOC_RD_MAX_SPEED_HZ:
		 retval = __put_user(slave->max_speed_hz, (__u32 __user *)arg);
		 break;
 
	 /* write requests */
	 case SPI_IOC_WR_MODE:
	 case SPI_IOC_WR_MODE32:
		 retval = __get_user(tmp, (u8 __user *)arg);
		 if (retval == 0) {
			 u8  save = slave->mode;
 
			 if (tmp & ~SPI_MODE_MASK) {
				 retval = -EINVAL;
				 break;
			 }
 
			 tmp |= slave->mode & ~SPI_MODE_MASK;
			 slave->mode = (u8)tmp;
			 retval = akspi_slave_setmode(slave);
			 if (retval < 0)
				 slave->mode = save;
			 else
				 dev_dbg(&slave->pdev->dev, "spi mode %02x\n", tmp);
		 }
		 break;
	 case SPI_IOC_WR_LSB_FIRST:
		 retval = -EINVAL;
		 break;
	 case SPI_IOC_WR_BITS_PER_WORD:
		 retval = 0;
		 break;
	 /*
	  case SPI_IOC_WR_RECEIVE_BYTE:
		 retval = __get_user(tmp, (__u32 __user *)arg);
	  	   pr_err("--retval:%d tmp:%d LINE:%d\n",retval,tmp,__LINE__);
		 if (retval == 0) {    
		 slave->rdsize = tmp;	
		 }
		 break;
		 */
	/*
	 case SPI_IOC_WR_MAX_SPEED_HZ:
		 retval = __get_user(tmp, (__u32 __user *)arg);
		 pr_err("--retval:%d tmp:%d LINE:%d\n",retval,tmp,__LINE__);
		 if (retval == 0) {
			 u32 save = slave->max_speed_hz;
             
			 slave->max_speed_hz = tmp;
			 retval = akspi_slave_setclk(slave);
			 if (retval < 0)
				 slave->max_speed_hz = save;
			 else
				 dev_dbg(&slave->pdev->dev, "%d Hz (max)\n", tmp);
		 }
		 break;
 	*/
	 default:
		 retval = -EINVAL;
		 break;
	 }
	 
	 mutex_unlock(&slave->slave_lock);
	 return retval;
 }

/**
*  @Copyright (C) 	Anyka 2022-07-13
*  @brief       	read slave device
*  @author   		luo minjie
*  @email			luo_minjie@anyka.oa
*  @date        	2022-07-13
*  @param[out]  	void
*  @param[in]   	*buf
*  @param[in]   	*filp
*  @return      	read data count
*/
static ssize_t akspi_slave_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct spi_anyka_slave *slave = filp->private_data;
	slave->rdsize=count;
	
	pr_info("%s  line:%d\n", __func__, __LINE__);

	//申请空间
	if (!slave->rdbuf){
		/* Alloc memory for receive data  */
		slave->rdbuf = kzalloc(slave->rdsize, GFP_KERNEL);
		if (!slave->rdbuf){
			mutex_unlock(&slave->slave_lock);
			return -ENOMEM;
		}
	}

	slave->rdend = slave->rdbuf + slave->rdsize;
	slave->readp = slave->recvp = slave->rdbuf; /* rd and wr from the beginning */
	//SPI transher
	ak_spi_txrx(slave);
	
	//获取互斥锁，引起的休眠可以被打断，适用于随时获取数据
	if (mutex_lock_interruptible(&slave->slave_lock)){
		return -ERESTARTSYS;
	}
	
	if (slave->recvp > slave->readp)	{
		/* return the data */
		count = min(count, (size_t)(slave->recvp - slave->readp));
	}
	else	{
		/* the write pointer has wrapped, return data up to end */	
		count = min (count, (size_t)(slave->rdend - slave->readp));
	}
	
	//与应用层交互数据
	if (copy_to_user(buf, slave->readp, count)) {
        pr_err("%s: copy_to_user fail line:%d\n", __func__, __LINE__);
		mutex_unlock(&slave->slave_lock);
		return -EFAULT;
	}
	slave->readp += count;
	if (slave->readp == slave->rdend){
		slave->readp = slave->rdbuf;
	}
	//释放空间，并指向NULL防止野指针
	kfree(slave->rdbuf);
	slave->rdbuf=NULL;
	
	mutex_unlock(&slave->slave_lock);
    
	pr_info("%s success line:%d\n", __func__, __LINE__);
    
	return count;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       	write slave device
*  @author   		luo minjie
*  @email			luo_minjie@anyka.oa
*  @date        	2022-07-13
*  @param[out]  	void
*  @param[in]   	*buf
*  @param[in]   	*filp, count, pos
*  @return      	write data count
*/
static ssize_t akspi_slave_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{

	struct spi_anyka_slave *slave = filp->private_data;
	slave->wrsize=count;

    pr_info("%s  line:%d\n", __func__, __LINE__);
    
	if (mutex_lock_interruptible(&slave->slave_lock)){
		return -ERESTARTSYS;
	}
	
	if (!slave->wrbuf){
		/* Alloc memory for receive data  */
		slave->wrbuf = kzalloc(slave->wrsize, GFP_KERNEL);
		if (!slave->wrbuf){
			mutex_unlock(&slave->slave_lock);
			pr_err("%s: %d\n", __func__, __LINE__);
			return -ENOMEM;
		}
	}

	slave->wrend = slave->wrbuf + slave->wrsize;
	slave->sentp = slave->writep = slave->wrbuf; /* rd and wr from the beginning */
	//从应用层获取数据
	if(copy_from_user(slave->writep, buf, slave->wrsize)) {
        pr_err("%s: copy_from_user fail line:%d\n", __func__, __LINE__);
		mutex_unlock(&slave->slave_lock);
		return -EFAULT;
	}
	//SPI transher
	ak_spi_txrx(slave);
	
	if (slave->sentp > slave->writep)	{
		
		count = min(count, (size_t)(slave->sentp - slave->writep));
	}
	else	{
		
		count = min (count, (size_t)(slave->wrend - slave->writep));
	}
		
	slave->writep += slave->wrsize;
	if (slave->writep == slave->wrend){
		slave->writep = slave->wrbuf;
	}	
	
	kfree(slave->wrbuf);
	slave->wrbuf=NULL;

	mutex_unlock(&slave->slave_lock);
    
	pr_info("%s success line:%d\n", __func__, __LINE__);
    
	return count;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		poll slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*table
*  @param[in]   	*filp
*  @return      	fail or not
*/
unsigned int akspi_slave_poll (struct file *filp, struct poll_table_struct *table)
{
	struct spi_anyka_slave *slave = filp->private_data;
	int mask = 0;

	sdbug("%s: %d\n", __func__, __LINE__);
	mutex_lock(&slave->slave_lock);
	poll_wait(filp, &slave->readq, table);
	poll_wait(filp, &slave->writeq, table);

	if (slave->recvp != slave->readp){
		mask |= POLLIN | POLLRDNORM;	/* readable */
	}

	if (slave->sentp != slave->writep){
		mask |= POLLOUT | POLLWRNORM;	/* writable */
	}
	mutex_unlock(&slave->slave_lock);
	sdbug("%s: %d\n", __func__, __LINE__);
	return mask;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		fasync slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*node
*  @param[in]   	*filp, fd, mode
*  @return      	fail or not
*/
static int akspi_slave_fasync(int fd, struct file *filp, int mode)
{
	struct spi_anyka_slave *slave = filp->private_data;

	return fasync_helper(fd, filp, mode, &slave->async_queue);
}

/**
*  @Copyright (C) 	Anyka 2022
*  @brief       	initialize slave device
*  @author   		luo minjie
*  @email			luo_minjie@anyka.oa
*  @date        	2022-07-13
*  @param[out]  	void
*  @param[in]   	*slave
*  @param[in]   	void
*  @return      	void
*/
static void akspi_slave_initial_setup(struct spi_anyka_slave *slave)
{
	u32 value = 0;

	spin_lock(&slave->regs_lock);
	//使能spi,slave模式默认为0 ，可不用配置
	value =  DFT_CON ;
	__raw_writel(value, slave->regs + AK_SPICON);
	spin_unlock(&slave->regs_lock);
	
	akspi_slave_setmode(slave);
	sdbug("value=%08x, reg=%08x\n",value, __raw_readl(slave->regs + AK_SPICON));
	//清中断
	akspi_slave_set_irq(slave, 0);

}

static const struct file_operations slave_ops = {
	.owner	= THIS_MODULE,
	.open	= akspi_slave_open,
	.release 	= akspi_slave_release,
	.unlocked_ioctl = akspi_slave_ioctl,
	.read	= akspi_slave_read,
	.write	= akspi_slave_write,
	.poll		= akspi_slave_poll,
	.fasync	= akspi_slave_fasync,
};

/**
*  @Copyright (C) 	Anyka 2022
*  @brief       		probe slave device
*  @author   		luo minjie
*  @email			luo_minjie@anyka.oa
*  @date        	2022-07-13
*  @param[out]  	slave
*  @param[in]   	*pdev
*  @param[in]   	*pdata
*  @return      	fail or not
*/
static int ak_spi_slave_probe(struct platform_device *pdev)
{
	struct spi_anyka_slave *slave = NULL;
	struct resource *res;
	int err = 0;

	/* Allocate Slave with space for drv_data and null dma buffer */
	slave = kzalloc(sizeof(struct spi_anyka_slave), GFP_KERNEL);
	if (!slave) {
		dev_err(&pdev->dev, "cannot alloc mem\n");
		err = -ENOMEM;
		goto err_nomem;
	}
    
    slave->tmp1= 100;
    slave->tmp2= 200;
    
	slave->pdev 	= pdev;
	cdev_init(&slave->cdev, &slave_ops);
	slave->cdev.owner = THIS_MODULE;
	
	spin_lock_init(&slave->regs_lock);
	mutex_init(&slave->slave_lock);
	init_waitqueue_head(&slave->readq);
	init_waitqueue_head(&slave->writeq);
	slave->mode = SPI_MODE_0;
	slave->bits_per_word = 8;

	//spi dma transfer
	slave->xfer_mode = AKSPI_XFER_MODE_DMA;
	slave->bus_num = AKSPI_BUS_NUM3;// need optimization
    init_completion(&slave->done); 
	/* get basic io resource and map it */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "Cannot get IORESOURCE_MEM\n");
		err = -ENOENT;
		goto err_no_iores;
	}

	slave->ioarea = request_mem_region(res->start, resource_size(res), pdev->name);
	if (slave->ioarea == NULL) {
		dev_err(&pdev->dev, "Cannot reserve region\n");
		err = -ENXIO;
		goto err_no_iores;
	}

	slave->regs = ioremap(res->start, resource_size(res));
	if (!slave->regs) {
		err = -ENOMEM;
		goto err_no_iomap;
	}
	printk(KERN_INFO "SPI-Slave:map regs = %08x\n", (int)slave->regs);

	/* Attach to IRQ */
	slave->irq = platform_get_irq(pdev, 0);
	if (slave->irq < 0) {
		dev_err(&pdev->dev, "No IRQ specified\n");
		err = -ENOENT;
		goto err_no_irq;
	}
	printk(KERN_INFO "SPI-Slave:get irq = %04x\n", (int)slave->irq);
	
	err = request_irq(slave->irq, akspi_slave_int, 0, pdev->name, slave);
	if (err < 0) {
		dev_err(&pdev->dev, "can not get IRQ\n");
		goto err_no_irq;
	}
	printk(KERN_INFO "SPI-Slave: request IRQ: %04x\n", slave->irq);

	err = cdev_add(&slave->cdev, MKDEV(slave_major, slave_minor), 1);
	if (err){
		dev_err(&pdev->dev, "cannot add cdev\n");
		err = -ENOMEM;
		goto err_register;
	}
	printk(KERN_INFO "SPI-Slave: register with char device framework\n");

	if (IS_ERR(device_create(slave_class, &pdev->dev,
				MKDEV(slave_major, slave_minor),
				slave, "spi_slave.%u", slave_minor))){
		dev_err(&pdev->dev, "cannot device_create\n");
	}
				
	//使能clk
	slave->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(slave->clk)) {
		dev_err(&pdev->dev, "No clock for device\n");
		err = PTR_ERR(slave->clk);
		goto err_no_clk;
	}
	clk_prepare_enable(slave->clk);
	
	/* spi dma transfer alloc*/
	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	if (err) {
		dev_warn(slave->dev, "Unable to set dma mask\n");
		return err;
	}
    slave->txbuffer = dmam_alloc_coherent(&pdev->dev, SPI_DMA_MAX_LEN,
			&slave->txdma_buffer, GFP_KERNEL);
	if (!slave->txbuffer)
		return -ENOMEM;
    slave->rxbuffer = dmam_alloc_coherent(&pdev->dev, SPI_DMA_MAX_LEN,
			&slave->rxdma_buffer, GFP_KERNEL);
	if (!slave->rxbuffer)
		return -ENOMEM;
	
	//配置控制寄存器
	akspi_slave_initial_setup(slave);
	
	platform_set_drvdata(pdev, slave);
	printk("Ak spi slave initialized!\n");
	return 0;

 err_no_clk:
	device_destroy(slave_class, MKDEV(slave_major, slave_minor));
	cdev_del(&slave->cdev);
 err_register:
	free_irq(slave->irq, slave);
 err_no_irq:
	iounmap(slave->regs);
 err_no_iomap:
	release_resource(slave->ioarea);
	kfree(slave->ioarea);
 err_no_iores:
 //err_no_pdata: 
 err_nomem:
	return err;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		remove slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*pdev
*  @param[in]   	*slave
*  @return      	void
*/
static int __exit ak_spi_slave_remove(struct platform_device *pdev)
{
	struct spi_anyka_slave *slave = platform_get_drvdata(pdev);
	int minor = MINOR(slave->cdev.dev);

	if (!slave)
		return 0;
	//disable  clk
 	clk_disable(slave->clk);
	
	platform_set_drvdata(pdev, NULL);
	device_destroy(slave_class, MKDEV(slave_major, minor));
	cdev_del(&slave->cdev);

	/* Release IRQ */
	free_irq(slave->irq, slave);
	iounmap(slave->regs);
	release_resource(slave->ioarea);
	kfree(slave->ioarea);
	kfree(slave);

	return 0;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		suspend and resume slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	*dev
*  @param[in]   	state
*  @return      	fail or not
*/
#ifdef CONFIG_PM
static int ak_spi_slave_suspend(struct device *dev, pm_message_t state)
{
	struct spi_anyka_slave *slave = dev_get_drvdata(dev);
	pr_info("%s: suspend\n", slave->pdev->name);
	clk_disable_unprepare(slave->clk);
	return 0;
}

static int ak_spi_slave_resume(struct device *dev)
{
	struct spi_anyka_slave * slave= dev_get_drvdata(dev);
	pr_info("%s: resume\n",  slave->pdev->name);
	clk_prepare_enable(slave->clk);
	return 0;
}
#else
#define ak_spi_slave_suspend NULL
#define ak_spi_slave_resume NULL
#endif /* CONFIG_PM */

static const struct of_device_id ak_spi_slave_match[] = {
	{ .compatible = "anyka,ak-slave", },
	{}
};

static struct platform_driver akspi_slave_driver = {
	.remove		= __exit_p(ak_spi_slave_remove),
	.probe		= ak_spi_slave_probe,
	.driver = {
		.name	= "akspi-spi",
		.of_match_table	= of_match_ptr(ak_spi_slave_match),
		.owner	= THIS_MODULE,
		.suspend = ak_spi_slave_suspend,
		.resume 	= ak_spi_slave_resume,
	},
};

static ssize_t slave_show_version(struct class *cls, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "spi-1.0.01\n");
}
static CLASS_ATTR(version, 0444, slave_show_version, NULL);

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		init slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	void
*  @param[in]   	void
*  @return      	fail or not
*/
static int __init ak_spi_slave_init(void)
{
	int ret;
	dev_t dev;

	slave_class = class_create(THIS_MODULE, "spi_slave");
	if (IS_ERR(slave_class)){
		ret = PTR_ERR(slave_class);
		printk("%s:class create fail!\n", __func__);
		goto err;
	}

	ret = class_create_file(slave_class, &class_attr_version);
	if (ret){
		printk("%s:class create file fail!\n", __func__);
		goto err_class;
	}

	ret = alloc_chrdev_region(&dev, 0, SLAVE_MAX_MINOR, "spi_slave");
	if (ret){
		printk("%s:alloc chrdev fail!\n", __func__);
		goto err_chrdev;
	}
	slave_major = MAJOR(dev);

	ret = platform_driver_register(&akspi_slave_driver);
	if (ret){
		printk("%s:platform_driver_register fail!\n", __func__);
		goto err_plat;
	}

	return 0;
	
err_plat:
	unregister_chrdev_region(dev, SLAVE_MAX_MINOR);
err_chrdev:
	class_remove_file(slave_class, &class_attr_version);
err_class:
	class_destroy(slave_class);
err:
	return ret;
}

/**
*  @Copyright (C) 	Anyka 2012
*  @brief       		exit slave device
*  @author   		Gao wangsheng
*  @email		gao_wangsheng@anyka.oa
*  @date        	2012-10-23
*  @param[out]  	void
*  @param[in]   	void
*  @param[in]   	void
*  @return      	void
*/
static void __exit ak_spi_slave_exit(void)
{
	platform_driver_unregister(&akspi_slave_driver);
	unregister_chrdev_region(MKDEV(slave_major, 0), SLAVE_MAX_MINOR);
	class_remove_file(slave_class, &class_attr_version);
	class_destroy(slave_class);
}

MODULE_AUTHOR("Wangsheng Gao");
MODULE_DESCRIPTION("Anyka SPI Slave Contoller");
MODULE_VERSION("1.0.00");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:AK-spi-slave");
module_init(ak_spi_slave_init);
module_exit(ak_spi_slave_exit);

