#pragma once

namespace dev
{
namespace con
{

#if defined(_WIN32)

#define VapReset ""       // Text Reset

#define VapReset ""       // Text Reset

	// Regular Colors
#define VapBlack	""    // Black
#define VapCoal		""    // Black
#define VapGray		""    // White
#define VapWhite	""    // White
#define VapMaroon	""    // Red
#define VapRed		""    // Red
#define VapGreen	""    // Green
#define VapLime		""    // Green
#define VapOrange	""    // Yellow
#define VapYellow	""    // Yellow
#define VapNavy		""    // Blue
#define VapBlue		""    // Blue
#define VapViolet	""    // Purple
#define VapPurple	""    // Purple
#define VapTeal		""    // Cyan
#define VapCyan		""    // Cyan

#define VapBlackBold	""      // Black
#define VapCoalBold		""      // Black
#define VapGrayBold		""      // White
#define VapWhiteBold	""      // White
#define VapMaroonBold	""      // Red
#define VapRedBold		""      // Red
#define VapGreenBold	""      // Green
#define VapLimeBold		""      // Green
#define VapOrangeBold	""      // Yellow
#define VapYellowBold	""      // Yellow
#define VapNavyBold		""      // Blue
#define VapBlueBold		""      // Blue
#define VapVioletBold	""      // Purple
#define VapPurpleBold	""      // Purple
#define VapTealBold		""      // Cyan
#define VapCyanBold		""      // Cyan

	// Background
#define VapOnBlack		""       // Black
#define VapOnCoal		""		 // Black
#define VapOnGray		""       // White
#define VapOnWhite		""		 // White
#define VapOnMaroon		""       // Red
#define VapOnRed		""       // Red
#define VapOnGreen		""       // Green
#define VapOnLime		""		 // Green
#define VapOnOrange		""       // Yellow
#define VapOnYellow		""		 // Yellow
#define VapOnNavy		""       // Blue
#define VapOnBlue		""		 // Blue
#define VapOnViolet		""       // Purple
#define VapOnPurple		""		 // Purple
#define VapOnTeal		""       // Cyan
#define VapOnCyan		""		 // Cyan

	// Underline
#define VapBlackUnder	""       // Black
#define VapGrayUnder	""       // White
#define VapMaroonUnder	""       // Red
#define VapGreenUnder	""       // Green
#define VapOrangeUnder	""       // Yellow
#define VapNavyUnder	""       // Blue
#define VapVioletUnder	""       // Purple
#define VapTealUnder	""       // Cyan

#else

#define VapReset "\x1b[0m"       // Text Reset

// Regular Colors
#define VapBlack "\x1b[30m"        // Black
#define VapCoal "\x1b[90m"       // Black
#define VapGray "\x1b[37m"        // White
#define VapWhite "\x1b[97m"       // White
#define VapMaroon "\x1b[31m"          // Red
#define VapRed "\x1b[91m"         // Red
#define VapGreen "\x1b[32m"        // Green
#define VapLime "\x1b[92m"       // Green
#define VapOrange "\x1b[33m"       // Yellow
#define VapYellow "\x1b[93m"      // Yellow
#define VapNavy "\x1b[34m"         // Blue
#define VapBlue "\x1b[94m"        // Blue
#define VapViolet "\x1b[35m"       // Purple
#define VapPurple "\x1b[95m"      // Purple
#define VapTeal "\x1b[36m"         // Cyan
#define VapCyan "\x1b[96m"        // Cyan

#define VapBlackBold "\x1b[1;30m"       // Black
#define VapCoalBold "\x1b[1;90m"      // Black
#define VapGrayBold "\x1b[1;37m"       // White
#define VapWhiteBold "\x1b[1;97m"      // White
#define VapMaroonBold "\x1b[1;31m"         // Red
#define VapRedBold "\x1b[1;91m"        // Red
#define VapGreenBold "\x1b[1;32m"       // Green
#define VapLimeBold "\x1b[1;92m"      // Green
#define VapOrangeBold "\x1b[1;33m"      // Yellow
#define VapYellowBold "\x1b[1;93m"     // Yellow
#define VapNavyBold "\x1b[1;34m"        // Blue
#define VapBlueBold "\x1b[1;94m"       // Blue
#define VapVioletBold "\x1b[1;35m"      // Purple
#define VapPurpleBold "\x1b[1;95m"     // Purple
#define VapTealBold "\x1b[1;36m"        // Cyan
#define VapCyanBold "\x1b[1;96m"       // Cyan

// Background
#define VapOnBlack "\x1b[40m"       // Black
#define VapOnCoal "\x1b[100m"   // Black
#define VapOnGray "\x1b[47m"       // White
#define VapOnWhite "\x1b[107m"   // White
#define VapOnMaroon "\x1b[41m"         // Red
#define VapOnRed "\x1b[101m"     // Red
#define VapOnGreen "\x1b[42m"       // Green
#define VapOnLime "\x1b[102m"   // Green
#define VapOnOrange "\x1b[43m"      // Yellow
#define VapOnYellow "\x1b[103m"  // Yellow
#define VapOnNavy "\x1b[44m"        // Blue
#define VapOnBlue "\x1b[104m"    // Blue
#define VapOnViolet "\x1b[45m"      // Purple
#define VapOnPurple "\x1b[105m"  // Purple
#define VapOnTeal "\x1b[46m"        // Cyan
#define VapOnCyan "\x1b[106m"    // Cyan

// Underline
#define VapBlackUnder "\x1b[4;30m"       // Black
#define VapGrayUnder "\x1b[4;37m"       // White
#define VapMaroonUnder "\x1b[4;31m"      // Red
#define VapGreenUnder "\x1b[4;32m"       // Green
#define VapOrangeUnder "\x1b[4;33m"      // Yellow
#define VapNavyUnder "\x1b[4;34m"        // Blue
#define VapVioletUnder "\x1b[4;35m"      // Purple
#define VapTealUnder "\x1b[4;36m"        // Cyan

#endif

}

}
