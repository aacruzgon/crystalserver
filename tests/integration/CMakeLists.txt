setup_test(crystalserver_it integration)

# Where the integration test's MySQL lives. Defaults match config.lua.dist; override at
# configure time when the local server diverges (this machine's OrbStack MySQL is on 3307:
#   cmake -B build/macos-tests -DTEST_MYSQL_PORT=3307
# ). The values reach the test binary as CRYSTAL_TEST_MYSQL_* environment variables.
set(TEST_MYSQL_HOST "127.0.0.1" CACHE STRING "MySQL host for the integration test")
set(TEST_MYSQL_PORT "3306" CACHE STRING "MySQL port for the integration test")
set(TEST_MYSQL_USER "root" CACHE STRING "MySQL user for the integration test")
set(TEST_MYSQL_PASS "root" CACHE STRING "MySQL password for the integration test")
set(TEST_MYSQL_DB "crystalserver" CACHE STRING "MySQL database for the integration test")

set_tests_properties(integration PROPERTIES ENVIRONMENT
    "CRYSTAL_TEST_MYSQL_HOST=${TEST_MYSQL_HOST};CRYSTAL_TEST_MYSQL_PORT=${TEST_MYSQL_PORT};CRYSTAL_TEST_MYSQL_USER=${TEST_MYSQL_USER};CRYSTAL_TEST_MYSQL_PASS=${TEST_MYSQL_PASS};CRYSTAL_TEST_MYSQL_DB=${TEST_MYSQL_DB}"
)
