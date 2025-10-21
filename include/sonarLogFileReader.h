// little-endian
// byte: 		1
// ushort:		2
// uint:		4
// short:		2
// int:			4
// float:		4

// general file structure
// 1. Header
// 2. List of Frames (last frame may have incomplete sounding/bounce data)

#include <cstddef>
#include <bitset>
#include <vector>
#include <array>
#include <iostream>
#include <iomanip>
#include <variant>


namespace SonarBuilder
{
	namespace LogFile
	{	
		enum class Format
		{
			slg,
			sl2,
			sl3,
		};
		
		struct Header
		{
			unsigned short 	format;
			unsigned short 	version;
			unsigned short 	bytesPerSounding;
			std::byte      	debug;
			std::byte 		unknown1;
			unsigned short 	unknown2;
		};
			
		namespace Frame
		{
			namespace Format1
			{
				enum class Flags
				{
					unknown0				= 1 << 0,
					unknown1 				= 1 << 1,
					altitudeValid			= 1 << 2,
					upperLimitValid			= 1 << 3,
					temp2Valid				= 1 << 4,
					temp3Valid				= 1 << 5,
					waterSpeedValid			= 1 << 6,
					positionValid			= 1 << 7,
					depthInvalid			= 1 << 8,	// flag is inverted
					surfaceDepthValid		= 1 << 9,
					topOfBottomDepthValid	= 1 << 10,
					columnIs50kHz			= 1 << 11,
					timeOffsetValid			= 1 << 12,
					speedAndTrackValid		= 1 << 13,
					unused					= 1 << 14,
				};
				
				struct Metadata
				{
					unsigned short	flags;
					float			lowerLimit;
					float			waterDepth;
					float			upperLimit;
					float			waterTempInCel;
					float			waterSpeed;
					unsigned int	northingInMercatorMeters;
					unsigned int	eastingInMercatorMeters;
					float			surfaceDepth;
					float			topOfBottomDepth;
					float			temp2InCel;
					float			temp3InCel;
					float			unknownDepth;
					float			unknown1;
					float			unknown2;
					float			timeOffset;
					float			nauticalSpeed;
					float			trackInRadians;
					float			altitude;
					unsigned short	packetSize;
				};

				struct Soundingdata
				{
					std::vector<std::byte> values;
				};

				struct Frame
				{
					Metadata metadata;
					Soundingdata soundingdata;
				};

				std::ostream& operator<<(std::ostream& out, const std::byte& b)
				{
					return out << std::to_integer<int>(b);
				}

				std::ostream& operator<<(std::ostream& out, const SonarBuilder::LogFile::Frame::Format1::Metadata& metadata)
				{
					out << "======================= FORMAT 1 SAMPLE VALUES =======================\n";
					out << "----------------- METADATA ----------------\n";

					const std::bitset<16> flags { metadata.flags };
					out << std::setw(40) << std::left << "flags: " 			<< std::right << flags << '\n';
					out << std::setw(40) << std::left << "lowerLimit: " 	<< std::right << metadata.lowerLimit << '\n';
					out << std::setw(40) << std::left << "waterDepth: " 	<< std::right << metadata.waterDepth << '\n';
					out << std::setw(40) << std::left << "upperLimit: " 	<< std::right << metadata.upperLimit << '\n';
					out << "======================================================================\n";

					return out;
				}
			}

			namespace Format2
			{	
				struct Metadata
				{
					unsigned int	frameOffset;
					unsigned int	lastPrimaryChannelOffset;
					unsigned int	lastSecondaryChannelOffset;
					unsigned int	lastDownscanChannelOffset;
					unsigned int	lastLeftSidescanChannelOffset;
					unsigned int	lastRightSidescanChannelOffset;
					unsigned int	lastCompositeSidescanChannelOffset;

					unsigned short	frameSizeInBytes;
					unsigned short	previousFrameSizeInBytes;

					unsigned short	channelType;
					unsigned short	packetSize;
					unsigned int	frameIndex;
					
