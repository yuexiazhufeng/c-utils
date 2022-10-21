/*
函数原型：
	int asprintf(char **strp, const char *fmt, ...);
使用场景：
	不知道该申请多大内存空间，用来保存字符串时，可以用这个函数。
这个例程在linux环境编译运行正常，可能不适合windows环境。

ps: asprintf 函数不是标准c库函数，是gcc实现的。
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	int i, ret;
	char *str;
	for (i = 1; i < argc; i++){
		if (strcmp("-s", argv[i]) == 0){
			if (++i < argc){
				ret = asprintf(&str, "input str is: %s", argv[i]);
				printf("str len: %ld\n%s\n", str);
				free(str);
			}
		}
	}
	return 0;
}
