# Improvement Plan: mmo-game

**Generated**: 2026-03-22
**Source**: Multi-discipline expert committee review (C Systems, Game Dev, Software Architecture, Networking, Procedural Gen, Graphics/Rendering, UX/Game Design)
**Status**: Active — Tier 1 not yet started

---

## Context

Full committee discussion available in session notes. The architecture is sound — the ground work is done. This plan covers the path from "working prototype" to "MMO-ready game." Three tiers: foundation correctness fixes → core quality improvements → MMO-scale architecture.

---

## TIER 1 — Foundation Fixes (Do Before Anything Else)

Correctness and stability issues that compound if left unfixed.

| # | Action | Files Affected | Domain | Why |
|---|--------|---------------|--------|-----|
| F1 | **Eliminate double noise evaluation** — cache `elev[r][c]` in one pass in `world_gen_chunk`, derive both `TileType` and `tier` from it. Unify into `tile_classify(e, m, *tile, *tier)`. | `src/world/world_gen.c` | C, PCG | 50% reduction in world gen CPU; eliminates risk of biome tile/tier threshold drift |
| F2 | **Normalize diagonal movement** — when both `dx` and `dy` are non-zero, multiply each by `0.707f` before applying velocity | `src/core/main.c` | C, GD | **Active bug**: diagonal movement is `sqrt(2)` faster than cardinal |
| F3 | **Add `chunk_manager_free(cm)`** — even a no-op stub now; real cleanup of future resources later | `src/world/chunk_manager.c/h` | C, SA | Prevents silent future leaks; makes shutdown contract explicit |
| F4 | **4-corner AABB collision** — check all 4 corners of a `~0.8×0.8` tile bounding box before allowing movement, not just player position point | `src/core/main.c` | C, GD | **Active bug**: player clips through thin walls and corners from certain angles/speeds |
| F5 | **Pre-bake `foam_mask` in world gen** — static `uint32_t foam_mask` bitmask per chunk storing which tiles border land; computed once in `world_gen_chunk`, referenced in `chunk_render_water_anim` | `src/world/chunk.h`, `src/world/world_gen.c`, `src/world/chunk_renderer.c` | GFX, PCG | Eliminates O(water_tiles × 4) redundant neighbor-check work every frame |
| F6 | **Centralize all tuning constants in `config.h`** — move `CHUNK_W/H` (chunk.h), `CHUNK_OBJ_MAX` (world_object.h), `CHUNK_PROP_MAX` (chunk.h), `NOISE_SCALE` (world_gen.c) | `src/core/config.h` + source sites | SA | Eliminates hunt-and-peck tuning; all knobs in one place |

---

## TIER 2 — Core Quality Improvements

Unlock the next level of visual and gameplay quality.

| # | Action | Files Affected | Domain | Why |
|---|--------|---------------|--------|-----|
| Q1 | **Chunk render-to-texture cache** — bake each chunk's tiles into an `SDL_Texture` render target on first render; blit entire chunk with one `SDL_RenderCopy`. Dirty flag trivially free (static world). | `src/world/chunk_renderer.c/h`, `src/world/chunk_manager.h` (PoolSlot) | GFX | O(visible_chunks) vs O(visible_tiles) — critical at min zoom; 256× fewer draw calls per chunk |
| Q2 | **Vertical cliff scan + corner pieces** — add column scan pass in world gen, intersect with horizontal scan, emit corner `WorldObject` types | `src/world/world_gen.c`, `src/world/world_object.h`, `src/world/object_renderer.c` | PCG, GD | Transforms terrain from "horizontal bands" to genuine 2D topology |
| Q3 | **Shore/transition tiles** — `TILE_SAND`, `TILE_SHALLOW_WATER`, `TILE_DEEP_WATER` + biome blending in `biome_tile()` | `src/world/tile.h`, `src/world/world_gen.c`, `src/world/tile_sprites.c/h` | PCG, GFX | Highest single-step visual ROI; eliminates hard water/land edge |
| Q4 | **Camera smoothing + look-ahead** — lerp `cam_x/cam_y` toward target in `camera_update`; offset slightly in current movement direction | `src/camera/camera.c` | GD, UX | Most impactful single-function game feel improvement |
| Q5 | **Game state machine** — `GameState` enum (`MENU`, `PLAYING`, `PAUSED`) owned in `main.c`; gates input and update paths | `src/core/main.c`, `src/core/config.h` | SA, GD | Required before any UI, menu, or pause screen work |
| Q6 | **Input action abstraction** — `InputAction` enum + `input_is_down(ACTION_X)` API in a new `src/core/input.c/h` | `src/core/input.c/h`, `src/core/main.c` | SA, UX | Decouples input from logic; enables controller support and rebinding |
| Q7 | **Entity ID field** — add `uint32_t id` to `Player` struct | `src/entity/player.h` | NET, SA | Zero-cost now; mandatory for networking; add during any Player edit |
| Q8 | **SDL_mixer integration + ambient audio** — add `SDL_mixer` alongside `SDL2_image`; play ambient loop in game state | `CMakeLists.txt`, `src/core/main.c`, new `src/audio/` module | UX | Transforms perceived quality more than any visual change |

