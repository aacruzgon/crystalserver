#include <boost/ut.hpp>

#include <cstdlib>

#include "account/account_repository_db.hpp"
#include "database/database.hpp"
#include "lib/logging/in_memory_logger.hpp"
#include "utils/tools.hpp"
#include "enums/account_type.hpp"
#include "account/account_info.hpp"

using namespace boost::ut;

auto databaseTest(const std::function<void(void)> &load) {
	return [load] {
		// A real transaction pins one pooled connection until rollback; the raw
		// executeQuery("BEGIN") this used to do does not, so with a pool the ROLLBACK could
		// land on a different connection and the test's writes would silently commit -
		// which is how stale copies of the test account ended up in the live database.
		// Returning false makes the wrapper roll back unconditionally.
		DBTransaction::executeWithinTransactionRollbackOnFailure([&load] {
			try {
				load();
			} catch (...) {
			}
			return false;
		});
	};
}

// premiumRemainingDays is not a stored column: load() derives it from `lastday` against
// the clock. A fixture with a past lastday therefore always loads 0 remaining days, so the
// fixture pins lastday 11 days into the future (plus an hour of slack for integer
// division) and expects 11 back.
static const time_t FIXTURE_LAST_DAY = getTimeNow() + 11 * 86400 + 3600;

// The fixture's name, email and session key are deliberately implausible: the test runs
// against the live database (writes rolled back), and `accounts` has a unique key on the
// name - a fixture called 'test' collided with a real local account of that name.
void createAccount(Database &db) {
	db.executeQuery(fmt::format(
		"INSERT INTO `accounts` "
		"(`id`, `name`, `email`, `password`, `type`, `premdays`, `lastday`, `premdays_purchased`, `creation`) "
		"VALUES(111, 'integration_test_111', '@integration_test_111', '', 3, 11, {}, 11, 42183281)",
		FIXTURE_LAST_DAY
	));

	db.executeQuery(fmt::format(
		"INSERT INTO `account_sessions` (`id`, `account_id`, `expires`) "
		"VALUES ('{}', 111, 1337)",
		transformToSHA1("integration_test_111")
	));
}

void assertAccountLoad(const AccountInfo &acc) {
	expect(eq(acc.id, 111));
	expect(eq(acc.accountType, AccountType::ACCOUNT_TYPE_SENIORTUTOR));
	expect(eq(acc.premiumRemainingDays, 11));
	expect(eq(acc.premiumLastDay, FIXTURE_LAST_DAY));
	expect(eq(acc.players.size(), 0));
	expect(eq(acc.oldProtocol, false));
	expect(eq(acc.premiumDaysPurchased, 11));
	expect(approx(acc.creationTime, 42183281, 60 * 60 * 1000));
}

namespace {
	std::string envOr(const char* name, const char* fallback) {
		const char* value = std::getenv(name);
		return value != nullptr && value[0] != '\0' ? value : fallback;
	}
}

