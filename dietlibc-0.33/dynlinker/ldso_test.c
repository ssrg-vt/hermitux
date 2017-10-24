#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../libdl/_dl_int.h"

struct elf_aux {
  unsigned long type;
  unsigned long val;
};

unsigned long ph_size;
unsigned long ph_num;

unsigned long pg_size;

void (*dyn_start)();

void ldso_start(void);
extern void (*fini_entry)(void);

static struct _dl_handle* dlh;
static void tt_fini(void) {
  struct _dl_handle*tmp;
#ifdef DEBUG
  pf("dyn fini\n");
#endif
  for(tmp=dlh;tmp;tmp=tmp->next) {
    if (tmp->fini) tmp->fini();
  }
}

int main(int argc, char**argv, char**envp)
{
  int i;
  unsigned int *ui=(unsigned int*)envp;
  struct elf_aux *ea;

  Elf32_Phdr *ph32=0;

  /* --- */
  unsigned int o=0, s=0;
  /* --- */

  dlh = _dl_get_handle();

  fini_entry = tt_fini;

  while (*ui) ++ui;	/* while (still an env-pointer) next */
  /* now *ui points to the tailing NULL-pointer of the envirioment */

  /* print the elf_aux table */
  for (ea=(struct elf_aux*)(ui+1); ea->type; ea++)
  {
    switch (ea->type) {
    case AT_EXECFD:
    case AT_NOTELF:
      write(2,"Unsupported execution type\n",27);
      _exit(42);
      break;

    case AT_PHDR:
      ph32=(Elf32_Phdr*)ea->val;
#ifdef DEBUG
      pf("program header @ "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_PHENT:
      ph_size=ea->val;
#ifdef DEBUG
      pf("program header size "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_PHNUM:
      ph_num=ea->val;
#ifdef DEBUG
      pf("program header # "); ph(ea->val); pf("\n");
#endif
      break;

#if 0
    case AT_BASE:
#ifdef DEBUG
      pf("interpreter base: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_FLAGS:
#ifdef DEBUG
      pf("flags "); ph(ea->val); pf("\n");
#endif
      break;

    case AT_UID:
#ifdef DEBUG
      pf(" UID: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_EUID:
#ifdef DEBUG
      pf("EUID: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_GID:
#ifdef DEBUG
      pf(" GID: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_EGID:
#ifdef DEBUG
      pf("EGID: "); ph(ea->val); pf("\n");
#endif
      break;
#endif

    case AT_PAGESZ:
      pg_size=ea->val;
#ifdef DEBUG
      pf("page size "); ph(ea->val); pf("\n");
#endif
      break;

    case AT_ENTRY:
      dyn_start=(void(*)())ea->val;
#ifdef DEBUG
      pf("start program  @ "); ph(ea->val); pf("\n");
#endif
      break;

#if 0
    case AT_PLATFORM:
#ifdef DEBUG
      pf("CPU: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_HWCAP:
#ifdef DEBUG
      pf("CPU capabilities: "); ph(ea->val); pf("\n");
#endif
      break;
    case AT_CLKTCK:
#ifdef DEBUG
      pf("CLK per sec "); ph( ea->val); pf("\n");
#endif
      break;
#endif

    default:
      break;
    }
  }

  for (i=0; (i<ph_num); i++)
  {
    if (ph32[i].p_type==PT_DYNAMIC)
    {
      o = ph32[i].p_vaddr;
      s = ph32[i].p_memsz;
      break;
    }
  }

  if (dyn_start==ldso_start) {
    write(2,"I'm not a normal program...\n",28);
    _exit(42);
  }

  /* dynamic scan from _dl_load must be called here */

  dlh = _dl_dyn_scan(dlh,(void*)o,0);

  _dl_error_location="diet-linker.so error";
  if (!dlh) {
    const char*err=dlerror();
    write(2,err,strlen(err)); write(2,"\n",1);
    _exit(23);
  }

  dlh->name=0;
  dlh->fini=0;

  _dl_open_dep();

#if 1
  {
    struct _dl_handle* dlso;
    if ((dlso = _dl_find_lib("libdl.so"))) {
      struct _dl_handle* *tmp;
      void(*rmp)(const char*rp);
      write(2,"libdl.so",8);
      write(2," used.\n",7);
      if ((tmp=_dlsym(dlso,"_dl_root_handle" ))) *tmp=_dl_root_handle;
      if ((tmp=_dlsym(dlso,"_dl_top_handle"  ))) *tmp=_dl_top_handle;
      if ((tmp=_dlsym(dlso,"_dl_free_list"   ))) *tmp=_dl_free_list;
      if ((rmp=_dlsym(dlso,"_dl_set_rpath"))) {
	rmp(_dl_get_rpath());
      }
    }
  }
  {
    struct _dl_handle* dietc;
    if ((dietc=_dl_find_lib("libc.so"))) {
      char***tmp;
      if ((tmp=_dlsym(dietc,"environ"))) {
	*tmp=envp;
      }
      else _exit(665);
    }
    else _exit(666);
  }
#endif

  /* all depending libs have been loaded, now start the program or die */
  if (dyn_start) return (int)dyn_start; /* found an AT_ENTRY in table -> jump to it */
  _exit(17);
}
