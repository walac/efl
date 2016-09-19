/* EINA - EFL data type library
 * Copyright (C) 2007-2008 Jorge Luis Zapata Muga
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

#ifdef EFL_HAVE_THREADS
# ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
# elif defined (__sun) || defined(__GNU__) || defined(__CYGWIN__)
#  include <unistd.h>
# elif defined (__FreeBSD__) || defined (__OpenBSD__) || \
   defined (__NetBSD__) || defined (__DragonFly__) || defined (__MacOSX__) || \
   (defined (__MACH__) && defined (__APPLE__))
#  include <unistd.h>
#  include <sys/param.h>
#  include <sys/sysctl.h>
# elif defined (__linux__) || defined(__GLIBC__)
#  include <sched.h>
# endif
# include <pthread.h>

# define TH_MAX 32
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "eina_log.h"
#include "eina_cpu.h"

#include <Eina.h>

#include "eina_cpu_private.h"

#if defined(HAVE_SYS_AUXV_H) && defined(HAVE_ASM_HWCAP_H) && defined(__arm__) && defined(__linux__)
# include <sys/auxv.h>
# include <asm/hwcap.h>
#endif

/*============================================================================*
*                                  Local                                     *
*============================================================================*/

static void _eina_page_size(void);

static int fastest_core_speed = 0;
static int slowest_core_speed = INT_MAX;
static Eina_Hash *cpu_hash = NULL;

/* FIXME this ifdefs should be replaced */
#if defined(__i386__) || defined(__x86_64__)
/* We save ebx and restore it to be PIC compatible */
static inline void _x86_cpuid(int op, int *a, int *b, int *c, int *d)
{
   __asm__ volatile (
#if defined(__x86_64__)
      "pushq %%rbx      \n\t" /* save %ebx */
#else
      "pushl %%ebx      \n\t" /* save %ebx */
#endif
      "cpuid            \n\t"
      "movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
#if defined(__x86_64__)
      "popq %%rbx       \n\t" /* restore the old %ebx */
#else
      "popl %%ebx       \n\t" /* restore the old %ebx */
#endif
      : "=a" (*a), "=r" (*b), "=c" (*c), "=d" (*d)
      : "a" (op)
      : "cc");
}

static
void _x86_simd(Eina_Cpu_Features *features)
{
   int a, b, c, d;

   _x86_cpuid(1, &a, &b, &c, &d);
   /*
    * edx
    * 18 = PN (Processor Number)
    * 19 = CLFlush (Cache Line Flush)
    * 23 = MMX
    * 25 = SSE
    * 26 = SSE2
    * 28 = HTT (Hyper Threading)
    * ecx
    * 0 = SSE3
    * 9 = SSSE3
    * 19 = SSE4.1
    * 20 = SSE4.2
    */
   if ((d >> 23) & 1)
      *features |= EINA_CPU_MMX;

   if ((d >> 25) & 1)
      *features |= EINA_CPU_SSE;

   if ((d >> 26) & 1)
      *features |= EINA_CPU_SSE2;

   if (c & 1)
      *features |= EINA_CPU_SSE3;

   if ((c >> 9) & 1)
      *features |= EINA_CPU_SSSE3;

   if ((c >> 19) & 1)
      *features |= EINA_CPU_SSE41;

   if ((c >> 20) & 1)
      *features |= EINA_CPU_SSE42;
}
#endif

#if defined(HAVE_SYS_AUXV_H) && defined(HAVE_ASM_HWCAP_H) && defined(__arm__) && defined(__linux__)
static void
_arm_cpu_features(Eina_Cpu_Features *features)
{
   unsigned long aux;

   aux = getauxval(AT_HWCAP);

   if (aux & HWCAP_NEON)
     *features |= EINA_CPU_NEON;
}
#endif

/*============================================================================*
*                                 Global                                     *
*============================================================================*/

/*============================================================================*
*                                   API                                      *
*============================================================================*/

/* FIXME the features checks should be called when this function is called?
 * or make it static by doing eina_cpu_init() and return a local var
 */
EAPI Eina_Cpu_Features eina_cpu_features = 0;

Eina_Bool
eina_cpu_init(void)
{
#if defined(__i386__) || defined(__x86_64__)
   _x86_simd(&eina_cpu_features);
#elif defined(HAVE_SYS_AUXV_H) && defined(HAVE_ASM_HWCAP_H) && defined(__arm__) && defined(__linux__)
   _arm_cpu_features(&eina_cpu_features);
#endif

   // Figure out the page size for this system
   _eina_page_size();

   return EINA_TRUE;
}

Eina_Bool
eina_cpu_shutdown(void)
{
   eina_hash_free(cpu_hash);
   cpu_hash = NULL;
   fastest_core_speed = 0;
   return EINA_TRUE;
}

/**
 *
 * @return
 */
EAPI Eina_Cpu_Features eina_cpu_features_get(void)
{
   return eina_cpu_features;
}

static int _cpu_count = -1;