int main() {
	// Defaults match config.lua.dist; a local setup that diverges (this machine's MySQL
	// lives in an OrbStack container on 3307) overrides through the environment, which
	// ctest populates from the TEST_MYSQL_* cache variables in tests/integration/CMakeLists.txt.
	struct DbConfig {
	public:
		std::string host = envOr("CRYSTAL_TEST_MYSQL_HOST", "127.0.0.1");
		std::string user = envOr("CRYSTAL_TEST_MYSQL_USER", "root");
		std::string password = envOr("CRYSTAL_TEST_MYSQL_PASS", "root");
		std::string database = envOr("CRYSTAL_TEST_MYSQL_DB", "crystalserver");
		uint32_t port = static_cast<uint32_t>(std::stoul(envOr("CRYSTAL_TEST_MYSQL_PORT", "3306")));
		std::string sock;
    };
	// The repository logs through the DI container, and the logging test below reads what
	// it logged - so install the in-memory logger the way the unit suite does, before
	// anything logs.
	di::extension::injector<> injector {};
	DI::setTestContainer(&InMemoryLogger::install(injector));

	// The singleton, not a local instance: AccountRepositoryDB reaches the database through
	// g_database(), so connecting anything else leaves the repository talking to an empty
	// pool while the test's own queries succeed.
	Database &db = g_database();
	DbConfig dbConfig{};

	db.connect(
		&dbConfig.host,
		&dbConfig.user,
		&dbConfig.password,
		&dbConfig.database,
		dbConfig.port,
		&dbConfig.sock
	);

	test("AccountRepositoryDB::loadByID") = databaseTest([&db] {
		AccountRepositoryDB accRepo{};
		createAccount(db);

		AccountInfo acc;
		accRepo.loadByID(111, acc);
		assertAccountLoad(acc);
		expect(eq(acc.sessionExpires, 0));
	});

	test("AccountRepositoryDB::loadByEmailOrName") = databaseTest([&db] {
		AccountRepositoryDB accRepo {};
		createAccount(db);

		AccountInfo acc;
		accRepo.loadByEmailOrName(false, "@integration_test_111", acc);
		assertAccountLoad(acc);
		expect(eq(acc.sessionExpires, 0));
	});

	test("AccountRepositoryDB::loadBySession") = databaseTest([&db] {
		AccountRepositoryDB accRepo {};
		createAccount(db);

		AccountInfo acc;
		accRepo.loadBySession("integration_test_111", acc);

		assertAccountLoad(acc);
		expect(eq(acc.sessionExpires, 1337));
	});

	// The old "load sets premium day purchased = remaining days" test is gone with the
	// behaviour it pinned: load() no longer adjusts premdays_purchased, and
	// premiumRemainingDays is derived from lastday rather than stored, so saving a changed
	// remaining-days value cannot survive a reload by design.

	test("AccountRepositoryDB::getPassword") = databaseTest([&db] {
		AccountRepositoryDB accRepo {};

		std::string password;

		expect(accRepo.getPassword(1, password));
		expect(eq(password, std::string { "21298df8a3277357ee55b01df9530b535cf08ec1" }));
	});

	test("AccountRepositoryDB::getPassword logs on failure") = databaseTest([&db] {
		// The repository logs through g_logger(), which the DI container above resolves to
		// the shared InMemoryLogger - a locally constructed instance would see nothing.
		auto &logger = dynamic_cast<InMemoryLogger &>(g_logger());
		logger.reset();
		AccountRepositoryDB accRepo {};

		std::string password;

		expect(!accRepo.getPassword(891237, password));
		expect(eq(logger.logs.size(), 1) >> fatal);
		expect(eq(logger.logs[0].level, std::string { "error" }));
		expect(eq(logger.logs[0].message, std::string { "Failed to get account:[891237] password!" }));
	});

	test("AccountRepositoryDB::save") = databaseTest([&db] {
		AccountRepositoryDB accRepo {};

		// lastday must sit in the future for the reload to report remaining days: load()
		// derives premiumRemainingDays from lastday against the clock, it is not a stored
		// column.
		const time_t lastDay = getTimeNow() + 10 * 86400 + 3600;

		AccountInfo acc;
		acc.id = 1;
		acc.accountType = AccountType::ACCOUNT_TYPE_SENIORTUTOR;
		acc.premiumRemainingDays = 10;
		acc.premiumLastDay = lastDay;
		acc.sessionExpires = 99999999;
		expect(accRepo.save(acc));

		AccountInfo acc2;
		accRepo.loadByID(1, acc2);
		expect(eq(acc2.id, 1));
		expect(eq(acc2.accountType, AccountType::ACCOUNT_TYPE_SENIORTUTOR));
		expect(eq(acc2.premiumRemainingDays, 10));
		expect(eq(acc2.premiumLastDay, lastDay));
		// sessionExpires is not saved
		expect(eq(acc2.sessionExpires, 0));
	});
}
