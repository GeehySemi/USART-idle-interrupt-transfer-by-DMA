/*!
 * @file        Board.h
 *
 * @brief       Header file for Board
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

/* Define to prevent recursive inclusion */
#ifndef BOARD_H
#define BOARD_H

#if defined (APM32F103_MINI)
#include "Board_APM32F103_MINI\inc\Board_APM32F103_MINI.h"

#elif defined (APM32F103VCS_MINI)
#include "Board_APM32F103VCS_MINI\inc\Board_APM32F103VCS_MINI.h"

#elif defined (APM32F107_MINI)
#include "Board_APM32F107_MINI\inc\Board_APM32F107_MINI.h"

#else
#error "Please select first the APM32  board to be used (in board.h)"
#endif

/** @addtogroup Board
  @{
*/

/**@} end of group Board */

#endif
