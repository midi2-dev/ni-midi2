#pragma once

extern unsigned num_sysex_data_allocations();

#if NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR
#define SYSEX_ALLOCATOR_CAPTURE_COUNT(var) const auto var = num_sysex_data_allocations()
#define SYSEX_ALLOCATOR_VERIFY_DIFF(var, diff) EXPECT_EQ(num_sysex_data_allocations(), (var + diff))
#else
#define SYSEX_ALLOCATOR_CAPTURE_COUNT(var) (void*)0
#define SYSEX_ALLOCATOR_VERIFY_DIFF(var, diff) (void*)0
#endif
