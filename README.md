# heap_allocater
这是一个简单的堆分配器，基于隐式空闲链表实现，分配时采用首次适配策略，并使用了立即边界标记合并方式。

隐式空闲链表：在每个堆块头部中保存该块的大小，从而空闲块能够通过头部的大小字段来隐含地连接着；通过遍历堆中所有的块，即可间接地遍历整个空闲块的集合。

首次适配：从头开始搜索空闲链表，选择第一个合适的空闲块，若空闲块还有剩余则需进行分割，剩下的成为新的空闲块。还有其他分配策略：
  下一次适配策略：每次搜索从上一次查询结束的地方开始；
  最佳适配策略：检查每个空闲块，选择适合所需请求大小的最小空闲块。
  
标记合并：当释放块时，需要合并前后相邻的空闲块，对于后相邻块可通过当前块大小来找到，但前相邻块只能从堆头遍历获得，因此为了能找到前相邻块，在每个堆块的尾部添加一个脚部（边界标记），它是头部的一个副本（包含块大小和是否空闲标志位）；从而要找前相邻块只需通过访问前相邻块的脚部来获取前块的大小即可实现。

立即合并：当释放块时，立即判断前后相邻块是否空闲，一旦空闲则合并；而不是再等到某个稍晚的时候再合并空闲块，如直到某个分配请求失败时才去合并所有空闲块。

代码包括：

memlib.cpp  存储器系统模型，目的在于允许我们在不干涉已经存在的系统层malloc包的情况下运行分配器    
mm.cpp      分配器，包括初始化、malloc和free的实现     
test.cpp    用于测试的程序     


可通过编译和链接memlib.cpp, mm.cpp到测试程序中，来查看效果：
g++ -static -m32 test.cpp memlib.cpp mm.cpp -o test
