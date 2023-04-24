/*!
 * @file        Board.c
 *
 * @brief       This file provides firmware functions to manage Leds and push-buttons
 *
 * @version     V1.0.4
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "Board.h"

#if defined (APM32F103_MINI)
#include "Board_APM32F103_MINI\src\Board_APM32F103_MINI.c"

#elif defined (APM32F103VCS_MINI)
#include "Board_APM32F103VCS_MINI\src\Board_APM32F103VCS_MINI.c"

#elif defined (APM32F107_MINI)
#include "Board_APM32F107_MINI\src\Board_APM32F107_MINI.c"

#else
#error "Please select first the APM32 board to be used (in board.c)"
#endif

/** @addtogroup Board
  @{

*/

/**@} end of group Board */
