#include "nvic.h"
// Модули, в которых есть перрывания
#include "timer.h"
#include "clk.h"
#include "spi.h"
#include "key.h"

// Имя секции стека
#define NVIC_SECTION_STACK      "CSTACK"

// Обявление сегмента для sfe
SECTION_DECL(NVIC_SECTION_STACK) 

// Точка входа в программу
extern void __iar_program_start(void);

// Заглушка для обработчиков прерываний
static void nvic_interrupt_dummy(void)
{
    while(1) 
    { }
}

    /*** Обработчики исключений ядра ***/
static void nmi__handler_isr(void)
{
    while(1) 
    { }
}

static void hard_fault_handler_isr(void)
{
    while(1) 
    { }
}

static void mpu_fault_handler_isr(void)
{
    while(1) 
    { }
}

static void bus_fault_handler_isr(void)
{
    while(1) 
    { }
}

static void usage_fault_handler_isr(void)
{
    while(1) 
    { }
}

static void svcall_handler_isr(void)
{
    while(1) 
    { }
}

static void debug_handler_isr(void)
{
    while(1) 
    { }
}

static void pendsv_handler_isr(void)
{
    while(1) 
    { }
}

static void systik_handler_isr(void)
{
    while(1) 
    { }
}

// Структура адресов обработчиков прерываний
typedef struct 
{
    // Адрес базы стека
    void *stack_base;
    // Адреса обработчиков прерываний
    void (* irq_handler[98])(void); 
} nvic_vtbl_t;

// __root - что бы эта чать не отбрасывалась 
// @ ".intvec" - директива компоновщика: разместить в сегменте .intvec 
__root const nvic_vtbl_t __vector_table @ ".intvec" =
    {
        __sfe(NVIC_SECTION_STACK),                  // Stack base
        {
            // --- Системные прерывания --- //
            
            __iar_program_start,                    // Reset Handler
            nmi__handler_isr,                       // NMI Handler
            hard_fault_handler_isr,                 // Hard Fault Handler
            mpu_fault_handler_isr,                  // MPU Fault Handler
            bus_fault_handler_isr,                  // Bus Fault Handler
            usage_fault_handler_isr,                // Usage Fault Handler
            NULL,                                   // Reserved
            NULL,                                   // Reserved
            NULL,                                   // Reserved
            NULL,                                   // Reserved
            svcall_handler_isr,                     // SVCall Handler
            debug_handler_isr,                      // Debug Monitor Handler
            NULL,                                   // Reserved
            pendsv_handler_isr,                     // PendSV Handler
            systik_handler_isr,                     // SysTick Handler

            // --- Прерывания периферийных модулей --- //
            
            nvic_interrupt_dummy,              //  Window WatchDog
            nvic_interrupt_dummy,              //  PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection
            clk_lse_fail_isr,                  //  Tamper and TimeStamps through the EXTI line
            nvic_interrupt_dummy,              //  RTC Wakeup through the EXTI line
            nvic_interrupt_dummy,              //  FLASH
            clk_lse_ready_isr,                 //  RCC
            key_0_isr,                      //  EXTI Line0
            key_1_isr,                      //  EXTI Line1
            key_2_isr,                      //  EXTI Line2
            key_3_isr,                      //  EXTI Line3
            nvic_interrupt_dummy,              //  EXTI Line4
            nvic_interrupt_dummy,              //  DMA1 Channel 1
            nvic_interrupt_dummy,              //  DMA1 Channel 2
            nvic_interrupt_dummy,              //  DMA1 Channel 3
            nvic_interrupt_dummy,              //  DMA1 Channel 4
            nvic_interrupt_dummy,              //  DMA1 Channel 5
            nvic_interrupt_dummy,              //  DMA1 Channel 6
            nvic_interrupt_dummy,              //  DMA1 Channel 7
            nvic_interrupt_dummy,              //  ADC1
            nvic_interrupt_dummy,              //  CAN1 TX
            nvic_interrupt_dummy,              //  CAN1 RX0
            nvic_interrupt_dummy,              //  CAN1 RX1
            nvic_interrupt_dummy,              //  CAN1 SCE
            nvic_interrupt_dummy,              //  External Line[9:5]s
            nvic_interrupt_dummy,              //  TIM1 Break and TIM15
            nvic_interrupt_dummy,              //  TIM1 Update and TIM16
            nvic_interrupt_dummy,              //  TIM1 Trigger and Commutation
            nvic_interrupt_dummy,              //  TIM1 Capture Compare
            nvic_interrupt_dummy,              //  TIM2
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  I2C1 Event
            nvic_interrupt_dummy,              //  I2C1 Error
            nvic_interrupt_dummy,              //  I2C2 Event
            nvic_interrupt_dummy,              //  I2C2 Error
            nvic_interrupt_dummy,              //  SPI1
            nvic_interrupt_dummy,              //  SPI2
            nvic_interrupt_dummy,              //  USART1
            nvic_interrupt_dummy,              //  USART2
            nvic_interrupt_dummy,              //  USART3
            nvic_interrupt_dummy,              //  External Line[15:10]
            nvic_interrupt_dummy,              //  RTC Alarm (A and B) through EXTI Line
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  SDMMC1
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  SPI3
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  TIM6 and DAC1&2 underrun errors
            nvic_interrupt_dummy,              //  TIM7
            nvic_interrupt_dummy,              //  DMA2 Channel 1
            nvic_interrupt_dummy,              //  DMA2 Channel 2
            nvic_interrupt_dummy,              //  DMA2 Channel 3
            nvic_interrupt_dummy,              //  DMA2 Channel 4
            nvic_interrupt_dummy,              //  DMA2 Channel 5
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  COMP Interrupt
            timer_lptim1_isr,                  //  LP TIM1 interrupt
            nvic_interrupt_dummy,              //  LP TIM2 interrupt
            nvic_interrupt_dummy,              //  USB FS
            nvic_interrupt_dummy,              //  DMA2 Channel 6
            nvic_interrupt_dummy,              //  DMA2 Channel 7
            nvic_interrupt_dummy,              //  LP UART 1 interrupt
            nvic_interrupt_dummy,              //  Quad SPI global interrupt
            nvic_interrupt_dummy,              //  I2C3 event
            nvic_interrupt_dummy,              //  I2C3 error
            nvic_interrupt_dummy,              //  Serial Audio Interface 1 global interrupt
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  Serial Wire Interface global interrupt
            nvic_interrupt_dummy,              //  Touch Sense Controller global interrupt
            nvic_interrupt_dummy,              //  LCD global interrupt
            NULL,                              //  Reserved
            nvic_interrupt_dummy,              //  RNG global interrupt
            nvic_interrupt_dummy,              //  FPU interrupt
            nvic_interrupt_dummy,              //  CRS interrupt
        }
    };
