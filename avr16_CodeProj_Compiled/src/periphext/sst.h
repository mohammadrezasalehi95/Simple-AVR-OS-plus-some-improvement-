/* SST 49F020 */


#define c_SSTBLKERASE	0
#define c_SSTSECERASE	1
#define c_SSTCHIPERASE	2

void f_SSTWrite(INT32U addr, INT8U data);
INT16U f_SSTID(void);
void f_SSTErase(INT16U base_addr, INT8U type);
