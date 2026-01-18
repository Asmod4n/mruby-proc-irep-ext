#pragma once

#include <mruby.h>

MRB_BEGIN_DECL
MRB_API mrb_value
mrb_proc_to_irep(mrb_state *mrb, struct RProc* proc);

MRB_API mrb_value
mrb_proc_from_irep(mrb_state* mrb, const void *bin, size_t len);
MRB_END_DECL