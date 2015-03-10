/*
 * Copyright (C) 2015 Nikolay Merinov <nikolay.merinov@member.fsf.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libstm8/stm8l.h>

#define RED   GPIO2		/* TIM1_CH1 */
#define BLUE  GPIO4		/* TIM1_CH2 */
#define GREEN GPIO5		/* TIM1_CH3 */

void
gpio_setup ()
{
  /* BUG: PD5 OD mode switches to PP when we run PWM. Other pins work fine */
  /* TIM1_CH1, TIM1_CH2, TIM1_CH3 */
  gpio_mode_setup (GPIOD, GPIO_MODE_OUTPUT, GPIO_CR1_OD, GPIO_CR2_2,
		   GPIO2 | GPIO4 | GPIO5);

  /* USART1_RX + BLUE LED*/
  gpio_mode_setup (GPIOC, GPIO_MODE_INPUT, GPIO_CR1_FI, GPIO_CR2_ID, GPIO2);
}

void
pwm_setup ()
{
  clk_periph_clock_enable (CLK_TIM1);

  /* Divide clock frequency (Default 2MHz) by 200 = 100KHz */
  TIM1_PSCRH = 0x00;
  TIM1_PSCRL = 0x14;

  /* ARR preload register */
  TIM1_CR1 |= TIM_CR1_ARPE;
  
  /* auto reload value 0x00FF */
  TIM1_ARRH = 0x00;
  TIM1_ARRL = 0xFF;

  /* Update event every cycle */
  TIM1_RCR = 0;

  /* set to PWM mode */
  /* RED */
  TIM1_CCMR1 |= TIM_CCMR1_OC1M_PWM_1 << TIM_CCMR1_OC1M_SHIFT;
  TIM1_CCMR1 |= TIM_CCMR1_OC1PE;

  TIM1_CCR1H = 0x00;
  TIM1_CCR1L = 0x00;
  /* BLUE */
  TIM1_CCMR2 |= TIM_CCMR2_OC2M_PWM_1 << TIM_CCMR2_OC2M_SHIFT;
  TIM1_CCMR2 |= TIM_CCMR2_OC2PE;
 
  TIM1_CCR2H = 0x00;
  TIM1_CCR2L = 0x00;
  /* GREEN */
  TIM1_CCMR3 |= TIM_CCMR3_OC3M_PWM_1 << TIM_CCMR3_OC3M_SHIFT;
  TIM1_CCMR3 |= TIM_CCMR3_OC3PE;

  TIM1_CCR3H = 0x00;
  TIM1_CCR3L = 0x00;

  /* Enable outputs */
  TIM1_CCER1 |= TIM_CCER1_CC1E | TIM_CCER1_CC2E;
  TIM1_CCER2 |= TIM_CCER2_CC3E;
  
  /* Enable output */
  TIM1_BKR |= TIM_BKR_MOE;
  
  /* Reinitialize registers */
  TIM1_EGR |= TIM_EGR_UG;
  
  /* Enable timer */
  TIM1_CR1 |= TIM_CR1_CEN;
}

void
set_color (uint8_t r, uint8_t g, uint8_t b)
{
  TIM1_CCR1H = 0;
  TIM1_CCR1L = r;
  TIM1_CCR2H = 0;
  TIM1_CCR2L = b;
  TIM1_CCR3H = 0;
  TIM1_CCR3L = g;
}

void
usart_setup ()
{
  clk_periph_clock_enable (CLK_USART1);
  usart_set_baudrate (USART1, 9600);
  usart_set_databits (USART1, 8);
  usart_set_stopbits (USART1, USART_STOPBITS_1);
  usart_set_parity (USART1, USART_PARITY_NONE);

  usart_enable_mode (USART1, USART_MODE_RX);
}

int
main ()
{
  uint8_t d = 0, r = 0, g = 0, b = 0, ch;
  gpio_setup ();
  pwm_setup ();
  usart_setup ();
  
  set_color (0xF0, 0x80, 0x80);
  while (1)
    {
      for (d = 0, r = 0; d < 2; d++)
	{
	  ch = usart_recv_blocking (USART1);
	  if (ch >= 'A') ch = ch - 'A' + 10;
	  else ch = ch - '0';
	  r = (r << 4) + ch;
	}
      do
	{ 
	  ch = usart_recv_blocking (USART1);
	}
      while (ch != '\n' && ch != '\r');
      set_color (r, g, b);

      for (d = 0, g = 0; d < 2; d++)
	{
	  ch = usart_recv_blocking (USART1);
	  if (ch >= 'A') ch = ch - 'A' + 10;
	  else ch = ch - '0';
	  g = (g << 4) + ch;
	}
      do
	{ 
	  ch = usart_recv_blocking (USART1);
	}
      while (ch != '\n' && ch != '\r');      
      set_color (r, g, b);


      for (d = 0, b = 0; d < 2; d++)
	{
	  ch = usart_recv_blocking (USART1);
	  if (ch >= 'A') ch = ch - 'A' + 10;
	  else ch = ch - '0';
	  b = (b << 4) + ch;
	}
      do
	{ 
	  ch = usart_recv_blocking (USART1);
	}
      while (ch != '\n' && ch != '\r');      
      set_color (r, g, b);
    }
}
