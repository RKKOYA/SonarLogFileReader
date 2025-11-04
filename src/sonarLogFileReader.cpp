#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <variant>
#include "../include/SonarLogFileReader.h"


std::ostream& operator<<(std::ostream& out, const std::byte& b)
{
	return out << std::to_integer<int>(b);
}

void readHeader(std::ifstream& inf, SonarBuilder::LogFile::Header* fileHeader)
{
	inf.read(reinterpret_cast<char*>(&(fileHeader->format)), 			sizeof(fileHeader->format));
	inf.read(reinterpret_cast<char*>(&(fileHeader->version)), 			sizeof(fileHeader->version));
	inf.read(reinterpret_cast<char*>(&(fileHeader->bytesPerSounding)), 	sizeof(fileHeader->bytesPerSounding));
	inf.read(reinterpret_cast<char*>(&(fileHeader->debug)), 			sizeof(fileHeader->debug));
	inf.read(reinterpret_cast<char*>(&(fileHeader->unknown1)), 			sizeof(fileHeader->unknown1));

	// unknown2 may or may not exist
	if (static_cast<SonarBuilder::LogFile::Format>(fileHeader->format) == SonarBuilder::LogFile::Format::slg)
	{
		inf.read(reinterpret_cast<char*>(&(fileHeader->unknown2)), sizeof(fileHeader->unknown2));	
	}
	else
	{
		fileHeader->unknown2 = 0;
	}
}

void printHeader(const SonarBuilder::LogFile::Header* fileHeader)
{
	std::cout << "======================= HEADER SAMPLE VALUES =========================\n";
	std::cout << std::setw(20) << std::left << "format: " 			<< std::right << fileHeader->format << '\n';
	std::cout << std::setw(20) << std::left << "version: " 			<< std::right << fileHeader->version << '\n';
	std::cout << std::setw(20) << std::left << "bytesPerSounding: " << std::right << fileHeader->bytesPerSounding << '\n';
	std::cout << std::setw(20) << std::left << "debug: " 			<< std::right << fileHeader->debug << '\n';
	std::cout << std::setw(20) << std::left << "unknown1: " 		<< std::right << fileHeader->unknown1 << '\n';
	std::cout << std::setw(20) << std::left << "unknown2: " 		<< std::right << fileHeader->unknown2 << '\n';
	std::cout << "======================================================================\n";
}

std::ostream& operator<<(std::ostream& out, const SonarBuilder::LogFile::Format& format)
{
	switch (format)
	{
		case SonarBuilder::LogFile::Format::slg:	out << "slg";	break;
		case SonarBuilder::LogFile::Format::sl2:	out << "sl2";	break;
		case SonarBuilder::LogFile::Format::sl3:	out << "sl3";	break;
		default:									out << "???"; 	break;
	}
	return out;
}

unsigned short operator&(const SonarBuilder::LogFile::Frame::Format1::Flags& flag, const unsigned int& flags)
{
	return static_cast<unsigned short>(flag) & flags;
}

void readFrame(std::ifstream& inf, SonarBuilder::LogFile::Frame::Format1::Frame* frame, const SonarBuilder::LogFile::Header* fileHeader)
{
	using Flags = SonarBuilder::LogFile::Frame::Format1::Flags;
	// custom deserialization due to flags
	// Credit: Herbert Oppman (https://www.memotech.franken.de/FileFormats/Navico_SLG_Format.pdf, page 5)
	inf.read(reinterpret_cast<char*>(&(frame->metadata.flags)), 	 sizeof(frame->metadata.flags));
	inf.read(reinterpret_cast<char*>(&(frame->metadata.lowerLimit)), sizeof(frame->metadata.lowerLimit));

	if (!static_cast<unsigned short>(Flags::depthInvalid & frame->metadata.flags)) // flag is inverted
		inf.read(reinterpret_cast<char*>(&(frame->metadata.waterDepth)), sizeof(frame->metadata.waterDepth));
	else
		inf.seekg(inf.tellg() + static_cast<std::istream::pos_type>(sizeof(frame->metadata.waterDepth)));

	if (static_cast<unsigned short>(Flags::upperLimitValid & frame->metadata.flags))
		inf.read(reinterpret_cast<char*>(&(frame->metadata.upperLimit)), sizeof(frame->metadata.upperLimit));

	if (static_cast<unsigned short>(Flags::waterTempValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.waterTempInCel)), sizeof(frame->metadata.waterTempInCel));

	if (static_cast<unsigned short>(Flags::waterSpeedValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.waterSpeed)), sizeof(frame->metadata.waterSpeed));

	if (static_cast<unsigned short>(Flags::positionValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.northingInMercatorMeters)), sizeof(frame->metadata.northingInMercatorMeters));

	if (static_cast<unsigned short>(Flags::positionValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.eastingInMercatorMeters)), sizeof(frame->metadata.eastingInMercatorMeters));

	if (static_cast<unsigned short>(Flags::surfaceDepthValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.surfaceDepth)), sizeof(frame->metadata.surfaceDepth));

	if (static_cast<unsigned short>(Flags::topOfBottomDepthValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.topOfBottomDepth)), sizeof(frame->metadata.topOfBottomDepth));

	if (static_cast<unsigned short>(Flags::temp2Valid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.temp2InCel)), sizeof(frame->metadata.temp2InCel));

	if (static_cast<unsigned short>(Flags::temp3Valid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.temp3InCel)), sizeof(frame->metadata.temp3InCel));

	if (static_cast<unsigned short>((Flags::unknown0Valid & frame->metadata.flags) && (Flags::speedAndTrackValid & frame->metadata.flags)))
		inf.read(reinterpret_cast<char*>(&(frame->metadata.unknownDepth)), sizeof(frame->metadata.unknownDepth));

	if (static_cast<unsigned short>(Flags::unknown1Valid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.unknown1)), sizeof(frame->metadata.unknown1));

	if (static_cast<unsigned short>(Flags::unknown1Valid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.unknown2)), sizeof(frame->metadata.unknown2));

	inf.read(reinterpret_cast<char*>(&(frame->metadata.timeOffset)), sizeof(frame->metadata.timeOffset));

	if (static_cast<unsigned short>(Flags::speedAndTrackValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.nauticalSpeed)), sizeof(frame->metadata.nauticalSpeed));

	if (static_cast<unsigned short>(Flags::speedAndTrackValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.trackInRadians)), sizeof(frame->metadata.trackInRadians));

	if (static_cast<unsigned short>(Flags::altitudeValid) & frame->metadata.flags)
		inf.read(reinterpret_cast<char*>(&(frame->metadata.altitude)), sizeof(frame->metadata.altitude));

	inf.read(reinterpret_cast<char*>(&(frame->metadata.packetSize)), sizeof(frame->metadata.packetSize));
	
	frame->soundingdata.values.reserve(static_cast<std::size_t>(fileHeader->bytesPerSounding) - (static_cast<std::size_t>(inf.tellg()) - sizeof(SonarBuilder::LogFile::Header)));
	frame->soundingdata.values.resize(static_cast<std::size_t>(fileHeader->bytesPerSounding) - (static_cast<std::size_t>(inf.tellg()) - sizeof(SonarBuilder::LogFile::Header)));	
	inf.read(reinterpret_cast<char*>((frame->soundingdata.values.data())), frame->soundingdata.values.size());
}

