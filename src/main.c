#include <stdio.h>
#include <stdlib.h>
#include "stimer.h"
#include "wrio.h"
#include "usart_monitor.h"
#include "psx.h"
#include "car.h"
#include "pwm.h"
#include "led.h"

/* psx按键处理 */
static void handle_psx(void);

// Cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char* argv[]) {
    /**********************/
    /* 初始化             */
    stimer_init();
    wrio_init();
    usart_monitor_init();
    psx_init();
    car_init();
    pwm_init();

    /**********************/
    /* 主循环             */
    StimerRun(TIMER_PSX, ms2tick(3000));    /* 3秒后再查询psx按键 */
    beep(1);                                /* 开机蜂鸣器响1下 */
    while (1) {
        led_event();                        /* led灯事件 */

        beep_event();                       /* 蜂鸣器事件 */

        if (StimerCheck(TIMER_PSX)) {
            StimerRun(TIMER_PSX, ms2tick(50));
            handle_psx();
        }
    }
}
#pragma GCC diagnostic pop

/**
 * @brief psx按键处理.
 */
static void handle_psx(void) {
    /***************/
    /* 查询psx按键 */
    if (!psx_query()) {
        return;                             /* PSX遥控器联接失败 */
    }

    /****************/
    /* 串口调试助手 */
    if (psx_changed()) {                    /* 如果有按键变化, 串口发送数据 */
        usart_monitor_send(psx_buffer(), 9);
    }

    /***************/
    /* 处理psx按键 */
    if (psx_button_released(PSXB_TRIANGLE)) {
        car_switch_speed();                 /* 切换速度 */
    }
    if (psx_button_released(PSXB_CROSS)) {
        car_switch_turn();                  /* 切换转弯模式 */
    }
    if (psx_button_released(PSXB_SQUARE)) {
        car_switch_mirror();                /* 切换镜像模式 */
    }
    /* 刹车, 优先级0 */
    if (psx_button_state(PSXB_CIRCLE)) {
        car_brake();                        /* 刹车 */
    }
    /* 左右键, 优先级1 */
    else if (psx_button_state(PSXB_PAD_LEFT)) {
        car_turnleft();                     /* 左转 */
    }
    else if (psx_button_state(PSXB_PAD_RIGHT)) {
        car_turnright();                    /* 右转 */
    }
    /* 上下键, 优先级2 */
    else if (psx_button_state(PSXB_PAD_UP)) {
        car_forward();                      /* 前进 */
    }
    else if (psx_button_state(PSXB_PAD_DOWN)) {
        car_backward();                     /* 后退 */
    }
    /* 没有按键 */
    else {
        car_nothing();                      /* 自由滑行 */
    }
}
