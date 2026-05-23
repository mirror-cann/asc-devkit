for s in 0 1 2 3; do
  echo "=== matrix_transpose SCENARIO_NUM=${s} ==="
  mkdir -p build_case_${s}
  cd build_case_${s}
  cmake -DSCENARIO_NUM=${s} ..
  make -j
  ./matrix_transpose
  cd ..
done
