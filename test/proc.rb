assert("Proc#to_irep Proc.from_irep") do
  proc = lambda { 1 }
  irep = proc.to_irep
  proc2 = Proc.from_irep(irep)
  assert_equal(1, proc2.call)
end
