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

void
gpio_setup ()
{
  /* TIM1_CH1, TIM1_CH2, TIM1_CH3N */
  gpio_mode_setup (GPIOD, GPIO_MODE_OUTPUT, GPIO_CR1_OD, GPIO_CR2_2,
		   GPIO2 | GPIO4);
  /* BUG: PD5 OD mode switches to PP when we run PWM. So use TIM1_CH3N */
  gpio_mode_setup (GPIOE, GPIO_MODE_OUTPUT, GPIO_CR1_OD, GPIO_CR2_2, GPIO2);

  /* USART1_RX */
  gpio_mode_setup (GPIOC, GPIO_MODE_INPUT, GPIO_CR1_FI, GPIO_CR2_ID, GPIO2);
}

void
pwm_setup ()
{
  clk_periph_clock_enable (CLK_TIM1);

  timer_set_mode (TIM1_BASE, TIM_CMS_EDGE, TIM_DIR_UP);
  /* Divide clock frequency (Default system clock freq = 2MHz) by 20 = 100KHz */
  timer_set_prescaler (TIM1_BASE, 0x0014);

  /* preload auto reload value register */
  timer_enable_preload (TIM1_BASE);
  
  /* auto reload value 0x00FF */
  timer_set_period (TIM1_BASE, 0x00FF);

  /* Update event every cycle */
  timer_set_repetition_counter (TIM1_BASE, 0);

  /* set to PWM mode */
  /* RED */
  timer_set_oc_mode (TIM1_BASE, TIM_OC1, TIM_OCM_PWM_1);
  timer_enable_oc_preload (TIM1_BASE, TIM_OC1);

  TIM1_CCR1H = 0x00;
  TIM1_CCR1L = 0x00;
  /* BLUE */
  timer_set_oc_mode (TIM1_BASE, TIM_OC2, TIM_OCM_PWM_1);
  timer_enable_oc_preload (TIM1_BASE, TIM_OC2);
 
  TIM1_CCR2H = 0x00;
  TIM1_CCR2L = 0x00;
  /* GREEN */
  timer_set_oc_mode (TIM1_BASE, TIM_OC3, TIM_OCM_PWM_1);
  timer_enable_oc_preload (TIM1_BASE, TIM_OC3);

  TIM1_CCR3H = 0x00;
  TIM1_CCR3L = 0x00;

  /* Enable outputs. */
  TIM1_CCER1 |= TIM_CCER1_CC1E | TIM_CCER1_CC2E;
  TIM1_CCER2 |= TIM_CCER2_CC3NE;
  
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
  
  set_color (0xF0, 0x80, 0x30);
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
