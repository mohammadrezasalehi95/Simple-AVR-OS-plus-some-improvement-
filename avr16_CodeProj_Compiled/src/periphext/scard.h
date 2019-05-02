
#define c_CARD_NONE     0
#define c_CARD_READ     3
#define c_CARD_DBNCE     4
#define c_CARD_IDBAD    1
#define c_CARD_IDOK     2

void f_Init_SCard(void);
INT8U f_SCard_CheckCardIn(void);
INT8U f_SCard_MatchID(void);
void f_SCard_CCLK(void);
void f_SCard_RstCard(void);
void f_SCard_Read(INT8U * p_data);