					float	upperLimitInFeet;
					float	lowerLimitInFeet;

					unsigned short	unknown1;
					std::byte 		unknown2;
					std::byte 		unknown3;
					std::byte 		unknown4;

					std::byte 	frequency;

					unsigned short	unknown5;
					unsigned short	unknown6;
					unsigned short 	unknown7;

					int creationTimestamp;

					float	waterDepthInFeet;
					float	keelDepthInFeet;

					std::byte 		unknown8;
					std::byte 		unknown9;
					unsigned short 	unknown10;
					std::byte 		unknown11;
					std::byte 		unknown12;
					unsigned short	unknown13;
					float			unknown14;
					float	 		unknown15;
					float	 		unknown16;
					float	 		unknown17;
					std::byte 		unknown18;
					std::byte 		unknown19;
					std::byte 		unknown20;
					std::byte 		unknown21;

					float	gpsSpeedInKnots;
					float	waterTempInCel;
					int		eastingInMercatorMeters;
					int		northingInMercatorMeters;
					float	waterSpeed;
					float	trackInRadians;
					float	altituteAboveSeaLevelInFeet;
					float	headingInRadians;

					unsigned short	flags;

					unsigned short 	unknown22;
					std::byte 	 	unknown23;
					std::byte 	 	unknown24;
					std::byte 		unknown25;
					std::byte 		unknown26;

					unsigned int 	timeOffsetInMs;

					friend std::ostream& operator<<(std::ostream& out, const SonarBuilder::LogFile::Frame::Format2::Metadata& metadata);
 				};

				struct Soundingdata
				{
					std::vector<std::byte> values;
				};

				struct Frame
				{
					Metadata metadata;
					Soundingdata soundingdata;
				};

				std::ostream& operator<<(std::ostream& out, const std::byte& b)
				{
					return out << std::to_integer<int>(b);
				}
				
