
@test "simple command invocation" {
  ./multirun "sh -c \"echo test > resultfile\""
  [ -e resultfile ]
  result=$(cat resultfile)
  echo $result
  [ "$result" -eq "test" ]
  rm resultfile
}