---

## TIER 3 — Architecture for Scale (MMO Readiness)

Architectural investments required before networking or content scale-out.

| # | Action | Files Affected | Domain | Why |
|---|--------|---------------|--------|-----|
| A1 | **Entity registry with pool** — `EntityPool` using `POOL_DEFINE`, tagged union `Entity` for player/npc/enemy/projectile types | New `src/entity/entity_registry.c/h` | SA, GD | Required for NPCs, enemies, other players, projectiles sharing animation + physics |
| A2 | **Fixed-tick simulation loop** — 20Hz sim tick separate from variable render loop; render interpolates between ticks | `src/core/main.c` | NET, GD | Foundational for deterministic netcode; also fixes dt-dependent gameplay edge cases |
| A3 | **Split `src/world/` into `terrain/` and `objects/`** — terrain: noise, world_gen, tile, tile_sprites; objects: world_object, object_renderer | Directory reorganization + `CMakeLists.txt` | SA | Prevents the world directory from becoming unnavigable |
| A4 | **Cross-chunk prop system** — world-space prop grid for large objects (trees spanning chunks); placement RNG keyed on world coords independent of chunk boundaries | `src/world/world_gen.c`, new `src/props/prop_grid.c/h` | PCG, GFX | Eliminates ugly chunk-edge foliage cutoffs |
| A5 | **Async chunk generation** — generation queue + worker thread; main thread requests, bg thread fills, atomic ready flag | `src/world/chunk_manager.c/h` | C, NET | Eliminates main-thread hitches on fast traversal; mandatory at MMO entity density |
| A6 | **Server-side position reconciliation model** — split `Player` into `predicted_pos` + `server_pos` + `render_pos` | `src/entity/player.h/c` | NET | The data layout refactor enabling client-side prediction without a full rewrite |
| A7 | **Choose and integrate ENet (or equivalent)** — UDP reliable-ordered library; add to `CMakeLists.txt` | `CMakeLists.txt`, new `src/net/` module | NET | The networking foundation; nothing else in the MMO stack works without it |
| A8 | **Y-sort depth rendering** — sort entities and foliage by world Y before blitting each frame | `src/core/main.c` or new render system | GFX, GD | Players and objects must draw behind/in-front correctly based on vertical position |

---

## Execution Timeline

```
Week 1    F1 → F2 → F4 → F5 → F3 → F6     Foundation fixes (all six)
Week 2    Q1 → Q4 → Q6                     Render cache + camera feel + input
Week 3    Q2 → Q3 → Q8                     Terrain variety + audio
Week 4    Q5 → Q7 → A3                     State machine + entity ID + dir refactor
Month 2   A1 → A2 → A8                     Entity registry + sim loop + z-sort
Month 3+  A4 → A5 → A6 → A7               Cross-chunk, async gen, net foundation
```

---

## When to Invoke This Plan

- **Starting any new feature**: check if it touches a system flagged in Tier 1 or 2 — fix the flag first.
- **Performance complaints**: Q1 (chunk texture cache) is the first thing to try.
- **Adding a second entity (NPC/enemy)**: A1 (entity registry) is a prerequisite.
- **Any networking discussion**: A2, A6, A7 must be designed together; don't start one without the others.
- **Terrain feels monotonous**: Q2 (vertical cliffs) and Q3 (shore tiles) are the levers.

<!-- context-handler: improvement-plan — generated 2026-03-22, committee review -->
