#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "app_if.h" 
#include "app_flash.h"
#include "app_data.h"

#define TAG "appData"

/* Extern variables ----------------------------------------------------------*/
const char *hugeMsg ="\r\n"                                                                           
                    "    _/    _/  _/_/_/_/  _/        _/          _/_/        _/          _/    _/_/    _/_/_/    _/        _/_/_/    \r\n"
                    "   _/    _/  _/        _/        _/        _/    _/      _/          _/  _/    _/  _/    _/  _/        _/    _/   \r\n"
                    "  _/_/_/_/  _/_/_/    _/        _/        _/    _/      _/    _/    _/  _/    _/  _/_/_/    _/        _/    _/    \r\n"
                    " _/    _/  _/        _/        _/        _/    _/        _/  _/  _/    _/    _/  _/    _/  _/        _/    _/     \r\n"
                    "_/    _/  _/_/_/_/  _/_/_/_/  _/_/_/_/    _/_/            _/  _/        _/_/    _/    _/  _/_/_/_/  _/_/_/        \r\n"
                    ;
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
