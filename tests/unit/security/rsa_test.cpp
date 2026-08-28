////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////
#include "pch.hpp"

#include <boost/ut.hpp>

#include "lib/logging/in_memory_logger.hpp"
#include "security/rsa.hpp"

using namespace boost::ut;

namespace {
	// RSA::start() opens "key.pem" relative to the working directory, so this test
	// only exercises the missing-file path when run somewhere that has no key.pem.
	// From the repository root it found the real key, and loadPEM's own g_logger()
	// error plus start()'s fallback produced two entries instead of one -- a failure
	// that depended entirely on where the suite was launched from.
	//
	// Run the case in a directory that is guaranteed to be empty, and restore the
	// previous working directory even if an assertion throws.
	class ScopedEmptyWorkingDirectory {
	public:
		ScopedEmptyWorkingDirectory() :
			previous(std::filesystem::current_path()),
			directory(std::filesystem::temp_directory_path() / ("crystalserver_rsa_test_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
			std::filesystem::remove_all(directory);
			std::filesystem::create_directories(directory);
			std::filesystem::current_path(directory);
		}

		ScopedEmptyWorkingDirectory(const ScopedEmptyWorkingDirectory &) = delete;
		ScopedEmptyWorkingDirectory &operator=(const ScopedEmptyWorkingDirectory &) = delete;

		~ScopedEmptyWorkingDirectory() {
			std::error_code error;
			std::filesystem::current_path(previous, error);
			std::filesystem::remove_all(directory, error);
		}

	private:
		std::filesystem::path previous;
		std::filesystem::path directory;
	};
}

suite<"security"> rsaTest = [] {
	test("RSA::start logs error for missing .pem file") = [] {
		const ScopedEmptyWorkingDirectory emptyCwd;

		di::extension::injector<> injector {};
		DI::setTestContainer(&InMemoryLogger::install(injector));

		DI::create<RSA &>().start();

		auto &logger = dynamic_cast<InMemoryLogger &>(injector.create<Logger &>());

		expect(eq(1, logger.logs.size()) >> fatal);
		expect(
			eq(std::string { "error" }, logger.logs[0].level) and eq(std::string { "File key.pem not found or have problem on loading... Setting standard rsa key\n" }, logger.logs[0].message)
		);
	};
};
