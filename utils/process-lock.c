/*

*/
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h> //fork()
#include <sys/wait.h> //waitpid()
#include <sys/shm.h>
#include <assert.h>
#include <stdio.h> //printf()

/**
 *  创建共享内存，用于后续获取该共享内存，以及销毁该共享内存
 *  projId 定义的该共享内存序号
 *  size 共享内存大小
 *  return shmHandle 共享内存句柄
 */
static const int create_flag(const int projId, const unsigned int size) {
    // 生成key
    const char* filePath = "./";
    key_t key = ftok(filePath, projId);

    // 创建共享内存空间
    const int shmHandle = shmget(key, size, IPC_CREAT | 0666);

    return shmHandle;
}

// 封装进程锁结构体
typedef struct {
    pthread_mutex_t lock;
    pthread_mutexattr_t lock_attr;
    int shmHandle;
} mutex_package_t;

// 初始化进程锁结构体
static const int init_lock(void* pthis) {
    mutex_package_t* mp = (mutex_package_t*)pthis;
    // 初始化锁状态，设置状态状态为——进程共享
    pthread_mutexattr_init(&(mp->lock_attr));
    pthread_mutexattr_setpshared(&(mp->lock_attr), PTHREAD_PROCESS_SHARED);
    // 用锁状态来初始化锁
    pthread_mutex_init(&(mp->lock), &(mp->lock_attr));

    return 0;
}

// 在共享内存上定义进程锁结构体并且返回其位置
mutex_package_t* create_mutex_package(const int projID) {
    const int shmHandle = create_flag(projID, sizeof(mutex_package_t));
    mutex_package_t* mp = (mutex_package_t*)shmat(shmHandle, NULL, SHM_R | SHM_W);
    mp->shmHandle = shmHandle;

    assert(init_lock(mp) == 0);

    return mp;
}

// 销毁进程锁结构体，利用其FLAG变量索引到其占用的共享内存并销毁
const int destory_mutex_package(mutex_package_t* mp) {
    // 销毁锁和锁状态
    pthread_mutex_destroy(&(mp->lock));
    pthread_mutexattr_destroy(&(mp->lock_attr));

    // 释放共享内存
    assert(shmctl(mp->shmHandle, IPC_RMID, NULL) == 0);

    return 0;
}

void* main_thread (void* arg);

mutex_package_t* mp;
int main (int argc, char *argv[])
{
    // my_lock_init(&verify_mutex_api);
    // my_lock_init("/tmp/aero.lock");
    
    // 创建自定义进程锁
    mp = create_mutex_package(111);

    pid_t ret = fork();
    if(0 == ret) {
        printf("into sub process......\n");
        (void)main_thread(NULL);
    } else if (ret > 0){
        int status;
        printf("into main process.....\n");
        (void)main_thread(NULL);
        waitpid(ret, &status, 0);
    } else {
        printf("[%d]: %s",__LINE__, strerror(errno));
    }
    return 0;
}

void* main_thread (void* arg)
{
    pthread_mutex_lock(&(mp->lock));
	
	printf("process start success\n");

    pthread_mutex_unlock(&(mp->lock));
}
