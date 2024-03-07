MRUBY_CONFIG=File.expand_path(ENV["MRUBY_CONFIG"] || "build_config.rb")

file :deps do
  if Dir.exists? "mruby"
    sh "cd mruby && MRUBY_CONFIG=#{MRUBY_CONFIG} rake clean"
  else
    sh "git clone --depth=1 https://github.com/mruby/mruby.git"
  end
end

desc "test"
task :test => :deps do
  sh "cd mruby && MRUBY_CONFIG=#{MRUBY_CONFIG} rake test"
end

task :default => :test
