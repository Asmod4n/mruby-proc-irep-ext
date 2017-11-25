MRuby::Gem::Specification.new('mruby-proc-irep-ext') do |spec|
  spec.license = 'Apache-2'
  spec.author  = 'Hendrik Beskow'
  spec.summary = 'irep extensions for Proc class'
  spec.cc.defines << 'MRB_USE_ETEXT_EDATA'
end
