#ifndef __LIST_H__
#define __LIST_H__


#include "system_config.h"
/***************************************************************************************************
链表本质上维护的是next和prev指针，故可把这一部分独立出来，把操作next、prev的部分独立出来
date部分在链表操作时无用，故可不用关心具体链表里存放的是什么数据，也不用关系HEAD所在的位置
唯一要注意的是怎样通过prev、next获取NODE的地址，即struct HEAD怎么转换成struct NODE
      HEAD <--|          NODE
	          |    |->|--HEAD--|    ->
<--|--prev--| |----+ -|--prev--|    |
   |--next--|------|  |--next--|----|
                      |--date--|
***************************************************************************************************/


struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};
	
#endif
