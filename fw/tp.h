#ifndef _TP_H_
#define _TP_H_


//----- DEFINES -----
#define MIN_PRESSURE    200

#define CAL_POINT_X1    20 //20
#define CAL_POINT_Y1    20 //20
#define CAL_POINT1      {CAL_POINT_X1,CAL_POINT_Y1}

#define CAL_POINT_X2    LCD_WIDTH-20 //300
#define CAL_POINT_Y2    LCD_HEIGHT/2 //120
#define CAL_POINT2      {CAL_POINT_X2,CAL_POINT_Y2}

#define CAL_POINT_X3    LCD_WIDTH/2   //160
#define CAL_POINT_Y3    LCD_HEIGHT-20 //220
#define CAL_POINT3      {CAL_POINT_X3,CAL_POINT_Y3}


typedef struct 
{
  uint_least32_t x;
  uint_least32_t y;
} CAL_POINT;

typedef struct 
{
  uint_least32_t a;
  uint_least32_t b;
  uint_least32_t c;
  uint_least32_t d;
  uint_least32_t e;
  uint_least32_t f;
  uint_least32_t div;
} CAL_MATRIX;


#ifdef TP_SUPPORT


//----- PROTOTYPES -----
uint_least16_t                         tp_getz(void);
uint_least16_t                         tp_gety(void);
uint_least16_t                         tp_getx(void);
uint_least8_t                          tp_calmatrix(CAL_POINT *lcd, CAL_POINT *tp);
CAL_MATRIX*                            tp_getmatrix(void);
void                                   tp_setmatrix(uint_least32_t a, uint_least32_t b, uint_least32_t c, uint_least32_t d, uint_least32_t e, uint_least32_t f, uint_least32_t div);
uint_least16_t                         tp_rawz(void);
uint_least16_t                         tp_rawy(void);
uint_least16_t                         tp_rawx(void);
uint_least16_t                         tp_read(void);
void                                   tp_init(void);


#endif //TP_SUPPORT

#endif //_TP_H_
