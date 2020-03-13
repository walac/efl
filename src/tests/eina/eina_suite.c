/* EINA - EFL data type library
 * Copyright (C) 2008 Cedric Bail
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <Eina.h>

#include "eina_suite.h"
#include "eina_suite.x"
#include "../efl_check.h"

static const Efl_Test_Case etc[] = {
   { "WIN32 Synchronization", eina_test_win32_synchronization },
   { NULL, NULL }
};

static Eina_Array *_modules;

SUITE_INIT(eina)
{
   ck_assert_int_eq(eina_init(), 1);
}

SUITE_SHUTDOWN(eina)
{
   ck_assert_int_eq(eina_shutdown(), 0);
}

EFL_START_TEST(mempool_module_load)
{
    _modules = eina_module_list_get(NULL,
                                  "/src/modules/eina",
                                 EINA_TRUE,
                                 NULL,
                                 NULL);
   eina_module_list_load(_modules);
   eina_module_list_unload(_modules);
}
EFL_END_TEST

static void
mempool_init(TCase *tc)
{
   tcase_add_test(tc, mempool_module_load);
}

static const Efl_Test_Case mp_etc[] = {
   { "mempool_init", mempool_init },
   { NULL, NULL },
};

int
main(int argc, char **argv)
{
   int failed_count;

   if (!_efl_test_option_disp(argc, argv, etc))
     return 0;

#ifdef NEED_RUN_IN_TREE
   putenv("EFL_RUN_IN_TREE=1");
#endif
   /* force modules to be loaded in case they are not installed
    * this function does not require eina_init to be called
    */


   failed_count = _efl_suite_build_and_run(argc - 1, (const char **)argv + 1,
                                           "eina_init_module", mp_etc, SUITE_INIT_FN(eina), SUITE_SHUTDOWN_FN(eina));

   eina_init();
   eina_module_list_load(_modules);
   failed_count += _efl_suite_build_and_run(argc - 1, (const char **)argv + 1,
                                           "Eina", etc, NULL, NULL);

   if (_modules)
     {
        while (eina_array_count(_modules))
          eina_module_free(eina_array_pop(_modules));
        eina_array_free(_modules);
     }
   eina_shutdown();

   return (failed_count == 0) ? 0 : 255;
}
