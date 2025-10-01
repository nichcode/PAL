# Changelog

## [1.0.0] - 2025-09-27
- Initial stable release of PAL.

## [1.0.1] - 2025-10-01

**Bugfix release** - improve C/C++ interop and build integration

### Fixed
- Added extern "C" guards to all exported functions so PAL can now be linked from both **C** and **C++** projects.
- Fixed a **condition variable** bug where the wrong thread could acquire the mutex first, causing intermittent locking issues. See **tests/condvar_test.c**
- Updated premake scripts to allow **PAL to be included as a submodule or directly in another workspace** if the build system is **premake**.

### Notes
- No API or ABI changes
- Safe upgrade from **v1.0** - just rebuild your project after updating.