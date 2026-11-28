#pragma once
// The decoder cycle must cover the entire application command window.
#define INHABITX_VOICE_WINDOW_MS 8000u
#define INHABITX_DECODER_TIMEOUT_MS 10000u
#if INHABITX_DECODER_TIMEOUT_MS < INHABITX_VOICE_WINDOW_MS
#error "Speech decoder timeout must cover the command window"
#endif
