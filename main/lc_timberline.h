#pragma once

#include <stdint.h>
#include <stddef.h>

#include "rvc_spec_generated.h"

#ifdef __cplusplus
extern "C" {
#endif

// Elwell Timberline 1.5 proprietary messages (DGN 1EF65).
//
// ⚠ WHY THIS FILE EXISTS, AND WHY IT IS HAND-WRITTEN.
//
// 1EF65 is a MULTIPLEXED DGN: byte 0 is a message type that selects a
// completely different layout for the remaining seven bytes. The generic
// decoder in main.c resolves one flat field list per DGN
// (dgn_desc_t.fields[]), which structurally cannot express that — which is
// why the generated spec carries exactly one field for 1EF65, "Message Type",
// and nothing else. The variant sub-fields do exist in
// generated/rvc_defs_generated.c as UI metadata (`1EF65.84[2]` etc.), but
// nothing in the decode path can reach them.
//
// So the variants are described here instead, as ordinary field_desc_t tables
// that the dispatch loop swaps in once it has read byte 0. Same shape, same
// decoder, no special-case extraction code — and, critically, the generated
// files stay untouched (CLAUDE.md: never edit them; they come from an xlsx +
// generator that is not even in this checkout).
//
// Source: docs/Elwell Timberline 1.5 RV-C compliance-2.pdf, "Proprietary
// packets" (0x84 Timberline 1.5 Extension status, 5 s broadcast).
//
// ⚠ Byte 0 is the message type, so a variant's own fields start at byte 1 and
// the SAME byte offsets mean different things under a different type. Do not
// reuse a table across variants.

// Returns a descriptor whose fields[] match this frame's message type, or
// NULL when the type is one we do not decode (the caller should then fall
// back to the generic 1EF65 descriptor, which yields just "Message Type").
//
// `data`/`len` are the raw frame bytes; a frame shorter than one byte has no
// message type and returns NULL.
const dgn_desc_t *lc_timberline_desc_for_frame(const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
