////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////

#include "game/sound_trace.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <utility>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "lib/di/container.hpp"

using json = nlohmann::json;

SoundParityTrace &SoundParityTrace::instance() {
	static SoundParityTrace trace;
	return trace;
}

SoundParityTrace::SoundParityTrace() {
	const char *path = std::getenv("CRYSTALSERVER_SOUND_TRACE");
	if (!path || !*path) {
		return;
	}

	try {
		const std::filesystem::path tracePath(path);
		if (tracePath.has_parent_path()) {
			std::filesystem::create_directories(tracePath.parent_path());
		}
		file_.open(tracePath, std::ios::out | std::ios::trunc);
	} catch (const std::exception &error) {
		g_logger().error("Unable to create sound parity trace '{}': {}", path, error.what());
		return;
	}

	if (!file_) {
		g_logger().error("Unable to create sound parity trace '{}'.", path);
		return;
	}

	startMonoUs_ = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
	enabled_.store(true, std::memory_order_release);
	writer_ = std::thread(&SoundParityTrace::writerLoop, this);
	event("session.start", json {
		{ "producer", "crystalserver" },
		{ "trace_path", path },
		{ "clock", "steady_clock+unix_epoch" },
	}.dump());
	g_logger().info("Sound parity trace enabled: {}", path);
}

SoundParityTrace::~SoundParityTrace() {
	if (!writer_.joinable()) {
		return;
	}

	event("session.stop", json { { "producer", "crystalserver" } }.dump());
	enabled_.store(false, std::memory_order_release);
	{
		std::lock_guard lock(mutex_);
		stopping_ = true;
	}
	condition_.notify_one();
	writer_.join();
	file_.flush();
	file_.close();
}

void SoundParityTrace::event(const std::string_view name, const std::string &dataJson) {
	if (!enabled_.load(std::memory_order_acquire)) {
		return;
	}

	const uint64_t monoUs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
	const uint64_t epochUs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());
	const uint64_t sequence = sequence_.fetch_add(1, std::memory_order_relaxed);
	const std::string line = fmt::format(
		R"({{"schema":"crystal-sound-trace-v1","producer":"crystalserver","seq":{},"mono_us":{},"session_us":{},"epoch_us":{},"event":{},"data":{}}})",
		sequence, monoUs, monoUs - startMonoUs_, epochUs, json(name).dump(), dataJson);

	{
		std::lock_guard lock(mutex_);
		static constexpr size_t MAX_QUEUED_EVENTS = 16384;
		if (queue_.size() >= MAX_QUEUED_EVENTS) {
			queue_.pop_front();
			++dropped_;
		}
		queue_.emplace_back(line);
	}
	condition_.notify_one();
}

void SoundParityTrace::writerLoop() {
	std::deque<std::string> pending;
	for (;;) {
		uint64_t dropped = 0;
		{
			std::unique_lock lock(mutex_);
			condition_.wait_for(lock, std::chrono::milliseconds(250), [this] {
				return stopping_ || !queue_.empty();
			});
			pending.swap(queue_);
			dropped = std::exchange(dropped_, 0);
			if (stopping_ && pending.empty() && dropped == 0) {
				break;
			}
		}

		if (dropped != 0) {
			const uint64_t epochUs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count());
			file_ << fmt::format(
				R"({{"schema":"crystal-sound-trace-v1","producer":"crystalserver","epoch_us":{},"event":"trace.dropped","data":{{"count":{}}}}})",
				epochUs, dropped) << '\n';
		}
		for (const auto &line : pending) {
			file_ << line << '\n';
		}
		pending.clear();
		file_.flush();
	}
}

void SoundParityTrace::soundEffect(
	const uint32_t playerId,
	const std::string_view playerName,
	const uint16_t x,
	const uint16_t y,
	const uint8_t z,
	const uint16_t mainId,
	const uint8_t mainSource,
	const uint16_t secondaryId,
	const uint8_t secondarySource
) {
	event("server.send_sound_effect", json {
		{ "recipient", { { "id", playerId }, { "name", std::string(playerName) } } },
		{ "world", { { "x", x }, { "y", y }, { "z", z } } },
		{ "main", { { "effect_id", mainId }, { "source", mainSource } } },
		{ "secondary", secondaryId == 0
			? json(nullptr)
			: json { { "effect_id", secondaryId }, { "source", secondarySource } } },
	}.dump());
}

void SoundParityTrace::anthem(
	const uint32_t playerId,
	const std::string_view playerName,
	const std::string_view kind,
	const uint16_t id
) {
	event("server.send_anthem", json {
		{ "recipient", { { "id", playerId }, { "name", std::string(playerName) } } },
		{ "kind", std::string(kind) },
		{ "id", id },
	}.dump());
}
