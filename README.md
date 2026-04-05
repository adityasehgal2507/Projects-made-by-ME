# Projects

A collection of simulation and creative coding projects built in C and Python.

---

## 1. Real-Time Fluid Simulator
**Language:** C  **Library:** Raylib

A grid-based interactive fluid simulation implementing the Navier-Stokes equations.

**Features**
- Left click to inject fluid and velocity
- Right click to draw walls
- Middle click to erase walls
- Real-time vector field overlay

**How it works:**  
Each frame runs three steps - diffusion (fluid spreads out), advection (fluid moves with velocity), and projection (keeps the velocity field divergence-free so it looks like real fluid).

**Build**
```bash
gcc fluid.c -lraylib -lm -o fluid
./fluid
```

---

## 2. Boids Flocking Simulation
**Language:** C  **Library:** Raylib

Simulates emergent flocking behaviour of 500 agents using three simple rules.

**Rules**
- **Separation** - avoid crowding neighbours
- **Alignment** - steer towards average heading of neighbours
- **Cohesion** - steer towards average position of neighbours

No agent is told to flock. The group behaviour emerges entirely from these local rules.

**Build**
```bash
gcc boids.c -lraylib -lm -o boids
./boids
```

---

## 3. Image-to-Braille Converter
**Language:** Python  **Libraries:** Pillow, Tkinter

Converts any image into Unicode Braille art and saves it as a `.txt` or `.html` file.

**Features**
- GUI file picker
- Atkinson dithering for accurate monochrome conversion
- HTML output with proper font rendering for Braille characters

**Usage**
```bash
pip install pillow
python main.py
```
Then select an image — output is saved as `output.txt` and opened in your browser.

---

## Dependencies
- [Raylib](https://www.raylib.com/) — for C projects
- [Pillow](https://python-pillow.org/) — for the Braille converter
