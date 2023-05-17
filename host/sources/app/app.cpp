#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include "ak.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void task_init() {
    APP_PRINT(KBLU"\r\n"
			KBLU"  _|_|_|    _|_|    _|      _|      _|    _|                        _|            _|    _|_|_|    \r\n"
			KBLU"_|        _|    _|  _|_|    _|      _|    _|    _|_|      _|_|_|  _|_|_|_|      _|  _|        _|  \r\n"
			KBLU"_|        _|_|_|_|  _|  _|  _|      _|_|_|_|  _|    _|  _|_|        _|          _|  _|    _|_|    \r\n"
			KBLU"_|        _|    _|  _|    _|_|      _|    _|  _|    _|      _|_|    _|          _|  _|        _|  \r\n"
			KBLU"  _|_|_|  _|    _|  _|      _|      _|    _|    _|_|    _|_|_|        _|_|        _|    _|_|_|    \r\n"
			"\r\n"
			);
}
