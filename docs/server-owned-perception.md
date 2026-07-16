# Server-Owned Perception

Rosakasa is currently an ESP32-first experiment, but the important idea is bigger
than the display target: the client should not be the authority on what exists,
what happened, or even what the player sees.

The client sends intent:

```text
move north
attack target 42
interact with door 3
use item 9
```

The server decides the result, then sends only the visual/state output that this
client is allowed to know.

## Current Embedded Shape

For the ESP32/ST7789 target, the client is intentionally small:

```text
C client:
  game loop
  input
  framebuffer
  websocket request batching
  cached render responses
  apply returned spans

Elixir server:
  render math
  visibility/perception rules
  command decoding
  span generation
```

The server receives compact binary draw commands and returns compact binary
spans:

```text
y, x_start, x_end, intensity
```

That makes filled shapes cheap. A `100x100` filled square becomes 100 horizontal
spans instead of 10,000 individual pixels.

## Why This Matters

If the server owns perception, the client cannot inspect local memory and learn
secrets that were never sent.

That allows real hidden information:

```text
player A sees a doorway
player B sees a wall
player C sees an enemy
player D sees nothing
```

Useful systems:

- fog of war
- stealth and invisibility
- hallucinations
- dreams and visions
- curses or blindness
- hidden rooms
- player-specific history
- server-generated mysteries
- anti-cheat visibility filtering

The core rule:

```text
If it is secret, do not ship it to the client.
```

## Future Desktop Shape

A desktop fork should keep the same authority model, but it should not send
pixels or spans for 3D rendering.

For desktop-class hardware:

```text
C/GPU client:
  input
  audio
  assets
  animation
  local interpolation
  GPU rendering

Elixir server:
  accounts
  world state
  authority
  persistence
  visibility/perception filtering
  local or remote networking
```

The server should send filtered state:

```text
entity 45 moved to x/y/z
enemy 8 started attack animation
door 3 opened
hallucination mesh 12 is visible to player 7
```

The GPU client renders locally from that state.

## MMORPG Direction

This architecture is interesting for an online RPG because modified clients
remain weak. They can change local rendering or send fake inputs, but the server
does not accept client claims like:

```text
I have 999 gold
I teleported
I killed that enemy
I looted that item
```

The server accepts only intent, validates it, updates the world, and sends each
player a filtered version of reality.

That makes room for mysteries that cannot be datamined from shipped client
files:

- rooms that exist only after world conditions change
- NPCs that appear only to specific players
- histories and myths that are revealed per account
- world events triggered by collective behavior
- secret assets streamed only when discovered
- generic assets combined with server-side behavior

## Project Path

The ESP32 version should come first because its constraints force the protocol
to stay honest:

```text
small binary messages
server-owned rendering
dirty updates
cache-friendly primitives
no client trust
hard bandwidth limits
```

Once the embedded protocol is stable, a desktop fork can reuse the philosophy
without copying the exact renderer:

```text
rosakasa_engine_embedded:
  framebuffer, spans, display transport

rosakasa_engine_desktop:
  GPU renderer, state protocol, local/remote server
```

