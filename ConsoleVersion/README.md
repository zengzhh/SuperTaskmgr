supertasksquash 为独立的控制台程序，方便调试。 

进程刷新时，RefreshProcessInfo 中： 

1、对原有内存分配方式进行了优化，避免每出现一个新进程，都去执行ProcessContainer.NewData()。 

2、ReleaseSaveNameBuffSize 和 GetSaveNameBuffSize 可以复用之前的进程名使用到的内存。
