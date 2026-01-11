#!/bin/bash

# ===============================
# Tests for Hamming code program
# ===============================

BIN=./hamming

pass=0
fail=0

run_test() {
  name="$1"
  cmd="$2"
  expected="$3"

  output=$(eval "$cmd")

  if [ "$output" = "$expected" ]; then
    echo "[OK]   $name"
    pass=$((pass+1))
  else
    echo "[FAIL] $name"
    echo "  command : $cmd"
    echo "  expected: '$expected'"
    echo "  got     : '$output'"
    fail=$((fail+1))
  fi
}

echo "Running tests..."
echo ""

# -------------------------------
# Basic (7,4) tests
# -------------------------------

run_test "encode (7,4)" \
  "$BIN encode 4 7 1011" \
  "0110011"

run_test "decode (7,4)" \
  "$BIN decode 7 0110011" \
  "3
1011"

run_test "decode (7,4) with error" \
  "$BIN decode 7 0110001" \
  "3
1011"

run_test "is_valid (7,4) true" \
  "$BIN is_valid 7 0110011" \
  "1"

run_test "is_valid (7,4) false" \
  "$BIN is_valid 7 0110001" \
  "0"

run_test "encode (7,4) all zeros" \
  "$BIN encode 4 7 0000" \
  "0000000"

run_test "encode (7,4) all ones" \
  "$BIN encode 4 7 1111" \
  "1111111"

# -------------------------------
# Property tests (15,11)
# -------------------------------

data_15="10101010101"
enc_15=$($BIN encode 11 15 "$data_15")

run_test "is_valid (15,11) generated" \
  "$BIN is_valid 15 $enc_15" \
  "1"

run_test "decode (15,11) generated" \
  "$BIN decode 15 $enc_15" \
  "4
$data_15"

# flip one bit (position 10)
err_15=$(echo "$enc_15" | awk '{ 
  for (i=1;i<=length;i++) {
    c=substr($0,i,1)
    if (i==10) c=(c=="0"?"1":"0")
    printf "%s", c
  }
}')

run_test "decode (15,11) with 1-bit error" \
  "$BIN decode 15 $err_15" \
  "4
$data_15"

# -------------------------------
# Property tests (31,26)
# -------------------------------

data_31="10101010101010101010101010"
enc_31=$($BIN encode 26 31 "$data_31")

run_test "is_valid (31,26) generated" \
  "$BIN is_valid 31 $enc_31" \
  "1"

run_test "decode (31,26) generated" \
  "$BIN decode 31 $enc_31" \
  "5
$data_31"

# -------------------------------
# Summary
# -------------------------------

echo ""
echo "Passed: $pass"
echo "Failed: $fail"

if [ $fail -ne 0 ]; then
  exit 1
fi
