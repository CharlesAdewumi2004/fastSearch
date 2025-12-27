add_test([=[Smoke.GTestWiresUp]=]  /home/charlie/dev/hybridSearch/build/tests/hybrid_search_tests [==[--gtest_filter=Smoke.GTestWiresUp]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Smoke.GTestWiresUp]=]  PROPERTIES WORKING_DIRECTORY /home/charlie/dev/hybridSearch/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  hybrid_search_tests_TESTS Smoke.GTestWiresUp)
