
@test "simple command invocation" {
  rm resultfile 2> /dev/null || true

  ./multirun "sh -c \"echo test > resultfile\""
  [ -e resultfile ]
  result=$(cat resultfile)
  [ "$result" = "test" ]
  rm resultfile
}
