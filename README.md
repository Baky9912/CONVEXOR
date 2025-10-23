# 🌀 Convexor

2024 Computer Graphics project at [RAF](https://raf.edu.rs/)

**Convexor** is a minimalistic 2D graphics engine and game written in **C**, built entirely around a single core operation —  
**drawing one pixel at coordinate `(x, y)` with color `(r, g, b)`**.  

Everything visible on screen — lines, polygons, collisions, textures, camera motion, and even AI mob behavior —  
is derived from that fundamental function. The project uses **RAFGL** and **GLFW** only to open a window  
and display the pixel buffer; all geometry and rendering are done manually on the CPU.

<img width="698" height="389" alt="1" src="https://github.com/user-attachments/assets/d85e628b-091a-4b87-8811-c93725b9806d" />


---

## 🎯 Educational Focus

Convexor was designed as a **teaching tool for computer graphics and geometric algorithms**,  
illustrating how an entire interactive world can be built from first principles:

- How pixels become **geometry** through rasterization.  
- How geometry becomes **motion** through mathematical transformations.  
- How procedural generation and simple AI create **emergent behavior**.  
- How all of it integrates into a coherent **rendering pipeline** with no engine, shaders, or GPU acceleration.

This project aims to bridge the gap between **algorithmic geometry** and **visual computing**,  
demonstrating the inner workings of a software renderer through readable, modular C code.
<img width="698" height="389" alt="1" src="https://github.com/user-attachments/assets/de89ba11-9244-4ba0-a123-38297c39d92b" />
<img width="694" height="394" alt="2" src="https://github.com/user-attachments/assets/c9644117-b218-4035-9fe9-ceb99fad21f9" />

---

## ⚙️ Overview

- **Language:** C (C99)  
- **Libraries:** [GLFW](https://www.glfw.org/), OpenGL (via Glad), RAFGL  
- **Build system:** `make all`  
- **Dependencies:** Standard C libraries + GLFW  

Convexor renders directly to a framebuffer using **world-space** and **local-space** coordinates.  
Procedural generation populates the environment with convex polygons, food shards, and concave AI mobs.  
Each frame is the result of thousands of explicit pixel writes.

---

## 🧠 Gameplay and Simulation

You control a **convex polygon** in a procedurally generated world.  
By consuming **food shards**, your polygon grows and gains mass.  
Around you, **concave AI mobs** wander and chase you using geometry-based pursuit logic.  

The **camera** operates in world space, smoothly following the player’s center of mass,  
and a **vignette post-effect** applies a darkened falloff toward the screen edges.

---

## 🧩 Code Structure

| Module | Description |
|---------|-------------|
| `geometry.h` | Core geometric primitives (vectors, lines, triangles, rectangles) and transformations. |
| `convexhull.h` / `concavehull.h` | Data structures for convex and concave polygons with triangulation and area computation. |
| `polygon.h` | High-level polygon intersection, bounding boxes, and rasterization. |
| `segment.h` | Line clipping and incremental line drawing (Cohen–Sutherland style). |
| `food_shards.h` | Procedural spawning and regeneration of collectible convex shapes. |
| `hostile_mob.h` | Concave **AI mobs** with scale-based movement, pursuit, and reaction logic. |
| `protagonist.h` | Player entity with keyboard-controlled rotation, motion, and scaling. |
| `camera.h` | World-to-screen transformation, parallax background, and vignette rendering. |
| `main_state.c` | Core loop integrating input, geometry, AI, and rendering. |
| `myrandom.h` | Randomization utilities for procedural generation. |
| `globals.h` | Shared constants and world configuration. |

---

## 🧮 Core Loop

Each frame (`main_state_update()`):

1. **Input & Movement**  
   `take_input() → move_protagonist() → move_hmobs()`
2. **Procedural World Updates**  
   `protagonist_eat_food(), hmobs_eat_food(), protagonist_fight_enemies()`
3. **Camera & Rendering**  
   `move_camera(), draw_background(), draw_all_shards(), draw_hostile_mobs(), draw_protagonist()`
4. **Post-Processing**  
   `postfx()` applies vignette and tonal correction, then the pixel buffer is blitted via `rafgl_texture_show()`.

Each object — food, mob, player — exists in world coordinates,  
projected to the screen purely through mathematical transformation.

---

## 🧱 Building & Running

### Requirements
- GLFW 3.x  
- OpenGL 3.3+  
- GCC / Clang / MinGW  

### Configuration
Edit your GLFW path in the **Makefile**:
```make
GLFW_DIR = C:/Program Files (x86)/GLFW
GLFW_INC = $(GLFW_DIR)/include
GLFW_LIB = $(GLFW_DIR)/lib
```

## ⚡ Performance Notes

Convexor performs **all rasterization on the CPU**, pixel by pixel —  
there are **no GPU shaders**, **no multithreading**, and **no hardware acceleration**.  

Performance scales linearly with resolution:  
it runs smoothly at **800×600**, but higher resolutions will cause **noticeable slowdowns**,  
especially when many **AI mobs** or complex geometric shapes are active on screen.  

This is **intentional** — the project is designed as an **educational example** of  
how a **software rasterizer** works internally.  
Each frame directly reveals the **computational cost** of geometry and rendering operations  
before any optimization, threading, or GPU offloading is applied.


## 🧾 Research and Supporting Documentation

2024 Project Managment Homework at [RAF](https://raf.edu.rs/)

### 1. Analize rizika (`research/Analize rizika.pdf`)
Focuses on **risk management and market planning** for Convexor.  
Covers how technical, design, and marketing teams handle development risks,  
bug testing, performance issues, and market timing.  
Highlights the game’s **unique pixel-by-pixel rendering** and minimal tech requirements.  
Also defines **budget allocation** and **expected profitability** after 3–6 months  
(contentReference[oaicite:1]{research/Analize rizika.pdf}).

---

### 2. Eksperiment (`research/Eksperiment.pdf`)
Describes **market validation experiments** for Convexor, including:
- **Letter of Intent:** email testing to gauge real buying intent for preorders.  
- **Skin Mock Sale:** simulated cosmetic sales to measure in-game purchase interest.  
- **Agar.io Comparison:** behavioral reference study on cosmetic motivation.  
Each test defines clear hypotheses, metrics, and success thresholds for player interest  
(contentReference[oaicite:2]{research/Eksperiment.pdf}).

---

### 3. Kanvas model gejmifikacije (`research/Kanvas model gejmifikacije.pdf`)
Models Convexor’s **game dynamics, mechanics, and reward systems** using a gamification canvas.  
Analyzes player **progression, gain-streak cycles,** and **social motivation** (friend scores, visual customization).  
Breaks down the system into components like engine logic, collision tracking, polygon randomization, and skin shop.  
Includes basic **market cost analysis** and **platform distribution** projections (Steam, Epic, Itch.io)  
(contentReference[oaicite:3]{research/Kanvas model gejmifikacije.pdf}).
  

<img width="694" height="386" alt="3" src="https://github.com/user-attachments/assets/54788e70-43af-491b-870e-cb16ba102d75" />

