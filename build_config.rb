MRuby::Build.new do |conf|
  toolchain :gcc
  enable_debug
  conf.enable_test
  conf.gembox 'default'
  conf.enable_sanitizer "address,undefined,leak"
  conf.enable_debug
  conf.gem File.expand_path(File.dirname(__FILE__))
end