				std::ostream& operator<<(std::ostream& out, const SonarBuilder::LogFile::Frame::Format2::Metadata& metadata)
				{
					out << "======================= FORMAT 2 SAMPLE VALUES =======================\n";
					out << "----------------- METADATA ----------------\n";
					out << std::setw(40) << std::left << "frameOffset: " 						<< std::right << metadata.frameOffset << '\n';
					out << std::setw(40) << std::left << "lastPrimaryChannelOffset: " 			<< std::right << metadata.lastPrimaryChannelOffset << '\n';
					out << std::setw(40) << std::left << "lastPrimaryChannelOffset: " 			<< std::right << metadata.lastSecondaryChannelOffset << '\n';
					out << std::setw(40) << std::left << "lastDownscanChannelOffset: " 			<< std::right << metadata.lastDownscanChannelOffset << '\n';
					out << std::setw(40) << std::left << "lastLeftSidescanChannelOffset: " 		<< std::right << metadata.lastLeftSidescanChannelOffset << '\n';
					out << std::setw(40) << std::left << "lastRightSidescanChannelOffset: " 	<< std::right << metadata.lastRightSidescanChannelOffset << '\n';
					out << std::setw(40) << std::left << "lastCompositeSidescanChannelOffset: " << std::right << metadata.lastCompositeSidescanChannelOffset << "\n\n";
				
					out << std::setw(40) << std::left << "frameSizeInBytes: " 			<< std::right << metadata.frameSizeInBytes << '\n';
					out << std::setw(40) << std::left << "previousFrameSizeInBytes: " 	<< std::right << metadata.previousFrameSizeInBytes << "\n\n";
				
					out << std::setw(40) << std::left << "channelType: " 	<< std::right << metadata.channelType << '\n';
					out << std::setw(40) << std::left << "packetSize: " 	<< std::right << metadata.packetSize << '\n';
					out << std::setw(40) << std::left << "frameIndex: " 	<< std::right << metadata.frameIndex << "\n\n";
				
					out << std::setw(40) << std::left << "upperLimitInFeet: " << std::right << metadata.upperLimitInFeet << '\n';
					out << std::setw(40) << std::left << "lowerLimitInFeet: " << std::right << metadata.lowerLimitInFeet << "\n\n";
				
					out << std::setw(40) << std::left << "unknown1: "	<< std::right << metadata.unknown1 << '\n';
					out << std::setw(40) << std::left << "unknown2: " 	<< std::right << metadata.unknown2 << '\n';
					out << std::setw(40) << std::left << "unknown3: " 	<< std::right << metadata.unknown3 << '\n';
					out << std::setw(40) << std::left << "unknown4: " 	<< std::right << metadata.unknown4 << "\n\n";
				
					out << std::setw(40) << std::left << "frequency: " << std::right << metadata.frequency << "\n\n";
				
					out << std::setw(40) << std::left << "unknown5: " << std::right << metadata.unknown5 << '\n';
					out << std::setw(40) << std::left << "unknown6: " << std::right << metadata.unknown6 << '\n';
					out << std::setw(40) << std::left << "unknown7: " << std::right << metadata.unknown7 << "\n\n";
				
					out << std::setw(40) << std::left << "creationTimestamp: " << std::right << metadata.creationTimestamp << "\n\n";
				
					out << std::setw(40) << std::left << "waterDepthInFeet: " 	<< std::right << metadata.waterDepthInFeet << '\n';
					out << std::setw(40) << std::left << "keelDepthInFeet: "	<< std::right << metadata.keelDepthInFeet << "\n\n";
				
					out << std::setw(40) << std::left << "unknown8: " 	<< std::right << metadata.unknown8 << '\n';
					out << std::setw(40) << std::left << "unknown9: " 	<< std::right << metadata.unknown9 << '\n';
					out << std::setw(40) << std::left << "unknown10: " 	<< std::right << metadata.unknown10 << '\n';
					out << std::setw(40) << std::left << "unknown11: " 	<< std::right << metadata.unknown11 << '\n';
					out << std::setw(40) << std::left << "unknown12: " 	<< std::right << metadata.unknown12 << '\n';
					out << std::setw(40) << std::left << "unknown13: " 	<< std::right << metadata.unknown13 << '\n';
					out << std::setw(40) << std::left << "unknown14: " 	<< std::right << metadata.unknown14 << '\n';
					out << std::setw(40) << std::left << "unknown15: " 	<< std::right << metadata.unknown15 << '\n';
					out << std::setw(40) << std::left << "unknown16: " 	<< std::right << metadata.unknown16 << '\n';
					out << std::setw(40) << std::left << "unknown17: " 	<< std::right << metadata.unknown17 << '\n';
					out << std::setw(40) << std::left << "unknown18: " 	<< std::right << metadata.unknown18 << '\n';
					out << std::setw(40) << std::left << "unknown19: " 	<< std::right << metadata.unknown19 << '\n';
					out << std::setw(40) << std::left << "unknown20: " 	<< std::right << metadata.unknown20 << '\n';
					out << std::setw(40) << std::left << "unknown21: " 	<< std::right << metadata.unknown21 << "\n\n";
				
					out << std::setw(40) << std::left << "gpsSpeedInKnots: " 				<< std::right << metadata.gpsSpeedInKnots << '\n';
					out << std::setw(40) << std::left << "waterTempInCel: " 				<< std::right << metadata.waterTempInCel << '\n';
					out << std::setw(40) << std::left << "eastingInMercatorMeters: " 		<< std::right << metadata.eastingInMercatorMeters << '\n';
					out << std::setw(40) << std::left << "northingInMercatorMeters: " 		<< std::right << metadata.northingInMercatorMeters << '\n';
					out << std::setw(40) << std::left << "waterSpeed: " 					<< std::right << metadata.waterSpeed << '\n';
					out << std::setw(40) << std::left << "trackInRadians: " 				<< std::right << metadata.trackInRadians << '\n';
					out << std::setw(40) << std::left << "altituteAboveSeaLevelInFeet: "	<< std::right << metadata.altituteAboveSeaLevelInFeet << '\n';
					out << std::setw(40) << std::left << "headingInRadians: " 				<< std::right << metadata.headingInRadians << "\n\n";
				
					out << std::setw(40) << std::left << "flags: " << std::right << metadata.flags << "\n\n";
				
					out << std::setw(40) << std::left << "unknown22: " << std::right << metadata.unknown22 << '\n';
					out << std::setw(40) << std::left << "unknown23: " << std::right << metadata.unknown23 << '\n';
					out << std::setw(40) << std::left << "unknown24: " << std::right << metadata.unknown24 << '\n';
					out << std::setw(40) << std::left << "unknown25: " << std::right << metadata.unknown25 << '\n';
					out << std::setw(40) << std::left << "unknown26: " << std::right << metadata.unknown26 << "\n\n";
				
					out << std::setw(40) << std::left << "timeOffsetInMs: " << std::right << metadata.timeOffsetInMs << '\n';
					out << "======================================================================\n";

					return out;
				}
			}

