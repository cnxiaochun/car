#include "pwm.h"
#include "stm32f10x.h"

#include "stmio.h"

/**
 * @brief PWM通道定义.
 *
 * TIM  | Channel | Pin  | Description
 * ---- | ------- | ---- | ------------
 * TIM4 | CH2     | PB07 | PWM L
 * TIM4 | CH1     | PB06 | PWM r
 *
 */

/* Left motor PWM信号 */
#define PWM_L_PIN           STMIO_PB07

/* Right motor PWM信号 */
#define PWM_R_PIN           STMIO_PB06

#define PWM_TIM             TIM4

#define TIM_PERIOD          1000

#define TIM_PRESCALER       ((uint16_t) ((72000000 / 7200000) -1))

void pwm_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
    /* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* GPIO clock enable */
    RCC_APB2Periph = RCC_APB2Periph_AFIO;
    RCC_APB2Periph |= STMIO_RCC_MASKx(PWM_L_PIN);
    RCC_APB2Periph |= STMIO_RCC_MASKx(PWM_R_PIN);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE); 

    // 引脚为复用推挽输出
    GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    STMIO_INIT(PWM_L_PIN, &GPIO_InitStructure);
    STMIO_INIT(PWM_R_PIN, &GPIO_InitStructure);
 
    // 初始化TIM
	TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD -1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_PRESCALER; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(PWM_TIM, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	// 初始化TIMx Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(PWM_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);
	
	// 初始化TIMx Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(PWM_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);

	TIM_Cmd(PWM_TIM, ENABLE);  //使能TIMx
}

void pwm_l(uint16_t value) {
    TIM_SetCompare2(PWM_TIM, value > TIM_PERIOD ? TIM_PERIOD : value);
}

void pwm_r(uint16_t value) {
    TIM_SetCompare1(PWM_TIM, value > TIM_PERIOD ? TIM_PERIOD : value);
}
