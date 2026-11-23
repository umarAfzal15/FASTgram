#pragma once
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace BinaryIO
{
	inline void writeString(std::ostream& out, const std::string& str)
	{
		uint32_t len = static_cast<uint32_t>(str.size());
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		if (len > 0)
		{
			out.write(str.data(), static_cast<std::streamsize>(len));
		}
		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write string");
		}
	}

	inline std::string readString(std::istream& in)
	{
		uint32_t len = 0;
		in.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (!in)
		{
			throw std::runtime_error("BinaryIO: failed to read string length");
		}
		if (len > 1000000)
		{
			throw std::runtime_error("BinaryIO: string length exceeds sanity limit");
		}

		std::string result(len, '\0');
		if (len > 0)
		{
			in.read(&result[0], static_cast<std::streamsize>(len));
			if (!in)
			{
				throw std::runtime_error("BinaryIO: failed to read string characters");
			}
		}
		return result;
	}

	inline void writeInt(std::ostream& out, int value)
	{
		int32_t v = static_cast<int32_t>(value);
		out.write(reinterpret_cast<const char*>(&v), sizeof(v));
		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write int");
		}
	}

	inline int readInt(std::istream& in)
	{
		int32_t v = 0;
		in.read(reinterpret_cast<char*>(&v), sizeof(v));
		if (!in)
		{
			throw std::runtime_error("BinaryIO: failed to read int");
		}
		return static_cast<int>(v);
	}

	inline void writeBool(std::ostream& out, bool value)
	{
		uint8_t v = value ? 1 : 0;
		out.write(reinterpret_cast<const char*>(&v), sizeof(v));
		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write bool");
		}
	}

	inline bool readBool(std::istream& in)
	{
		uint8_t v = 0;
		in.read(reinterpret_cast<char*>(&v), sizeof(v));
		if (!in)
		{
			throw std::runtime_error("BinaryIO: failed to read bool");
		}
		return v != 0;
	}

	inline void writeIntList(std::ostream& out, const std::vector<int>& values)
	{
		uint32_t count = static_cast<uint32_t>(values.size());
		out.write(reinterpret_cast<const char*>(&count), sizeof(count));
		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write int list count");
		}
		for (int val : values)
		{
			writeInt(out, val);
		}
	}

	inline std::vector<int> readIntList(std::istream& in)
	{
		uint32_t count = 0;
		in.read(reinterpret_cast<char*>(&count), sizeof(count));
		if (!in || count > 1000000)
		{
			throw std::runtime_error("BinaryIO: invalid int list count");
		}
		std::vector<int> result;
		result.reserve(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			result.push_back(readInt(in));
		}
		return result;
	}

	inline void writeStringList(std::ostream& out, const std::vector<std::string>& values)
	{
		uint32_t count = static_cast<uint32_t>(values.size());
		out.write(reinterpret_cast<const char*>(&count), sizeof(count));
		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write string list count");
		}
		for (const std::string& s : values)
		{
			writeString(out, s);
		}
	}

	inline std::vector<std::string> readStringList(std::istream& in)
	{
		uint32_t count = 0;
		in.read(reinterpret_cast<char*>(&count), sizeof(count));
		if (!in || count > 1000000)
		{
			throw std::runtime_error("BinaryIO: invalid string list count");
		}
		std::vector<std::string> result;
		result.reserve(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			result.push_back(readString(in));
		}
		return result;
	}

	inline void writeHeader(std::ostream& out, uint32_t count)
	{
		const char magic[4] = { 'F', 'T', 'G', '1' };
		uint16_t version = 1;

		out.write(magic, 4);
		out.write(reinterpret_cast<const char*>(&version), sizeof(version));
		out.write(reinterpret_cast<const char*>(&count), sizeof(count));

		if (!out)
		{
			throw std::runtime_error("BinaryIO: failed to write FTG1 header");
		}
	}

	inline uint32_t readHeader(std::istream& in)
	{
		char magic[4] = { 0, 0, 0, 0 };
		uint16_t version = 0;
		uint32_t count = 0;

		in.read(magic, 4);
		if (!in || magic[0] != 'F' || magic[1] != 'T' || magic[2] != 'G' || magic[3] != '1')
		{
			throw std::runtime_error("Damaged or unrecognized file: invalid FTG1 magic bytes");
		}

		in.read(reinterpret_cast<char*>(&version), sizeof(version));
		if (!in || version != 1)
		{
			throw std::runtime_error("Damaged or unsupported file: version mismatch");
		}

		in.read(reinterpret_cast<char*>(&count), sizeof(count));
		if (!in)
		{
			throw std::runtime_error("Damaged file: failed to read record count");
		}

		return count;
	}
}
