//读者-写者问题，读者优先实现
#include<pthread.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>

#define TIME_READ 1
#define TIME_WRITE 5
#define TIME_REST 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_r = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t db = PTHREAD_MUTEX_INITIALIZER;
pthread_t readers[6];
pthread_t writers[2];
unsigned rc = 0;
int data = 0;

//模拟读操作
void read_data_base(){
    pthread_mutex_lock(&mutex_r);	
    std::cout<<"Read "<<data<<std::endl;
    pthread_mutex_unlock(&mutex_r);
}

//模拟写操作
void write_data_base(){
    data = rand()%100;
    std::cout<<"Write "<<data<<std::endl;
}

//读者线程
void* reader(void* r){
    while(true){

        //获取对rc的互斥访问权
        pthread_mutex_lock(&mutex);

        //读者数加一
        ++rc;

        //若为等待队列中的第一个读者，
        //获取对data的互斥访问权
        if(rc == 1) pthread_mutex_lock(&db);

        //释放对rc的互斥访问权
        pthread_mutex_unlock(&mutex);

        read_data_base();
        sleep(TIME_READ);
        
        //获取对rc的互斥访问权
        pthread_mutex_lock(&mutex);

        //读者数减一
        --rc;

        //若为等待队列中最后一个读者，
        //释放对data的互斥访问权
        if(rc == 0) pthread_mutex_unlock(&db);

        //释放对rc的互斥访问权
        pthread_mutex_unlock(&mutex);

        sleep(TIME_REST);
    }
}

//写者线程
void* writer(void* w){
    while(true){
        
        //获取对data的互斥访问权
        pthread_mutex_lock(&db);

        write_data_base();
        sleep(TIME_WRITE);
        
        //释放对data的互斥访问权
        pthread_mutex_unlock(&db);

        sleep(TIME_REST);
    }
}

int main(){

    //生成读写序列
    pthread_create(&writers[0], NULL, writer, NULL);
    for(unsigned i = 0;i < 3;++i)
        pthread_create(&readers[i], NULL, reader, NULL);
    pthread_create(&writers[1], NULL, writer, NULL);
    for(unsigned i = 3;i < 6;++i)
	pthread_create(&readers[i], NULL, reader, NULL);
    
    //主线程保持休眠不退出
    while(true)
        sleep(10);

    return 0;
}    
    	        
