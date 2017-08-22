#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <mruby/throw.h>
#include <mruby/string.h>
#include <mruby/value.h>

#if (__GNUC__ >= 3) || (__INTEL_COMPILER >= 800) || defined(__clang__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

static mrb_value
mrb_proc_to_irep(mrb_state *mrb, mrb_value self)
{
    struct RProc *proc = mrb_proc_ptr(self);
    if (unlikely(MRB_PROC_CFUNC_P(proc))) {
        mrb_raise(mrb, E_TYPE_ERROR, "cannot pack C procs");
    }

    uint8_t *bin = NULL;
    size_t bin_size = 0;
#ifdef MRB_DEBUG
    int result = mrb_dump_irep(mrb, proc->body.irep, DUMP_ENDIAN_NAT|DUMP_DEBUG_INFO, &bin, &bin_size);
#else
    int result = mrb_dump_irep(mrb, proc->body.irep, DUMP_ENDIAN_NAT, &bin, &bin_size);
#endif
    if (unlikely(result != MRB_DUMP_OK)) {
        mrb_raise(mrb, E_RUNTIME_ERROR, "cannot dump irep");
    }

    struct mrb_jmpbuf* prev_jmp = mrb->jmp;
    struct mrb_jmpbuf c_jmp;
    mrb_value irep = mrb_nil_value();

    MRB_TRY(&c_jmp)
    {
        mrb->jmp = &c_jmp;
        irep = mrb_str_new(mrb, (const char *) bin, bin_size);
        mrb_free(mrb, bin);
        mrb->jmp = prev_jmp;
    }
    MRB_CATCH(&c_jmp)
    {
        mrb->jmp = prev_jmp;
        mrb_free(mrb, bin);
        MRB_THROW(mrb->jmp);
    }
    MRB_END_EXC(&c_jmp);

    return irep;
}

static mrb_value
mrb_proc_from_irep(mrb_state *mrb, mrb_value self)
{
#ifdef MRB_USE_ETEXT_EDATA
    mrb_value bin;
    mrb_get_args(mrb, "S", &bin);
    mrb_str_modify(mrb, RSTRING(bin));

    mrb_irep *irep = mrb_read_irep(mrb, (const uint8_t *) RSTRING_PTR(bin));

    if (!irep) {
        mrb_raise(mrb, E_SCRIPT_ERROR, "irep load error");
    }

    struct mrb_jmpbuf* prev_jmp = mrb->jmp;
    struct mrb_jmpbuf c_jmp;
    mrb_value proc = mrb_nil_value();

    MRB_TRY(&c_jmp)
    {
        mrb->jmp = &c_jmp;
        proc = mrb_obj_value(mrb_proc_new(mrb, irep));
        mrb_irep_decref(mrb, irep);
        mrb->jmp = prev_jmp;
    }
    MRB_CATCH(&c_jmp)
    {
        mrb->jmp = prev_jmp;
        mrb_irep_decref(mrb, irep);
        MRB_THROW(mrb->jmp);
    }
    MRB_END_EXC(&c_jmp);

    return proc;
#else
    mrb_raise(mrb, E_NOTIMP_ERROR, "mruby was compiled without MRB_USE_ETEXT_EDATA, cannot unpack procs");
#endif
}

void
mrb_mruby_proc_irep_ext_gem_init(mrb_state *mrb)
{
    mrb_define_method(mrb, mrb->proc_class, "to_irep", mrb_proc_to_irep, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, mrb->proc_class, "from_irep", mrb_proc_from_irep, MRB_ARGS_REQ(1));
}

void mrb_mruby_proc_irep_ext_gem_final(mrb_state *mrb) {}