static int
_eina_cpu_count_internal(void)
{
#ifdef EFL_HAVE_THREADS

# if   defined (_WIN32)
   SYSTEM_INFO sysinfo;

   GetSystemInfo(&sysinfo);
   return sysinfo.dwNumberOfProcessors;

# elif defined (__sun) || defined(__GNU__) || defined(__CYGWIN__)
   /*
    * _SC_NPROCESSORS_ONLN: number of processors that are online, that
                            is available when sysconf is called. The number
                            of cpu can change by admins.
    * _SC_NPROCESSORS_CONF: maximum number of processors that are available
                            to the current OS instance. That number can be
                            change after a reboot.
    * _SC_NPROCESSORS_MAX : maximum number of processors that are on the
                            motherboard.
    */
   return sysconf(_SC_NPROCESSORS_ONLN);

# elif defined (__FreeBSD__) || defined (__OpenBSD__) || \
   defined (__NetBSD__) || defined (__DragonFly__) || defined (__MacOSX__) || \
   (defined (__MACH__) && defined (__APPLE__))

   int mib[4];
   int cpus;
   size_t len = sizeof(cpus);

   mib[0] = CTL_HW;
#ifdef HW_AVAILCPU
   mib[1] = HW_AVAILCPU;
#else
   mib[1] = HW_NCPU;
#endif
   sysctl(mib, 2, &cpus, &len, NULL, 0);
   if (cpus < 1)
      cpus = 1;

   return cpus;

# elif defined (__linux__) || defined(__GLIBC__)
   cpu_set_t cpu;
   int i;
   static int cpus = 0;

   if (cpus != 0)
      return cpus;

   CPU_ZERO(&cpu);
   if (sched_getaffinity(0, sizeof(cpu), &cpu) != 0)
     {
        fprintf(stderr, "[Eina] could not get cpu affinity: %s\n",
                strerror(errno));
        return 1;
     }

   for (i = 0; i < TH_MAX; i++)
     {
        if (CPU_ISSET(i, &cpu))
           cpus++;
     }
   return cpus;

# else
#  error "eina_cpu_count() error: Platform not supported"
# endif
#else
   return 1;
#endif
}

static int _page_size = 0;

static void
_eina_page_size(void)
{
#ifdef _WIN32
   SYSTEM_INFO si;

   GetSystemInfo(&si);

   _page_size = (int)si.dwPageSize;
#elif defined _SC_PAGESIZE
   _page_size = (int)sysconf(_SC_PAGESIZE);
#elif defined _SC_PAGE_SIZE
   _page_size = (int)sysconf(_SC_PAGE_SIZE);
#elif defined HAVE_GETPAGESIZE
   _page_size = getpagesize();
#else
# warn "Falling back to a safe default page size (4K) !"
   _page_size = 4096;
#endif
   if (_page_size < 1)
     {
        EINA_LOG_ERR("system reported weird value for PAGESIZE, assuming 4096.");
        _page_size = 4096;
     }
}

EAPI int eina_cpu_page_size(void)
{
   if (_page_size == 0) _eina_page_size();
   return _page_size;
}

EAPI int eina_cpu_count(void)
{
   return _cpu_count;
}

void eina_cpu_count_internal(void)
{
   if (getenv("EINA_CPU_FAKE"))
     _cpu_count = atoi(getenv("EINA_CPU_FAKE"));
   else
     _cpu_count = _eina_cpu_count_internal();
}

static void
eina_cpu_map_init(void)
{
   fastest_core_speed = -1;

#if defined (__linux__) || defined(__GLIBC__)
   FILE *f = NULL;
   Eina_Iterator *it;
   Eina_Strbuf *fname;
   const Eina_File_Direct_Info *f_info;

   it = eina_file_stat_ls("/sys/devices/system/cpu/cpufreq");
   if (!it) return;

   cpu_hash = eina_hash_int32_new(free);

   fname = eina_strbuf_new();
   EINA_ITERATOR_FOREACH(it, f_info)
     {
        if ((f_info->type == EINA_FILE_DIR) &&
            eina_str_has_prefix(f_info->path,
                                "/sys/devices/system/cpu/cpufreq/policy"))
          {
             int num, speed;

             eina_strbuf_append_printf(fname, "%s%s", f_info->path, "/cpuinfo_max_freq");
             f = fopen(eina_strbuf_string_get(fname), "r");
             eina_strbuf_reset(fname);
             if (!f) goto err;
             speed = -1;
             num = fscanf(f, "%d", &speed);
             fclose(f);
             f = NULL;
             if ((num != 1) || (speed == -1)) goto err;

             slowest_core_speed = MIN(speed, slowest_core_speed);
             fastest_core_speed = MAX(speed, fastest_core_speed);

             eina_strbuf_append_printf(fname, "%s%s", f_info->path, "/affected_cpus");
             f = fopen(eina_strbuf_string_get(fname), "r");
             eina_strbuf_reset(fname);
             if (!f) goto err;
             do
               {
                  int core;
                  uint64_t *corelist;
                  num = fscanf(f, "%d", &core);
                  if ((num == EOF) || (core > 63)) break;

                  corelist = eina_hash_find(cpu_hash, &speed);
                  if (!corelist)
                    {
                       corelist = malloc(sizeof(*corelist));
                       if (!corelist) goto err;
                       *corelist = 1LL << core;
                       eina_hash_add(cpu_hash, &speed, corelist);
                    }
                  *corelist |= 1LL << core;
               } while (num != EOF);
             fclose(f);
             f = NULL;
          }
     }
err:
   if (f) fclose(f);
   eina_strbuf_free(fname);
   eina_iterator_free(it);
#endif
}

EAPI int
_eina_cpu_fast_core_get(void)
{
#if defined (__linux__) || defined(__GLIBC__)
   uint64_t *corelist;
   uint64_t cores;
   int bit, place = 0;

   if (fastest_core_speed == -1) return -1;

   if (fastest_core_speed == 0) eina_cpu_map_init();

   /* Check again now that it's actually set up */
   if (fastest_core_speed == -1) return -1;

   corelist = eina_hash_find(cpu_hash, &fastest_core_speed);
   cores = *corelist;
   bit = rand() % __builtin_popcount(cores);
   while (bit || !(cores & 1))
     {
        if (cores & 1) bit--;
        cores = cores >> 1;
        place++;
     }

   return place;
#else
   return 0;
#endif
}
