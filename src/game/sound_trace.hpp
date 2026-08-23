////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

// Authoritative, opt-in JSONL trace of the sound packets serialized for each
// player. CRYSTALSERVER_SOUND_TRACE=/absolute/path.jsonl enables it. Keeping
// this separate from the regular logger makes timing analysis machine-readable
// and prevents trace I/O from blocking the game dispatcher.
class SoundParityTrace final {
public:
	static SoundParityTrace &instance();

	SoundParityTrace(const SoundParityTrace &) = delete;
	SoundParityTrace &operator=(const SoundParityTrace &) = delete;

	bool enabled() const {
		return enabled_.load(std::memory_order_relaxed);
	}

	void soundEffect(
		uint32_t playerId,
		std::string_view playerName,
		uint16_t x,
		uint16_t y,
		uint8_t z,
		uint16_t mainId,
		uint8_t mainSource,
		uint16_t secondaryId = 0,
		uint8_t secondarySource = 0
	);
	void anthem(uint32_t playerId, std::string_view playerName, std::string_view kind, uint16_t id);

private:
	SoundParityTrace();
	~SoundParityTrace();

	void event(std::string_view name, const std::string &dataJson);
	void writerLoop();

	std::atomic_bool enabled_ { false };
	std::atomic<uint64_t> sequence_ { 0 };
	uint64_t startMonoUs_ { 0 };
	std::ofstream file_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::deque<std::string> queue_;
	std::thread writer_;
	bool stopping_ { false };
	uint64_t dropped_ { 0 };
};
