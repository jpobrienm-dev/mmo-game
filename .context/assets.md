# Asset Reference — mmo-game

All assets live under `assets/` relative to the repo root. Assets are loaded at runtime via SDL2_image (`IMG_Load`). The CMake build symlinks `assets/` into `build/assets/` post-build so the binary finds them via relative paths.

---

## Tiles — `assets/tiles/`

Tile sheets use a **48×48 px grid**. The interior fill cell (no edge blending) is always at column 2, row 2 → `src = {96, 96, 48, 48}`.

| File | Used by | Notes |
|------|---------|-------|
| `tilemap_color1.png` | `TILE_GRASS_0`, `TILE_STONE`, `TILE_RAMP_L`, `TILE_RAMP_R`, `TILE_WALL` | Master sheet; grass + all rocky/structural tiles |
| `tilemap_color2.png` | `TILE_GRASS_1` | Medium green grass |
| `tilemap_color3.png` | `TILE_GRASS_2` | Rich green grass |
| `tilemap_color4.png` | `TILE_GRASS_3` | Olive / arid grass |
| `tilemap_color5.png` | `TILE_GRASS_4` | Teal / wet / forest grass |
| `water_bg.png` | `TILE_WATER` | Single 64×64 tile |
| `water_foam.png` | Water animation overlay | 3072×192 strip — 16 frames of 192×192, ~8 fps |
| `shadow.png` | Unused / utility | Small shadow sprite |

### Sprite coordinates in `tilemap_color1.png`

| Tile | Col | Row | `src` rect (x, y, w, h) |
|------|-----|-----|--------------------------|
| Interior grass fill | 2 | 2 | 96, 96, 48, 48 |
| `TILE_STONE` | 8 | 5 | 384, 240, 48, 48 |
| `TILE_RAMP_L` | 7 | 3 | 336, 144, 48, 48 |
| `TILE_RAMP_R` | 9 | 3 | 432, 144, 48, 48 |
| `TILE_WALL` | 7 | 5 | 336, 240, 48, 48 |

> `TILE_STONE`, `TILE_RAMP_L`, `TILE_RAMP_R`, and `TILE_WALL` all share the `tilemap_color1` texture with `TILE_GRASS_0` — skip them in `tile_sprites_free()` to avoid double-free.

---

## Props — `assets/props/`

### Bushes — `assets/props/bushes/`

| File | `DecoType` | Scale (`deco_tile_scale`) | Biome |
|------|-----------|-----------|-------|
| `bush_01.png` | `DECO_BUSH_0` | <!-- TODO: verify exact scale value --> | GRASS biome |
| `bush_02.png` | `DECO_BUSH_1` | <!-- TODO: verify --> | GRASS biome |
| `bush_03.png` | `DECO_BUSH_2` | <!-- TODO: verify --> | GRASS biome |
| `bush_04.png` | — | — | Not yet used |

### Trees — `assets/props/trees/`

| File | `DecoType` | Scale (`deco_tile_scale`) | Frames | Biome |
|------|-----------|-----------|--------|-------|
| `tree_01.png` | `DECO_TREE_GREEN` | <!-- TODO: verify --> | auto-detect | FOREST biome |
| `tree_02.png` | `DECO_TREE_ORANGE` | <!-- TODO: verify --> | auto-detect | dense GRASS biome |
| `tree_03.png` | — | — | — | Not yet used |
| `tree_04.png` | `DECO_PALM` | <!-- TODO: verify --> | auto-detect | SAND biome |

### Stumps — `assets/props/trees/`

| File | Notes |
|------|-------|
| `stump_01.png` | Not yet used |
| `stump_02.png` | Not yet used |
| `stump_03.png` | Not yet used |
| `stump_04.png` | Not yet used |

### Rocks — `assets/props/rocks/`

| File | Notes |
|------|-------|
| `rock_01.png` | Not yet used |
| `rock_02.png` | Not yet used |
| `rock_03.png` | Not yet used |
| `rock_04.png` | Not yet used |

> Rock props are natural candidates for stone biome decoration once `DECO_ROCK_*` types are added.

### Water Rocks — `assets/props/water_rocks/`

| File | Notes |
|------|-------|
| `water_rock_01.png` | Not yet used |
| `water_rock_02.png` | Not yet used |
| `water_rock_03.png` | Not yet used |
| `water_rock_04.png` | Not yet used |

---

## Buildings — `assets/buildings/`

Four faction colour variants: `black/`, `blue/`, `red/`, `yellow/`.

Each variant contains: `archery.png`, `barracks.png`, `castle.png`, `house1.png`, `house2.png`, `house3.png`, `monastery.png`, `tower.png`

Not yet integrated into the game world.

---

## Sprites — `assets/sprites/`

### Player / Unit types — `assets/sprites/units/`

Three unit classes, each in four faction colours (`black/`, `blue/`, `red/`, `yellow/`):

| Class | Animations |
|-------|-----------|
| `warrior/` | `idle.png`, `run.png`, `guard.png`, `attack1.png`, `attack2.png` |
| `archer/` | `idle.png`, `run.png`, `shoot.png`, `arrow.png` |
| `lancer/` | `idle.png`, `run.png`, and 8 directional attack/defence variants |

### Enemies — `assets/sprites/enemies/`

| Enemy | Animations |
|-------|-----------|
| `bear/` | idle, run, attack |
| `boat/` | idle |
| `gnoll/` | idle, walk, throw, bone, hit |
| `gnome/` | idle, run, attack |
| `harpoon_fish/` | idle, run, throw, harpoon |
| `lancer/` | idle, run, attack |
| `lizard/` | idle, run, attack, hit |
| `minotaur/` | idle, walk, attack, guard |
| `paddle_fish/` | idle, run, attack |
| `panda/` | idle, run, attack, guard |
| `shaman/` | idle, run, attack, explosion, projectile |
| `skull/` | idle, run, attack, guard |
| `snake/` | idle, run, attack |
| `spider/` | idle, run, attack |
| `thief/` | idle, run, attack |
| `troll/` | idle, walk, windup, attack, clubpart1, clubpart2, recovery, dead |
| `turtle/` | idle, walk, attack, guard_in, guard_out |

---

## Effects — `assets/fx/`

| File | Description |
|------|-------------|
| `dust_01.png` | Dust puff animation |
| `dust_02.png` | Dust puff animation (variant) |
| `explosion_01.png` | Explosion animation |
| `explosion_02.png` | Explosion animation (variant) |
| `fire_01.png` | Fire animation |
| `fire_02.png` | Fire animation (variant) |
| `fire_03.png` | Fire animation (variant) |
| `water_splash.png` | Water splash animation |

Not yet integrated.

---

## Resources — `assets/resources/`

### Gold — `assets/resources/gold/`

6 gold stone variants, each with a highlight version: `gold_stone_01..06.png` + `*_highlight.png`. Also `gold_resource.png` + `gold_resource_highlight.png`.

### Meat — `assets/resources/meat/`

| File | Description |
|------|-------------|
| `sheep_idle.png` | Sheep idle animation |
| `sheep_move.png` | Sheep walk animation |
| `sheep_grass.png` | Sheep grazing |
| `meat_resource.png` | Dropped meat icon |

### Wood — `assets/resources/wood/`

`wood_resource.png` — dropped wood icon.

### Tools — `assets/resources/tools/`

`tool_01.png` through `tool_04.png` — tool item sprites.

---

## Maps — `assets/maps/`

| File | Notes |
|------|-------|
| `map01.map` | Static map file — format not yet documented; world is primarily procedural |
