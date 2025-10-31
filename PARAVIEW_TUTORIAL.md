# ParaView Visualization Tutorial
## Wind Tunnel Simulation Guide

---

## Table of Contents

1. [Before You Start](#before-you-start)
2. [Running the Simulation](#running-the-simulation)
3. [ParaView Basics](#paraview-basics)
4. [Tutorial 1: Visualizing Smoke](#tutorial-1-visualizing-smoke)
5. [Tutorial 2: Adding Obstacles](#tutorial-2-adding-obstacles)
6. [Tutorial 3: Creating Streamlines](#tutorial-3-creating-streamlines)
7. [Tutorial 4: Combined Visualization](#tutorial-4-combined-visualization)
8. [Tutorial 5: Advanced Techniques](#tutorial-5-advanced-techniques)
9. [What to Observe](#what-to-observe)
10. [Troubleshooting](#troubleshooting)

---

## Before You Start

### What You'll Learn

This tutorial teaches you how to:
- Load 3D simulation data into ParaView
- Visualize smoke (density) fields
- Display 3D obstacles
- Create streamlines showing flow direction
- Combine multiple visualizations
- Export images and animations

### What You Need

✅ Completed simulation run (output_*.vti files in `build/` folder)  
✅ ParaView installed (download from paraview.org)  
✅ ~15-30 minutes for full tutorial

---

## Running the Simulation

### Step 1: Generate Data

Open terminal and run:

```bash
cd build
./fluid_sim -n 64 -s 1000 --smoke-steps 500 -o 10
```

**Wait for completion** (~50 seconds). You should see:
```
Simulation complete!
VTK files saved (XML format).
```

### Step 2: Verify Output Files

Check that files were created:
```bash
ls output_*.vti | head -5
```

You should see: `output_0000.vti`, `output_0010.vti`, etc.

**Total files**: ~100 files (1000 steps ÷ 10 output interval)

---

## ParaView Basics

### Opening ParaView

1. Launch **ParaView** application
2. You'll see three main areas:
   - **Pipeline Browser** (left): Shows loaded data and filters
   - **3D View** (center): Main visualization window
   - **Properties Panel** (bottom-left): Controls for selected item

### Key Controls

**Mouse:**
- **Left-click + drag**: Rotate view
- **Middle-click + drag**: Pan (move)
- **Scroll wheel**: Zoom in/out
- **Right-click**: Context menu

**Toolbar Buttons:**
- **Apply**: Confirm changes (important!)
- **Play (▶)**: Animate through time
- **Reset camera**: Fit view to data

---

## Tutorial 1: Visualizing Smoke

### Goal
See smoke flowing through the wind tunnel and around obstacles.

---

### Step 1.1: Load the Data

1. **File → Open** (or Ctrl+O)
2. Navigate to your `build/` folder
3. **Select ALL output files**:
   - Type `output_` in filename box
   - It should show: `output_..vti`
   - Click **OK**
4. In Pipeline Browser, you'll see `output_..vti`
5. **Click "Apply"** in Properties panel

✅ **Check**: You should see a gray/white box in the 3D view

---

### Step 1.2: Color by Smoke Density

1. Find toolbar at top with dropdown saying **"Solid Color"**
2. Click dropdown → select **"density"**
3. Data should now show colors (blue/red)

✅ **Check**: You see colored patterns (smoke)

---

### Step 1.3: Choose Better Colors

1. Next to "density" dropdown, there's a color bar
2. Click the **color map dropdown** (says "Cool to Warm")
3. Try these options:
   - **Rainbow**: Full spectrum
   - **Jet**: Blue → Red
   - **Viridis**: Perceptually uniform
4. Choose your favorite!

💡 **Tip**: "Cool to Warm" or "Rainbow" work well for smoke

---

### Step 1.4: Adjust Color Range

Sometimes colors don't match data well:

1. Click the **⟲ icon** (Rescale to Data Range)
2. Or right-click color bar → "Rescale to Data Range"

✅ **Check**: Colors now show smoke clearly

---

### Step 1.5: Play Animation

1. Find **VCR controls** at top: `|◄ ◄ ▶ ►| ⟲`
2. Click **Play (▶)**
3. Watch smoke flow from left to right!

**Controls:**
- **▶**: Play animation
- **⏸**: Pause
- **◄/►**: Step backward/forward one frame
- **|◄/►|**: Jump to first/last frame

✅ **Check**: Smoke moves smoothly through tunnel

---

### Step 1.6: Adjust View Angle

Get a better perspective:

1. **Left-click + drag**: Rotate to see from angle
2. **Scroll**: Zoom in to see details
3. **Middle-click + drag**: Pan to center interesting features

Try viewing from:
- **Side**: See smoke hitting obstacles
- **Top**: See vortex patterns from above
- **Angle**: 3D perspective view

---

### 🎓 What You Learned

✅ Load time-series data  
✅ Color by field values  
✅ Choose colormaps  
✅ Play animations  
✅ Navigate 3D view  

**Next**: Add 3D obstacles to see what smoke is flowing around!

---

## Tutorial 2: Adding Obstacles

### Goal
Display the sphere and cylinder obstacles in 3D.

---

### Step 2.1: View Obstacle Field

1. With `output_..vti` selected in Pipeline Browser
2. Change coloring from **"density"** → **"obstacle"**
3. You'll see:
   - **Red/Yellow** = obstacles (value 1)
   - **Blue** = fluid (value 0)

✅ **Check**: You can see where obstacles are, but it's not clear

---

### Step 2.2: Create Obstacle Surface

Make obstacles look like solid 3D shapes:

1. Select `output_..vti` in Pipeline Browser
2. **Filters → Common → Contour**
3. In Properties panel:
   - **Contour By**: Select **"obstacle"**
   - **Isosurfaces**: Type **0.5** (or click + and set to 0.5)
4. **Click "Apply"**

✅ **Check**: You see clean 3D surfaces of obstacles

---

### Step 2.3: Make Obstacles Gray

1. Select **"Contour1"** in Pipeline Browser
2. In toolbar, change coloring to **"Solid Color"**
3. Click color square → choose **gray** or **white**
4. **Optional**: Adjust "Opacity" slider to 0.8 (slightly transparent)

✅ **Check**: Obstacles look like solid gray shapes

---

### Step 2.4: Show Both Smoke and Obstacles

1. Look at Pipeline Browser (left side)
2. You'll see:
   - 👁 `output_..vti`
   - 👁 `Contour1`
3. **Click eye icons** (👁) to toggle visibility
4. **Enable both** to see smoke AND obstacles!

✅ **Check**: Gray obstacles with colored smoke around them

---

### Step 2.5: Adjust Layers

If obstacles block smoke:

1. Select `output_..vti`
2. Find **"Opacity"** in Properties panel
3. Set to **0.5** (semi-transparent smoke)
4. Now you see smoke AND obstacles clearly!

**Layer order:**
- **Obstacles**: Opaque (0.8-1.0) - solid shapes
- **Smoke**: Semi-transparent (0.3-0.5) - see through to streamlines

---

### 🎓 What You Learned

✅ Use Contour filter for iso-surfaces  
✅ Display multiple datasets simultaneously  
✅ Control visibility with eye icons  
✅ Adjust transparency/opacity  

**Next**: Add streamlines to show flow direction!

---

## Tutorial 3: Creating Streamlines

### Goal
Show the paths that air follows through the tunnel.

---

### Step 3.1: Add Stream Tracer Filter

1. Select `output_..vti` in Pipeline Browser (the original data)
2. **Filters → Common → Stream Tracer**
3. Properties panel opens with many options

---

### Step 3.2: Configure Seed Points

Seed points are where streamlines start. Set:

**Seed Type**: Select **"Point Source"** (dropdown)

**Center**: `[10, 32, 32]`
- This places seeds upstream (x=10), center height (y=32), center depth (z=32)

**Radius**: `20`
- Seeds spread in sphere of radius 20

**Number of Points**: `100`
- Creates 100 streamlines

---

### Step 3.3: Configure Integration

Scroll down in Properties:

**Integration Direction**: **FORWARD** (flows downstream only)

**Integrator Type**: **Runge-Kutta 4-5** (accurate)

**Maximum Streamline Length**: `500`
- Lines stop after traveling 500 units

**Maximum Steps**: `2000` (default is usually fine)

**Click "Apply"**

✅ **Check**: You see lines flowing from left to right!

---

### Step 3.4: Color by Velocity

Show flow speed:

1. Select **"StreamTracer1"** in Pipeline Browser
2. Change coloring to **"velocity"** or **"velocity_magnitude"**
3. Choose colormap: **"Cool to Warm"** or **"jet"**

**Color meaning:**
- **Blue**: Slow flow (~0-2 m/s)
- **Green/Yellow**: Medium flow (~3-4 m/s)
- **Red**: Fast flow (~5+ m/s)

✅ **Check**: Streamlines show different colors indicating speed

---

### Step 3.5: Make Lines Thicker

If lines are too thin to see:

**Option A**: Increase Line Width
1. In Properties, find **"Line Width"**
2. Set to **2** or **3**

**Option B**: Use Tube Representation
1. Change **"Representation"** from "Surface" to **"Tubes"**
2. Set **"Radius"**: 0.5

✅ **Check**: Streamlines are now visible and easy to follow

---

### Step 3.6: Add More Streamlines (Optional)

Create streamlines for all three smoke streams:

**For cylinder stream:**
1. Select `output_..vti` again
2. Add new Stream Tracer
3. Center: `[10, 20, 20]` (lower position)

**For upper stream:**
1. Add another Stream Tracer
2. Center: `[10, 48, 32]` (upper position)

Now you have streamlines matching all three smoke streams!

---

### 🎓 What You Learned

✅ Use Stream Tracer filter  
✅ Configure seed points  
✅ Color by vector fields  
✅ Adjust streamline appearance  
✅ Create multiple stream tracers  

**Next**: Combine everything into one beautiful visualization!

---

## Tutorial 4: Combined Visualization

### Goal
Show smoke, obstacles, and streamlines together in one view.

---

### Step 4.1: Enable All Three

In Pipeline Browser, make sure these have eye icons (👁) checked:

- ✅ **output_..vti** (smoke)
- ✅ **Contour1** (obstacles)
- ✅ **StreamTracer1** (flow lines)

---

### Step 4.2: Adjust Transparency

**Smoke** (output_..vti):
- Select it
- Opacity: **0.3-0.4** (see through)

**Obstacles** (Contour1):
- Select it
- Opacity: **0.9-1.0** (mostly solid)
- Color: Gray or white

**Streamlines** (StreamTracer1):
- Select it
- Opacity: **1.0** (fully visible)
- Keep bright colors

---

### Step 4.3: Optimize View

**Camera angle:**
1. Rotate view to see from side and slightly above
2. Zoom to fit obstacles in frame
3. Pan to center interesting features

**Or use preset:**
- View → Camera → Isometric View

---

### Step 4.4: Play Animation

1. Click **Play (▶)**
2. Observe:
   - Smoke flowing along streamline paths
   - Vortices forming behind obstacles
   - Flow separating at obstacle edges
   - Speed changes (color variations on streamlines)

---

### Step 4.5: Toggle Layers

Experiment with combinations:

**Smoke + Obstacles only:**
- Hide streamlines (👁 off on StreamTracer1)
- See smoke patterns clearly

**Streamlines + Obstacles only:**
- Hide smoke
- See clean flow lines

**All three:**
- Full visualization with all information

---

### 🎓 What You Learned

✅ Layer multiple visualizations  
✅ Balance transparency for clarity  
✅ Optimize camera angles  
✅ Toggle visibility for different insights  

**Next**: Advanced techniques for publication-quality images!

---

## Tutorial 5: Advanced Techniques

### Technique 1: Cross-Section Slice

See flow patterns in 2D plane:

1. Select `output_..vti`
2. **Filters → Common → Slice**
3. Set **Plane**: "Y Normal" (horizontal cut)
4. Set **Origin**: `[0, 32, 0]` (middle height)
5. **Apply**
6. Color by **density** or **velocity_magnitude**

**Result**: Top-down view showing vortex patterns

---

### Technique 2: Volume Rendering

Make smoke look like real 3D fog:

1. Select `output_..vti`
2. Change **Representation** to **"Volume"**
3. Click **"Edit"** next to color map
4. In **Transfer Function Editor**:
   - Adjust opacity curve (bottom)
   - Create bell curve centered on medium density
5. **Apply**

**Result**: 3D volumetric smoke visualization

---

### Technique 3: Velocity Glyphs

Show velocity as arrows:

1. Select `output_..vti`
2. **Filters → Common → Glyph**
3. **Glyph Type**: Arrow
4. **Orientation Array**: velocity
5. **Scale Array**: velocity_magnitude
6. **Scale Factor**: 2 (adjust to taste)
7. **Apply**

**Result**: Arrows showing flow direction and speed

---

### Technique 4: Vorticity Visualization

See rotation/swirling:

1. Select `output_..vti`
2. **Filters → Alphabetical → Gradient Of Unstructured DataSet**
3. **Scalar Array**: None
4. **Compute Vorticity**: ✅ Check
5. **Apply**
6. Color by **"Vorticity"** magnitude

**Result**: Highlights vortices and rotation

---

### Technique 5: Save Images/Animations

**Save single image:**
1. File → Save Screenshot
2. Choose resolution (e.g., 1920×1080)
3. Select PNG format
4. Click OK

**Save animation:**
1. File → Save Animation
2. Choose format (AVI, MP4, or PNG sequence)
3. Set frame rate (e.g., 10 fps)
4. Click OK
5. Wait for export

---

### Technique 6: Split View Comparison

Compare different visualizations:

1. Click **split view** icon in toolbar
2. Choose horizontal or vertical split
3. Each view can show different:
   - Time steps
   - Coloring (density vs velocity)
   - Filters
4. Link cameras: View → Link Camera

**Use for**: Compare cylinder vs sphere wakes side-by-side

---

### 🎓 What You Learned

✅ Create 2D slices  
✅ Use volume rendering  
✅ Show vector fields with glyphs  
✅ Calculate derived quantities (vorticity)  
✅ Export images and animations  
✅ Use split views for comparison  

---

## What to Observe

### Phenomenon 1: Von Kármán Vortex Street

**Where**: Behind cylinder obstacle

**What to see**:
- Alternating vortices (left-right-left-right pattern)
- Regular spacing and frequency
- Vortices travel downstream

**Best view**: Top-down slice through middle of cylinder

**Timesteps**: 100-500 (after initial startup)

---

### Phenomenon 2: Flow Separation

**Where**: At obstacle surfaces

**What to see**:
- Smoke/streamlines detach from surface
- Separation point on cylinder: ~120° from front
- Creates recirculation zone behind obstacle

**Best view**: Side view with streamlines

**Timesteps**: 50-150 (watch it develop)

---

### Phenomenon 3: Wake Turbulence

**Where**: Region downstream of obstacles

**What to see**:
- Chaotic smoke patterns
- Mixing of different flow speeds
- Lower velocity (blue colors on streamlines)
- Smoke accumulation

**Best view**: Smoke density animation

**Timesteps**: 150+ (fully developed turbulence)

---

### Phenomenon 4: Cylinder vs Sphere Comparison

**Cylinder**:
- 2D-like flow (consistent along height)
- Strong, regular vortex shedding
- Sharp periodic pattern

**Sphere**:
- 3D flow structures
- More complex vortex patterns
- Less regular shedding

**Best view**: Split screen showing both simultaneously

---

### Phenomenon 5: Boundary Layer

**Where**: Very close to obstacle surfaces

**What to see**:
- Velocity drops from 5 m/s to near zero
- Thin layer of slow-moving fluid
- Enhanced drag effect makes it thicker

**Best view**: Velocity contours with tight spacing near walls

---

## Troubleshooting

### Problem: Can't see anything in ParaView

**Solution**:
1. Click **Apply** button (very common mistake!)
2. Check eye icons (👁) are enabled
3. Click **Reset Camera** (camera might be pointing wrong direction)
4. Rescale colors to data range (⟲ icon)

---

### Problem: Colors look wrong

**Solutions**:
- Click **⟲** (Rescale to Data Range)
- Try different colormap (Rainbow, Jet, Viridis)
- Check you're coloring by correct field (density, not obstacle)
- Adjust opacity if too transparent

---

### Problem: Streamlines don't show

**Solutions**:
- Check **Maximum Streamline Length** isn't too small (use 500)
- Verify **Seed Type** is Point Source with reasonable radius
- Make sure **Integration Direction** is FORWARD
- Increase **Number of Points** to 100+
- Check data actually has velocity field (it should)

---

### Problem: Animation is choppy

**Solutions**:
- Reduce output frequency: run with `-o 20` instead of `-o 10`
- Use smaller grid: `-n 32` instead of `-n 64`
- Close other applications (free up memory)
- Simplify visualization (hide smoke, show only streamlines)

---

### Problem: Obstacles not visible

**Solutions**:
- Use Contour filter with **isosurface = 0.5**
- Make sure you selected "obstacle" field
- Change to **Solid Color** and pick contrasting color (gray)
- Increase **Opacity** to 1.0 (fully opaque)

---

### Problem: Everything is transparent

**Solutions**:
- Select each item in Pipeline Browser
- Set **Opacity** to 1.0 for all
- Or use **Volume** representation for smoke
- Check **Representation** isn't set to "Outline"

---

### Problem: Can't see smoke behind obstacles

**Solutions**:
- Reduce **obstacle opacity** to 0.7-0.8 (slightly transparent)
- Reduce **smoke opacity** to 0.3-0.5
- Use **Volume** rendering for smoke
- Rotate view to angle where both are visible

---

### Problem: File won't load

**Solutions**:
- Check files exist: `ls build/output_*.vti`
- Make sure simulation completed successfully
- Try loading single file first: `output_0100.vti`
- Check file isn't corrupted (size > 0 bytes)

---

## Quick Reference

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| **Ctrl+O** | Open file |
| **Ctrl+S** | Save state |
| **Space** | Play/Pause animation |
| **Ctrl+Shift+S** | Save screenshot |
| **R** | Reset camera |
| **3** | Surface representation |
| **2** | Wireframe |
| **1** | Outline |

### Common Filters

| Filter | Purpose |
|--------|---------|
| **Contour** | Create iso-surfaces (obstacles) |
| **Stream Tracer** | Flow lines following velocity |
| **Slice** | 2D cross-section |
| **Glyph** | Arrows/shapes at points |
| **Calculator** | Compute new fields |
| **Gradient** | Calculate derivatives |

### Representations

| Type | Use For |
|------|---------|
| **Surface** | Standard 3D view |
| **Volume** | 3D fog/smoke effects |
| **Points** | Sparse data |
| **Wireframe** | See structure |
| **Outline** | Bounding box only |

---

## Next Steps

**Completed this tutorial?** Try these:

1. **Change wind speed**: Edit `main.cpp` line 128, recompile
2. **Different grid size**: Run with `-n 128` for higher detail
3. **Longer simulation**: Run with `-s 2000`
4. **Export video**: Create animation for presentation
5. **Custom colormap**: Edit transfer function for artistic effects

**Learn more:**
- ParaView Guide: paraview.org/documentation
- ParaView tutorials on YouTube
- Read `CHANGELOG.md` for simulation features

---

## Summary

**You learned how to:**

✅ Load simulation data into ParaView  
✅ Visualize smoke density fields  
✅ Display 3D obstacle geometry  
✅ Create streamlines showing flow  
✅ Combine multiple visualizations  
✅ Use advanced techniques (slices, volume rendering, glyphs)  
✅ Export images and animations  
✅ Identify key flow phenomena  

**ParaView is powerful!** These basics unlock many more possibilities. Experiment and explore! 🎨
