#include <array>
#include <cryptopp/blake2.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "Logging.hpp"
#include "Logic/UUID.hpp"

#include "common.hpp"

namespace Logic::UUID {

namespace {

std::string extract_digest(CryptoPP::BLAKE2b& hasher) {
	std::array<CryptoPP::byte, HASH_NUM_BYTES> digest;
	hasher.Final(digest.data());

	std::ostringstream ret_stream;
	ret_stream << std::hex << std::setfill('0');
	for (auto const byte : digest) {
		ret_stream << std::setw(2) << +byte;
	}
	return ret_stream.str();
}

}  // namespace

std::string from_data(BinaryView const data) {
	CryptoPP::BLAKE2b hasher(false, HASH_NUM_BYTES);
	hasher.Update(data.data(), data.size());
	return extract_digest(hasher);
}

}  // namespace Logic::UUID

#if defined(linux) || defined(__linux__)

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <system_error>

namespace {

[[noreturn]] inline void throw_errno() {
	throw std::system_error{ errno, std::system_category() };
}

struct FileHandle {
protected:
	int fd;
public:
	FileHandle(FileHandle const&) = delete;
	FileHandle& operator=(FileHandle const&) = delete;

	FileHandle(std::filesystem::path const& path, int flags);
	// NOLINTNEXTLINE(bugprone-exception-escape)
	~FileHandle();
	[[nodiscard]] struct stat stat() const;
	friend struct MemMappedFile;
};

FileHandle::FileHandle(std::filesystem::path const& path, int const flags) {
	fd = openat(AT_FDCWD, path.c_str(), flags);
	if (fd < 0) {
		throw_errno();
	}
}

// NOLINTNEXTLINE(bugprone-exception-escape)
FileHandle::~FileHandle() {
	if (close(fd) < 0) {
		throw_errno();
	}
}

struct stat FileHandle::stat() const {
	struct stat ret;
	if (::fstat(fd, &ret) < 0) {
		throw_errno();
	}
	return ret;
}

template <typename T, typename U>
concept SameSize = sizeof(T) == sizeof(U);

struct MemMappedFile {
protected:
	FileHandle handle;
	char* data_ptr;
	size_t data_size;
public:
	[[nodiscard]] auto data() const {
		return data_ptr;
	}
	[[nodiscard]] auto size() const {
		return data_size;
	}
	[[nodiscard]] auto begin() const {
		return data_ptr;
	}
	[[nodiscard]] auto end() const {
		return data_ptr + data_size;
	}
	[[nodiscard]] auto string_view() const {
		return std::string_view{ data(), size() };
	}
	template <typename T>
	[[nodiscard]] std::basic_string_view<T> view() const requires SameSize<T, decltype(data_ptr[0])> {
		return { reinterpret_cast<T*>(data()), size() };
	}
	explicit MemMappedFile(std::filesystem::path const& filename, int mmap_flags = PROT_READ, int file_flags = O_RDONLY);
	// NOLINTNEXTLINE(bugprone-exception-escape)
	~MemMappedFile();
	MemMappedFile(MemMappedFile const&) = delete;
	MemMappedFile& operator=(MemMappedFile const&) = delete;
};

MemMappedFile::MemMappedFile(std::filesystem::path const& filename, int const mmap_flags, int const file_flags) : handle{ filename, file_flags } {
	data_size = static_cast<size_t>(handle.stat().st_size);
	data_ptr = reinterpret_cast<char*>(::mmap(nullptr, data_size, mmap_flags, MAP_PRIVATE, handle.fd, 0));
	// NOLINTNEXTLINE(performance-no-int-to-ptr)
	if (data_ptr == reinterpret_cast<char*>(MAP_FAILED)) {
		throw_errno();
	}
}
// NOLINTNEXTLINE(bugprone-exception-escape)
MemMappedFile::~MemMappedFile() {
	if (::munmap(reinterpret_cast<void*>(data_ptr), data_size) < 0) {
		throw_errno();
	}
}
}  // namespace

namespace Logic::UUID {

std::string from_file(std::filesystem::path const& filename) {
	Logging::trace("Generating UUID from file '{}'", filename);
	MemMappedFile file{ filename };
	return from_data(file.view<CryptoPP::byte>());
}

}  // namespace Logic::UUID

#else

namespace Logic::UUID {

std::string from_file(std::filesystem::path const& filename) {
	Logging::trace("Generating UUID from file '{}'", filename);
	CryptoPP::BLAKE2b hasher(false, HASH_NUM_BYTES);
	std::ifstream file{ filename };
	std::array<CryptoPP::byte, 4096> buffer;
	static_assert(sizeof(char) == sizeof(CryptoPP::byte));
	while (file.readsome(reinterpret_cast<char*>(buffer.data()), buffer.size()) == buffer.size()) {
		hasher.Update(buffer.data(), buffer.size());
	}
	return extract_digest(hasher);
}

}  // namespace Logic::UUID

#endif
