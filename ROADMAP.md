# Morphogenesis Sandbox: Roadmap

A long-term, part-time project: one engine (C++20, raylib, Dear ImGui) that hosts many artificial life "scenarios": cellular automata, reaction-diffusion, agent systems, and growth models; first on the CPU, then on the GPU.

## Phase 0: Foundation

* [x] CMake build system setup (raylib + ImGui + rlImGui)
* [x] Basic application loop: window creation, fixed-timestep accumulator, UI panel (FPS, pause, step)
* [x] Verify Debug and Release builds

## Phase 1: CPU Cellular Automata

* [x] `Grid<T>` class (flat `std::vector<T>`, width/height, `at(x, y)`)
* [x] Render the grid by writing pixels to a texture (`UpdateTexture`, nearest filtering)
* [ ] Conway's Game of Life with double buffering
* [ ] Mouse painting and erasing, random fill, clear
* [ ] Controls: pause, single step, speed slider
* [ ] Generalize to Golly notation (B/S) and add popular presets (see [Wikipedia](https://en.wikipedia.org/wiki/Life-like_cellular_automaton))
* [ ] RLE pattern loader (copy a few patterns from the LifeWiki into `assets/patterns/`), place by click
* [ ] Brian's Brain (3 states) with a state-to-color palette
* [ ] Langton's Ant and a generalized multi-color version
* [ ] Wireworld
* [ ] 1D Wolfram rules (rule number 0-255, rows scroll downward)
* [ ] Cyclic cellular automaton
* [ ] Simple benchmark: cells updated per second, shown in the UI
* [ ] Optimization pass: profile with `perf`, try removing the modulo from the hot loop, then bitpacking (optional)
* [ ] Camera: pan and zoom with mouse wheel and drag

## Phase 2: Architecture Abstraction

* [ ] Define `Scenario` interface (`init`, `update`, `render`, `drawUI`, `shutdown`)
* [ ] Scenario registry and UI combo-box switcher
* [ ] Refactor Phase 1 systems into isolated scenarios
* [ ] Shared camera and infrastructure
* [ ] Write `docs/architecture.md` (what exists and why)

## Phase 3: First Shaders & GPU

* [ ] GLSL shader loading and hot-reloading system
* [ ] RAII wrapper for render textures (move-only, frees in the destructor)
* [ ] Ping-pong pair of render textures
* [ ] Life step as a fragment shader with wraparound
* [ ] Separate display shader (color maps, optional grid lines)
* [ ] Paint on the GPU (draw to the current buffer with the mouse)
* [ ] Verify GPU matches CPU on the same seed for N generations
* [ ] Brian's Brain on the GPU (state packed in a channel)
* [ ] Refactor Phase 2 scenarios to use shaders
* [ ] Multiple simulation steps per frame, show steps/sec
* [ ] Compare CPU vs GPU performance for different grid sizes

## Phase 4: Continuous Grid Systems

Infrastructure:
* [ ] Float render textures via `rlgl` (RGBA32F), verify values survive a round trip
* [ ] Reusable fullscreen-pass helper (shader + inputs + target)
* [ ] Parameter presets in the UI

Gray-Scott reaction-diffusion:
* [ ] 5-point and 9-point Laplacian, Euler step
* [ ] Feed/kill sliders, several substeps per frame
* [ ] Presets (spots, stripes, worms, coral, mitosis) and a feed/kill map across the canvas
* [ ] Seed by painting chemical B with the mouse

More patterns:
* [ ] Multi-scale Turing patterns (McCabe)
* [ ] Belousov-Zhabotinsky-style excitable media (Greenberg-Hastings, Hodgepodge machine)
* [ ] Larger-than-Life (big neighborhoods, naive convolution)

SmoothLife:
* [ ] Ring/disk kernels, integrals computed by brute-force sampling
* [ ] Smooth transition function, find the stable "gliders" from Rafler's paper
* [ ] Optimization: separable or reduced kernels (FFT as an optional stretch)

Lenia:
* [ ] Single-channel Lenia: kernel, growth function, time step `dt`
* [ ] Orbium and a few other known species (load from parameter files)
* [ ] Kernel visualizer in the UI
* [ ] Multi-channel Lenia
* [ ] FFT-based convolution (optional)

## Phase 5: Particles & Agents

Infrastructure:
* [ ] Spatial hash grid (uniform cells, neighbor queries)
* [ ] Instanced or batched drawing for many particles
* [ ] Quadtree for comparison (optional)

Scenarios:
* [ ] Boids on the CPU (separation, alignment, cohesion, spatial hash)
* [ ] Particle Life (matrix of attractions, CPU first)
* [ ] Physarum slime mold (Jones model): agents sense, rotate, move, deposit; trail map diffuses and decays
* [ ] Move Physarum to compute shaders (SSBOs): first 10k agents, then 1M
* [ ] Differential growth: polyline with springs, edge subdivision, repulsion
* [ ] Space colonization in 2D
* [ ] Diffusion-limited aggregation
* [ ] L-systems with a turtle renderer
* [ ] Cellular Potts model of cell sorting (optional)

## Phase 6: Engine Maturity

* [ ] State serialization (JSON save/load for presets)
* [ ] Media export (screenshots and image sequences)
* [ ] Performance overlay (frame time graph, per-pass GPU timers)
* [ ] Tests for CPU rules (doctest or Catch2) wired into CTest
* [ ] CI (GitHub Actions) building Debug and Release
* [ ] README with GIFs, build instructions and scenario list

## Phase 7: Frontier

Neural cellular automata:
* [ ] Study "Growing Neural Cellular Automata" (Mordvintsev et al., Distill)
* [ ] Train a small NCA in Python/PyTorch (outside the engine)
* [ ] Export the weights, run inference in a fragment shader
* [ ] Interactive damage and regeneration demo
* [ ] Training inside the engine (optional)

Evolution and discovery:
* [ ] Genetic search over Lenia/Gray-Scott parameters with a fitness function
* [ ] Novelty search and MAP-Elites for creature discovery
* [ ] Flow-Lenia or other mass-conserving systems
* [ ] Evolving agents in a simple ecosystem (Polyworld/Avida-style)

## Backlog

* Mouse as an environmental force
* Tweakable color palettes, post-processing
* Gallery page with GIFs of cool discoveries
