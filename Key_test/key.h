


#ifndef key_H
#define key_H

extern unsigned char key_pressed;//按键是否按下标志位
extern unsigned char key_val;//按键值存储变量
extern unsigned char key_flag;//按键事件标志

extern void key_init();
extern void check_key();
extern void key_event();

#endif
