
@test "simple command invocation" {
  ./multirun "sh -c \"echo test > resultfile\""
  [ -e resultfile ]
  result=$(cat resultfile)
  [ "$result" -eq "test" ]
  rm resultfile
}
