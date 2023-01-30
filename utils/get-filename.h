//使用strrchr,反向查找第一个 / 的位置
#define __FILENAME__ (strrchr(__FILE__, '/') ? \
		strrchr(__FILE__, '/') + 1 : __FILE__)