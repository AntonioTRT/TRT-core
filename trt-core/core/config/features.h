#pragma once

#ifndef TRT_HAS_STL
#define TRT_HAS_STL 1
#endif

#ifndef TRT_HAS_DYNAMIC_MEMORY
#define TRT_HAS_DYNAMIC_MEMORY 1
#endif

#ifndef TRT_HAS_MODULES
#define TRT_HAS_MODULES 1
#endif

#ifndef TRT_MAX_FRAME_SIZE
#define TRT_MAX_FRAME_SIZE 256
#endif

#if !TRT_HAS_STL && TRT_HAS_DYNAMIC_MEMORY
#error "TRT_HAS_DYNAMIC_MEMORY requires TRT_HAS_STL in Phase 1."
#endif