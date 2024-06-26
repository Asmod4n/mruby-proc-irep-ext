# mruby-proc-irep-ext

Adds extensions to the Proc class to dump mruby blocks as a irep struct and load them again.

Limitations
-----------
Only Procs/lambdas created with ruby code can be dumped.
Normally ruby captures the local variables around a proc, this is currently not supported in this gem but is beeing investigated.


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
