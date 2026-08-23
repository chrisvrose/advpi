set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# set(CMAKE_STAGING_PREFIX /home/devel/stage)

set(TOOLCHAIN_PREFIX aarch64-linux-gnu)
set(CMAKE_SYSROOT /usr/aarch64-linux-gnu)


set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# Search packages only in target environment
# found while googling - not covered in the cmake documentation for some reason
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
