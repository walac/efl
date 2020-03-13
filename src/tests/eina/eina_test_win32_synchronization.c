/* EINA - EFL data type library
 * Copyright (C) 2013 Cedric Bail
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

#include <Eina.h>

#include "eina_suite.h"

EFL_START_TEST(eina_test_win32_lock)
{
    fail_if(1);
}
EFL_END_TEST

EFL_START_TEST(eina_test_win32_spinlock)
{
    fail_if(0);
}
EFL_END_TEST

EFL_START_TEST(eina_test_win32_srwlock)
{
}
EFL_END_TEST

EFL_START_TEST(eina_test_win32_condition_variable)
{
}
EFL_END_TEST

EFL_START_TEST(eina_test_win32_barrier)
{
}
EFL_END_TEST

EFL_START_TEST(eina_test_win32_tls)
{
}
EFL_END_TEST

void
eina_test_win32_synchronization(TCase *tc)
{
   tcase_add_test(tc, eina_test_win32_lock);
   tcase_add_test(tc, eina_test_win32_spinlock);
   tcase_add_test(tc, eina_test_win32_srwlock);
   tcase_add_test(tc, eina_test_win32_condition_variable);
   tcase_add_test(tc, eina_test_win32_barrier);
   tcase_add_test(tc, eina_test_win32_tls);
}
