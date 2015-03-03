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
  gpio_mode_setup (GPIOC, GPIO_MODE_OUTPUT, GPIO_CR1_OD, GPIO_CR2_2,
		   GPIO6 | GPIO5 | GPIO1);
}

int
main ()
{
  uint16_t d = 0;
  gpio_setup ();

  while (1)
    {
      GPIOC_ODR = GPIO6;
      do {} while (--d);
      do {} while (--d);
      GPIOC_ODR = GPIO5;
      do {} while (--d);
      do {} while (--d);
      GPIOC_ODR = GPIO1;
      do {} while (--d);
      do {} while (--d);
    }
}
