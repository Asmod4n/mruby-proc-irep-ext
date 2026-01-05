#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <mruby/string.h>
#include <mruby/value.h>
MRB_BEGIN_DECL
#include <mruby/internal.h>
MRB_END_DECL
#include <mruby/presym.h>

#if (__GNUC__ >= 3) || (__INTEL_COMPILER >= 800) || defined(__clang__)
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

// ------------------------------------------------------------
// RAII helper for mrb_irep* that ensures decref on destruction
// ------------------------------------------------------------
struct IrepHolder {
    mrb_state* mrb;
    mrb_irep* irep;

    IrepHolder(mrb_state* m, mrb_irep* i) : mrb(m), irep(i) {}
    ~IrepHolder() {
        if (irep) {
            mrb_irep_decref(mrb, irep);
        }
    }

    // non-copyable
    IrepHolder(const IrepHolder&) = delete;
    IrepHolder& operator=(const IrepHolder&) = delete;

    // movable
    IrepHolder(IrepHolder&& other) noexcept : mrb(other.mrb), irep(other.irep) {
        other.irep = nullptr;
    }
};

// ------------------------------------------------------------
// RAII helper for raw malloc'd memory from mrb_dump_irep
// ------------------------------------------------------------
struct BinHolder {
    mrb_state* mrb;
    uint8_t* ptr;

    BinHolder(mrb_state* m, uint8_t* p) : mrb(m), ptr(p) {}
    ~BinHolder() {
        if (ptr) {
            mrb_free(mrb, ptr);
        }
    }

    // non-copyable
    BinHolder(const BinHolder&) = delete;
    BinHolder& operator=(const BinHolder&) = delete;

    // movable
    BinHolder(BinHolder&& other) noexcept : mrb(other.mrb), ptr(other.ptr) {
        other.ptr = nullptr;
    }
};

static mrb_value
mrb_proc_to_irep(mrb_state* mrb, mrb_value self)
{
    struct RProc* proc = mrb_proc_ptr(self);
    if (unlikely(MRB_PROC_CFUNC_P(proc))) {
        mrb_raise(mrb, E_TYPE_ERROR, "cannot pack C procs");
    }

    uint8_t* bin = nullptr;
    size_t bin_size = 0;

#ifdef MRB_DEBUG
    int result = mrb_dump_irep(mrb, proc->body.irep, MRB_DUMP_DEBUG_INFO, &bin, &bin_size);
#else
    int result = mrb_dump_irep(mrb, proc->body.irep, 0, &bin, &bin_size);
#endif

    if (unlikely(result != MRB_DUMP_OK)) {
        mrb_raise(mrb, E_RUNTIME_ERROR, "cannot dump irep");
    }

    BinHolder bin_holder(mrb, bin);

    return mrb_str_new(mrb, reinterpret_cast<const char*>(bin), bin_size);
}

static mrb_value
mrb_proc_from_irep(mrb_state* mrb, mrb_value self)
{
    mrb_value bin;
    mrb_get_args(mrb, "S", &bin);

    mrb_irep* irep = mrb_read_irep_buf(mrb, RSTRING_PTR(bin), RSTRING_LEN(bin));
    if (!irep) {
        mrb_raise(mrb, E_SCRIPT_ERROR, "irep load error");
    }

    IrepHolder holder(mrb, irep);

    mrb_value proc = mrb_obj_value(mrb_closure_new(mrb, irep));

    return proc;
}

MRB_BEGIN_DECL
void
mrb_mruby_proc_irep_ext_gem_init(mrb_state* mrb)
{
    mrb_define_method_id(mrb, mrb->proc_class, MRB_SYM(to_irep),
                      mrb_proc_to_irep, MRB_ARGS_NONE());

    mrb_define_class_method_id(mrb, mrb->proc_class, MRB_SYM(from_irep),
                            mrb_proc_from_irep, MRB_ARGS_REQ(1));
}

void mrb_mruby_proc_irep_ext_gem_final(mrb_state*) {}
MRB_END_DECL