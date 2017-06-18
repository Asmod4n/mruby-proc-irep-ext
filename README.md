[![Build Status](https://travis-ci.org/Asmod4n/mruby-proc-irep-ext.svg?branch=master)](https://travis-ci.org/Asmod4n/mruby-proc-irep-ext)
# mruby-proc-irep-ext

Adds extensions to the Proc class to dump mruby blocks as a irep structur and load them again

To be able to unpack procs you have to compile mruby with ```c #define MRB_USE_ETEXT_EDATA``` in include/mrbconf.h of your mruby directory. It isn't supported with Visual Studio.

Examples
========

```ruby
proc = Proc.new { 1+2 }
irep = proc.to_irep

proc2 = Proc.from_irep(irep)
proc2.call
```

LICENSE
=======

```
Copyright 2017 Hendrik Beskow

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this project except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
