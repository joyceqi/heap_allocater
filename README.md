# heap_allocater
这是一个简单的堆分配器，基于隐式空闲链表实现，分配时采用首次适配策略，并使用了立即边界标记合并方式。


代码包括：    
memlib.c  存储器系统模型，目的在于允许我们在不干涉已经存在的系统层malloc包的情况下运行分配器         
mm.c      分配器，包括初始化、malloc和free的实现     
test.c    用于测试的程序     


可通过编译和链接memlib.c, mm.c到测试程序中，来查看效果：     
gcc -static -m32 test.c memlib.c mm.c -o test