void readFrame(std::ifstream& inf, SonarBuilder::LogFile::Frame::Format2::Frame* frame, const SonarBuilder::LogFile::Header* fileHeader)
{
	inf.read(reinterpret_cast<char*>(&(frame->metadata)), sizeof(frame->metadata));
	
	// Credit: Herbert Oppman (https://www.memotech.franken.de/FileFormats/Navico_SLG_Format.pdf)
	frame->soundingdata.values.reserve(static_cast<std::size_t>(frame->metadata.packetSize));
	frame->soundingdata.values.resize(static_cast<std::size_t>(frame->metadata.packetSize));
	inf.read(reinterpret_cast<char*>(frame->soundingdata.values.data()), (frame->metadata.packetSize));
}

void readFrame(std::ifstream& inf, SonarBuilder::LogFile::Frame::Format3::Frame* frame, const SonarBuilder::LogFile::Header* fileHeader)
{
	
}

void printFrame(const SonarBuilder::LogFile::Frame::Format1::Frame* frame)
{
	std::cout << frame->metadata << '\n';
}

void printFrame(const SonarBuilder::LogFile::Frame::Format2::Frame* frame)
{
	std::cout << frame->metadata << '\n';
}

void printFrame(const SonarBuilder::LogFile::Frame::Format3::Frame* frame)
{
	
}

int main()
{
	using SonarBuilder::LogFile::Format;
	using SonarBuilder::LogFile::Header;
	using Format1Frame = SonarBuilder::LogFile::Frame::Format1::Frame;
	using Format2Frame = SonarBuilder::LogFile::Frame::Format2::Frame;
	using Format3Frame = SonarBuilder::LogFile::Frame::Format3::Frame;
	
	// header
	std::string fileName{ "testing/sonarOnly/sonar.slg" };
	std::ifstream inf{ fileName, std::ios::binary };
	if (!inf)
	{
		std::cerr << "Could not open " << fileName << '\n';
		return 1;
	}
	
	auto fileHeader{ std::make_unique<Header>() };

	try
	{
		readHeader(inf, fileHeader.get());
	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << "File Read Error: " << e.what() << '\n';
		inf.close();
		return 1;
	}
	
	printHeader(fileHeader.get());
	
	// format 1, 2, or 3 frames
	std::variant<std::unique_ptr<Format1Frame>, std::unique_ptr<Format2Frame>, std::unique_ptr<Format3Frame>> frame;
	using namespace std::literals;
	
	if (fileHeader->format == static_cast<unsigned short>(SonarBuilder::LogFile::Format::slg))
	{
		frame = std::make_unique<Format1Frame>();
	}
	else if (fileHeader->format == static_cast<unsigned short>(SonarBuilder::LogFile::Format::sl2))
	{
		frame = std::make_unique<Format2Frame>();
	}
	else if (fileHeader->format == static_cast<unsigned short>(SonarBuilder::LogFile::Format::sl3))
	{
		frame = std::make_unique<Format3Frame>();
	}
	else
	{
		std::cerr << "Unsupported file format encountered.\n";
		return 1;
	}

	try
	{
		std::visit([&inf, &fileHeader](auto& frame)
		{
			readFrame(inf, frame.get(), fileHeader.get());
		}, frame);
	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << "File Read Error:" << e.what() << '\n';
		inf.close();
		return 1;
	}
	
	std::visit([](auto& frame)
	{
		printFrame(frame.get());
	}, frame);

	inf.close();
	return 0;
}
