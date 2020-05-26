/* EINA - EFL data type library
 * Copyright (C) 2020 Carlos Ré Signor
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
#ifndef EINA_INLINE_THREAD_WIN32_H_
#define EINA_INLINE_THREAD_WIN32_H_

#include "eina_thread_win32.h"

#define EINA_THREAD_CLEANUP_PUSH(cleanup, data)
#define EINA_THREAD_CLEANUP_POP(exec_cleanup)

static inline void *
_eina_thread_join(Eina_Thread t)
{
   if (WaitForSingleObject(t.handle, INFINITE) == WAIT_OBJECT_0)
     {
        DWORD* ret;
        if (GetExitCodeThread(t.handle, &ret))
          {
             if (ret != 0) return NULL;
             CloseHandle(t.handle);
             return t.out;
          }
     }
   return NULL;
}

DWORD WINAPI
_eina_thread_func(void *params)
{
    void *(*func)(void *);

    Eina_Thread *thread = (Eina_Thread *) params;
    void *data = thread->in;
    func = thread->func;

    thread->out = func(data);
    return 0;
}

static inline void
_eina_thread_set_priority(Eina_Thread_Priority prio, Eina_Thread *t)
{

   int nPriority = THREAD_PRIORITY_NORMAL;

   switch (prio)
     {
      case EINA_THREAD_URGENT:
        nPriority = THREAD_PRIORITY_HIGHEST;
        break;
      case EINA_THREAD_BACKGROUND:
        nPriority = THREAD_PRIORITY_BELOW_NORMAL;
        break;
      case EINA_THREAD_IDLE:
        nPriority = THREAD_PRIORITY_IDLE;
        break;
      case EINA_THREAD_NORMAL:
      default:
        nPriority = THREAD_PRIORITY_NORMAL;
        break;
     }

   SetThreadPriority((HANDLE)t->handle, nPriority);
}

static inline Eina_Bool
_eina_thread_create(Eina_Thread *t, int affinity, void *(*func)(void *data), void *data)
{
   Eina_Bool ret;
   DWORD threadID;

   SIZE_T dwStackSize = 2*1024*1024;

   Eina_Thread_Call *c = (Eina_Thread_Call *)(data);


   t->func = func;
   t->data = data;

   t->handle = CreateThread(NULL, dwStackSize, &_eina_thread_func
                   , t, CREATE_SUSPENDED, &threadID);
   ret = (t->handle != NULL) ? EINA_TRUE : EINA_FALSE;

   if (ret)
     {
        ResumeThread(t->handle);
        _eina_thread_set_priority(c->prio, t);
     }

   if (ret)
     {
        ResumeThread(*t);
        _eina_thread_set_priority(c->prio, t);
     }
   else
     {
        free(thread_func);
     }

   if (affinity >= 0 && ret)
     {
   #ifdef EINA_HAVE_WIN32_THREAD_AFFINITY
        SetThreadAffinityMask(t.handle, (DWORD_PTR *)&affinity);
   #endif
     }

   return ret;
}

static inline Eina_Bool
_eina_thread_equal(Eina_Thread t1, Eina_Thread t2)
{
   DWORD t1_thread_id = GetThreadId(t1.handle);
   DWORD t2_thread_id = GetThreadId(t2.handle);

   return (t1_thread_id == t2_thread_id) ? EINA_TRUE : EINA_FALSE;
}

static inline Eina_Thread
_eina_thread_self(void)
{
   Eina_Thread ret;
   ret.handle = GetCurrentThread();
   return ret;
}

static inline Eina_Bool
_eina_thread_name_set(Eina_Thread thread, char *buf)
{
   HRESULT res = SetThreadDescription((HANDLE)thread.handle, (PCWSTR)buf);
   return HRESULT_CODE(res);
}

static inline Eina_Bool
_eina_thread_cancel(Eina_Thread thread)
{
   return EINVAL;
}

static inline void
_eina_thread_cancel_checkpoint(void)
{
   return;
}

static inline Eina_Bool
_eina_thread_cancellable_set(Eina_Bool cancellable, Eina_Bool *was_cancellable)
{
   return EINA_FALSE;
}
#endif