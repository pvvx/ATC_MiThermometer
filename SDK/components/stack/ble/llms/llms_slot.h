/*
 * llms_slot.h
 *
 *  Created on: 2019-5-24
 *      Author: Administrator
 */

#ifndef LLMS_SLOT_H_
#define LLMS_SLOT_H_







#define			SLOT_MAX_NUM									64


/*******************************************************************************
 *
 * BIT(7)	BIT(6)	BIT(5)	BIT(4)	BIT(3)	BIT(2)	   ( BIT(1)	BIT(0) )
 *  M3		 M2		  M1	 S0		 SCAN	 ADV    	conn_handle_idx
 ******************************************************************************/
#define			SLOT_TASK_ADV									BIT(2)
#define			SLOT_TASK_SCAN									BIT(3)
#define			SLOT_TASK_SLAVE									BIT(4)
#define			SLOT_TASK_MASTER1								BIT(5)
#define			SLOT_TASK_MASTER2								BIT(6)
#define			SLOT_TASK_MASTER3								BIT(7)

#define			SLOT_TASK_MASTER								(SLOT_TASK_MASTER1 | SLOT_TASK_MASTER2 | SLOT_TASK_MASTER3)
#define 		SLOT_TASK_CONN									(SLOT_TASK_SLAVE | SLOT_TASK_MASTER)




#define			SLOT_UPDT_SLAVE_SYNC							BIT(1)
#define			SLOT_UPDT_SLAVE_SYNC_DONE						BIT(2)
#define			SLOT_UPDT_SLAVE_TERMINATE						BIT(3)

#define			SLOT_UPDT_MASTER_CONN							BIT(4)
#define			SLOT_UPDT_MASTER_TERMINATE						BIT(5)
//#define			SLOT_UPDT_MASTER_TERMINATE_ALL					BIT(6)


//#define			SLOT_UPDT_ADV_DISBALE							BIT(6)
//#define			SLOT_UPDT_ADV_ENABLE							BIT(7)







#define			SLOT_DURATION_SET_SCAN							1
#define			SLOT_DURATION_ADV								3   //ADV use 3*1.25mS=3.75mS most(may change later, depend on actual test value)
#define			SLOT_DURATION_SLAVE_COMMON						3
#define			SLOT_DURATION_MASTER_COMMON						2



#define			PKT_INIT_AFTER_RX								550   // at least: 352+150
#define			BOUNDARY_MARGIN_INIT							900	  // PKT_INIT_AFTER_RX + slot_update_rebuild_allocate running potential biggest time
#define			BOUNDARY_MARGIN_COMMON							300




enum{
	CLOCK_16M_SYS_TIMER_CLK_312US  = 5000,   //312.5*816
	CLOCK_16M_SYS_TIMER_CLK_625US  = 10000,  //625*16
	CLOCK_16M_SYS_TIMER_CLK_1250US = 20000,  //1250*16
};


typedef enum{
	SLOT_PRIORITY_LOW		=	0,
	SLOT_PRIORITY_MID		=	100,
	SLOT_PRIORITY_HIGH		=	200,
}slot_pri_t;




typedef unsigned char slot_type ;
#define SLOT_TYPE_THRES				  256

typedef struct{

	u8 	num_cur;
	u8 	task_mask;
	u8	update;
	u8 	rebuild;

	u8	slot_task_irq;    			 	//only used for insert Scan in slot gap
	slot_type 	slot_idx_irq_real; //used for timing calculate when initiate, see the explanations in blms_m_connect
	slot_type 	slot_idx_irq;
	slot_type	slot_idx_end;
	slot_type 	slot_mark_adv;
	slot_type 	slot_mark_scan;
	slot_type	slot_idx_next;
	slot_type	slot_idx_start;   // start of a new slot timing

	slot_type 	slot_mark_conn[BLMS_MAX_CONN_NUM]; //s0 m1 m2 m3




	u8	slot_duration[BLMS_MAX_CONN_NUM];
	u8	slot_interval[BLMS_MAX_CONN_NUM];
	u8	slot_drop_num[BLMS_MAX_CONN_NUM];
	u8	priority	 [BLMS_MAX_CONN_NUM];




	u32 slot_tick_irq;
	u32	slot_tick_next;
	u32	slot_start_adv;
	u32	slot_start_scan;




	u32	slot_start_conn[BLMS_MAX_CONN_NUM];

	u32	slot_tick_conn[BLMS_MAX_CONN_NUM];

	u32	slot_tick_irq_real;  //used for timing calculate when initiate, see the explanations in blms_m_connect

}slot_st_t;

extern slot_st_t	bltSlot;





extern u8	slot_map[];


/************************************ User Interface  ******************************************************/




/*********************************** Stack Interface, user can not use!!! **********************************/
int		blt_llms_slotUpdateRebuildAllocate(void);

int		blt_llms_startSlotTiming(void);
int 	blt_llms_rebuildSlotTimingAdv(void);
int 	blt_llms_rebuildSlotTimingScan(void);
int 	blt_llms_findAdvSlotAvailable(void);

u32 	blt_llms_findFirstAvailableStartSlot(slot_type mark, u16 interval);

static inline void blt_slot_setConnSlotPriority(u8 conn_idx, slot_pri_t pri){
	bltSlot.priority[conn_idx] = pri;
}

static inline u8 blt_slot_getConnSlotPriority(u8 conn_idx){
	return bltSlot.priority[conn_idx];
}

static inline void blt_slot_increaseConnSlotPriority(u8 conn_idx){  //increase priority due to packet loss & previous slot drop ...
	bltSlot.priority[conn_idx] = 0;
}

static inline void blt_slot_clearConnSlotPriority(u8 conn_idx){
	bltSlot.priority[conn_idx] = 0;
}



static inline void blt_slot_addTask(u8 task){
	bltSlot.task_mask |= (task);
}

static inline void blt_slot_removeTask(u8 task){
	bltSlot.task_mask &= ~(task);
}

static inline void blt_slot_clearTask(void){
	bltSlot.task_mask = 0;
}

static inline void blt_slot_addUpdate(u8 updt){
	bltSlot.update |= (updt);
}

static inline void blt_slot_removeUpdate(u8 updt){
	bltSlot.update &= ~(updt);
}




#endif /* LLMS_SLOT_H_ */
