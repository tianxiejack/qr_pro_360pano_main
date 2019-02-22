#ifndef _GST_RING_BUFFER_
#define _GST_RING_BUFFER_

/*初始化，分配空间*/
void create_ring_buffer();
/*不用之后，释放空间*/
void destory_ring_buffer();

/*写数据，到分配的空间*/
void put_ring_buffer(char *pData, int datalen);

/*读数据，获取读数据空间的信息*/
int get_ring_buf();
/*读数据，获取读数据空间的地址*/
char* get_ring_buf_ptr();
/*读数据，获取该空间的数据长度*/
int get_ring_buf_length();
/*读完数据，释放该空间的使用权*/
void get_ring_buf_release();

#endif