			namespace Format3
			{
				struct Metadata
				{
					unsigned int	frameOffset;
					unsigned int	unknown1;

					unsigned short	frameSizeInBytes;
					unsigned short	previousFrameSizeInBytes;

					unsigned short 	channelType;

					unsigned short	unknown2;

					unsigned int	frameIndex;

					float	upperLimitInFeet;
					float	lowerLimitInFeet;

					std::array<std::byte, 12>	unknown3;

					unsigned int	creationTimestamp;

					unsigned short packetSize;

					unsigned short unknown4;

					float waterDepthInFeet;

					std::byte frequency;

					std::array<std::byte, 11>	unknown5;

					float	unknown6;
					float	unknown7;
					float	unknown8;
					float	unknown9;

					std::byte unknown10;
					std::byte unknown11;
					std::byte unknown12;
					std::byte unknown13;

					float	gpsSpeedInKnots;

					float	waterTempInCel;

					int	eastingInMercatorMeters;
					int northingInMercatorMeters;

					unsigned int	waterSpeedInKnots;

					float trackInRadians;
					float altituteAboveSeaLevelInFeet;
					float headingInRadians;

					unsigned int unknown14;

					std::byte unknown15;
					std::byte unknown16;
					std::byte unknown17;
					std::byte unknown18;

					unsigned int	timeOffsetInMs;
					unsigned int	lastPrimaryChannel;
					unsigned int	lastSecondaryChannel;
					unsigned int	lastDownscanChannel;
					unsigned int	lastLeftSidescanChannel;
					unsigned int	lastRightSidescanChannel;
					unsigned int	lastCompositeSidescanChannel;

					std::array<unsigned int, 3> unknown19;

					unsigned int	last3dscanChannel;
				};

				struct Channel7Soundingdata
				{
					std::vector<std::byte> values;
				};

				struct Channel8Soundingdata
				{
					std::vector<unsigned short> values;
				};

				struct Channel9Soundingdata
				{
					unsigned int	headerSizeInBytes;
					unsigned int	section1SizeInBytes;
					unsigned int 	section2SizeInBytes;
					unsigned int	section3SizeInBytes;
					unsigned int	section3aSizeInBytes;
					unsigned int	section3bSizeInBytes;

					std::array<unsigned int, 7>	unknown1{ 0 };

					float unknown2;

					std::array<unsigned int, 5> unknown3{ 0 };

					std::vector<std::pair<float, float>> section1;
					std::vector<std::pair<float, float>> section2;

					// section 3
					unsigned int	unknown4;
					unsigned int	unknown5;
					unsigned int	unknown6;
					unsigned int	unknown7;

					std::variant<std::vector<float>, std::vector<unsigned int>> uknown8;

					std::array<std::byte, 4> unknown9;
					std::byte unknown10;

					std::vector<std::byte> padding;
				};

				struct Frame
				{
					Metadata metadata;
					std::variant<Channel7Soundingdata, Channel8Soundingdata, Channel9Soundingdata> soundingdata;
				};
			}
		}
	}
}
