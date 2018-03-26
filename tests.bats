#!/usr/bin/env bats

@test "simple command invocation" {
  rm resultfile 2> /dev/null || true

  ./multirun \
    "sh -c \"echo test > resultfile\""
  [ -e resultfile ]
  result=$(cat resultfile)
  [ "$result" = "test" ]
  rm resultfile
}

@test "multiple commands invocation" {
  rm resultfile1 2> /dev/null || true
  rm resultfile2 2> /dev/null || true

  ./multirun \
    "sh -c \"echo test > resultfile1; sleep 1\"" \
    "sh -c \"echo test > resultfile2; sleep 1\""
  [ -e resultfile1 ]
  [ -e resultfile2 ]
  result1=$(cat resultfile1)
  [ "$result1" = "test" ]
  result2=$(cat resultfile2)
  [ "$result2" = "test" ]

  rm resultfile1
  rm resultfile2
}

@test "returns 0" {
  run ./multirun "sh -c \"exit 0\""
  [ "$status" -eq 0 ]
}

@test "returns -1" {
  run ./multirun "sh -c \"exit -1\""
  [ "$status" -eq -1 ]
  run ./multirun "sh -c \"exit 1\""
  [ "$status" -eq -1 ]
}
