#ifndef BUILD_INFO_H
#define BUILD_INFO_H

// Build information
#define HE3_VERSION_MAJOR 0
#define HE3_VERSION_MINOR 1
#define HE3_VERSION_PATCH 0
#define HE3_BUILD_NAME "Sirius"

// Version string macros
#define HE3_VERSION_STRING "0.1.0"
#define HE3_FULL_VERSION_STRING "0.1.0-Sirius"

// Build date and time (will be set by build system)
#ifndef BUILD_DATE
#define BUILD_DATE "Unknown"
#endif

#ifndef BUILD_TIME
#define BUILD_TIME "Unknown"
#endif

// Build information string
#define HE3_BUILD_INFO_STRING "Build: " HE3_BUILD_NAME " (" BUILD_DATE " " BUILD_TIME ")"

#endif // BUILD_INFO_H
