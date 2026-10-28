#  Dog 2.5D

**Custom 2.5D Movement & Object Interaction System**

A 2D stealth-puzzle platformer about a dog who ended up on the streets, built without the standard `CharacterMovementComponent`.

![Unreal Engine 5.8](https://img.shields.io/badge/Unreal%20Engine-5.8-0E1128?logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Gameplay-00599C?logo=cplusplus&logoColor=white)
![Blueprints](https://img.shields.io/badge/Blueprints-Visuals-2F80ED)

 [**Game Design Document**](https://docs.google.com/document/d/1MlYBMOL2WL9FrSRFxDr4BEb3YQDV5YuQcnI8sWkBV3o/edit?usp=sharing)

---

##  Table of Contents

- [About the Game](#-about-the-game)
- [Demo](#-demo)
- [Project Structure](#-project-structure)
- [Gameplay Systems](#-gameplay-systems)
- [Implemented Architectural Solutions & Fixes](#-implemented-architectural-solutions--fixes)
- [Bug Hunts: Case Studies](#-bug-hunts-case-studies)
- [Project Setup](#-project-setup)

---

##  About the Game

A 2D platformer with puzzle and stealth elements. The player is a dog who has lost the way home and tries to survive in a world seen through the eyes of a frightened animal. The game is told without text or dialogue: orientation comes from visuals, sounds, and indirect hints. Core mechanics are jumping, crawling, running, moving objects, and carrying small items in the mouth.

The full concept (lore, memory system, endings, capture system) lives in the **[Game Design Document](https://docs.google.com/document/d/1MlYBMOL2WL9FrSRFxDr4BEb3YQDV5YuQcnI8sWkBV3o/edit?usp=sharing)**.

This repository contains the gameplay prototype: custom pawn physics, interactive objects, stealth cover, an NPC guard dog, and a house alarm system.

---

##  Demo

Pushing and pulling a box



https://github.com/user-attachments/assets/29fd7b03-ef2e-419f-9322-1150aad29ef6




Hiding in cover



https://github.com/user-attachments/assets/954bc918-9eff-4cab-869f-55c82e56b7d4



NPC guard dog



https://github.com/user-attachments/assets/16c4d43c-03df-4bde-9e24-40fbb89be8f2


---

##  Project Structure

```
Source/Dog_2D
├── Characters
│   ├── BaseDogPawn       – shared physics and movement for every dog
│   ├── DogPawn           – the player: boxes, cover, fear
│   └── NpcDogPawn        – NPC guard dog (state machine)
├── Interactive
│   ├── InteractiveBox    – pushable / pullable box
│   ├── ItemBone          – carryable item
│   ├── StealthCover      – hiding spot
│   └── HouseAlarmActor   – house, light and human (state machine)
├── GameModes
│   └── DogGameMode       – game states and UI flow
├── Core
│   └── DogGameInstance   – level paths
└── UI
    ├── MainMenuWidget
    ├── PauseMenuWidget
    └── CaughtWidget
```

Both the player and the NPC inherit from `ABaseDogPawn`, so movement, gravity, collisions, and crouching are implemented once. Visuals and animation are driven from Blueprints through `BlueprintImplementableEvent`s.

---

##  Gameplay Systems

| System | What it does |
|---|---|
| **Custom Pawn physics** | Gravity, jump, crouch, head/body collision and depenetration without `CharacterMovementComponent`. |
| **Interactive box** | Push / pull with its own physics, wall detection and ground check. |
| **Item pickup** | The dog carries a bone in its mouth socket. |
| **Stealth cover** | The player lies down and slides along Y into cover while danger is active. |
| **NPC guard dog** | State machine: sleeps in the doghouse, chases within a leash, barks, bites, returns home. |
| **House alarm** | Barking turns on the light and brings a human to the window; the player must not be caught in the light. |
| **Game flow** | Main menu → gameplay → pause → "Caught" screen with level restart. |

---

##  Implemented Architectural Solutions & Fixes

### 1. Y-Axis Stabilization for Character and Objects

* **Box locking:** The interactive object (`AInteractiveBox`) is rigidly locked to its initial depth line (`OriginalY`) and fully isolated from any screen-depth displacement.

* **Dynamic Pawn approach:** When grabbing an object, the dog automatically performs a micro-shift along Y toward the near edge of the box via `FInterpTo` interpolation, calculating the distance based on the head's dimensions (`TargetY = BoxY + DogHeadY`). On release, the character smoothly returns to the base 2.5D line.

* **Jump centering:** When jumping onto the box from above (`IsJumping`), the target offset point is recalculated to the box's pure geometric center (`TargetY = Box->GetActorLocation().Y`), ensuring a perfect paw landing on the object.

### 2. Character Collision Optimization & X-Axis Alignment

* **Tick optimization:** Heavy physics box-traces for wall detection have been completely removed from the per-frame update. X-axis alignment (`InteractMovementX`) is triggered strictly on events: when movement bools change (`bIsPushing` / `bIsPulling`) and forcibly on the first frame of an interaction click.

* **Texture clipping fix (Z-constraint):** A geometric check of the character's height relative to the box's top has been implemented. Depth movement along Y (into the screen) is only allowed once the dog's paw bottom rises above the box's top edge (`GetBottomZ() > BoxTopZ`). This fully eliminated the bug where the character would clip into the box's side walls when jumping close to it.

### 3. Engine Physics Integration & Logic Leak Fixes

* **Wall collision speed sync:** Stopping the box on collision between the dog and walls is implemented by intercepting the `FHitResult` structure from the `AddActorWorldOffset(..., true)` function. When the character's body hits a wall, its C++ `VelocityX` is zeroed out by the engine, and the box instantly stops along with it, resetting the block when input is reversed.

* **Mirrored collision restoration:** Fixed a critical flag-leak bug. A mirrored reset of the `IgnoreActorWhenMoving(Box, false)` flag was added inside `InteractReleased`, restoring the box's collidability for the ground trace right after dragging ends.

* **Blueprint preset fix:** The box's collision preset was switched from the finicky `PhysicsBody` to a solid `BlockAllDynamic` (with object type `WorldDynamic`), allowing the shared C++ ground trace (`CheckGrounded`) to correctly read the object's top as a proper floor.

### 4. Smart Crouch & 2.5D Camera Lock

* **Feet position getter:** Implemented a custom `GetFeetZ()` function that dynamically returns the world-space Z coordinate of the paws based on body scale changes, simplifying and shortening the math checks throughout the code.

* **"Inverse" crouch logic:** Implemented a concise memory reset for the box when landing on the floor (`if (GetBottomZ() < BoxTopZ)`), while the box reference is retained in memory when the dog lies down *on top of* the box, firmly locking the Y-axis.

* **Camera Y-lock:** Inside the C++ Tick, a performant world-space Y-coordinate lock is applied to the Blueprint SpringArm component (`WorldLocation.Y = OriginalY`) based on a pointer saved in `BeginPlay`. The screen smoothly follows the character along the X and Z axes while fully ignoring depth-axis drift.

---

##  Bug Hunts: Case Studies

Two bugs where the C++ was correct, compiled, and logical, and the real cause was somewhere else.

### Case 1: The NPC sinks into the floor

**Symptoms**
- The NPC fell through the floor on the first spawn.
- If pulled out manually during Play-In-Editor, all its states and mechanics worked correctly.
- Everything worked until it went back to the doghouse, where it sank into the floor again.
- The player worked perfectly, even though both the player and the NPC inherit from the same `ABaseDogPawn`.

**Investigation**
Because both classes share one parent, the shared C++ logic was ruled out quickly. The difference had to be in the Blueprints, and in particular in how crouching was set up.

**Root cause**
Crouching shrinks the scale of the **body**. In the NPC Blueprint, the scale node was connected to the **head** component instead. The visual part of the character is scaled together with the body so that it always stays on the floor; with the wrong target, the head shrank while the body stayed unchanged relative to the visuals, and the NPC slipped into the ground every time it crouched (for example, when going back into the doghouse).

**Fix**
Reconnect the crouch scale node to the body component. No C++ changes were needed.

**Takeaway:** when shared C++ behaves differently in two child classes, compare the Blueprint wiring first.

### Case 2: The player freezes when moving into cover

**Context**
The `StealthCover` class was added to let the player hide from the house light. The cover finds the player through an overlap; when the right objects intersect, the player receives a matching bool. At danger, pressing the lie-down key should move the player along Y toward the cover. The Y-travel system already existed and was proven to work for box interaction.

**Symptoms**
- The player moved along Y into the cover and then froze.
- Only the visual left/right turning still worked.

**Investigation**
The suspicion was that the player lost the ground under their feet (the grounded bool turned `false`), and the reason was unknown. To catch it live, all player state values were printed every frame in `Tick`. The suspicion was confirmed: the grounded flag dropped and every mechanic that depends on it stopped.

Since the player never lost the ground while interacting with boxes, the Y-travel itself was fine. The problem had to be in the cover class.

**Root cause**
The cover handed the player its outermost point, even with an extra `+10` margin. But the player moves with its **whole body**, not with its edge, so the body ended up inside the cover's collision and the ground check failed.

**Fix**
The target position is now calculated from the player's side as:

```
TargetY = CoverPoint + PlayerHalfSize
```

so the pawn stops with its body outside the cover's collision and the grounded state is preserved.

**Takeaway:** when moving a pawn to a point, remember that the pawn's origin is its center. Always add its half-extent.

---

##  Project Setup

**Requirements:** Unreal Engine **5.8**, Visual Studio with C++ game development workload (see `.vsconfig`).

1. Clone the repository.
2. Open `Dog_2D.uproject` and let Unreal rebuild the modules (or build `Dog_2DEditor` from your IDE).
3. Open the main menu level and press **Play**.

**Configuration notes**
- Interaction trace channel: `ECC_GameTraceChannel2` (Project Settings → Collision).
- The player Blueprint needs a camera boom component named `SpringArm` (used for the camera Y-lock).
- Assign the widget classes (`MainMenuWidget`, `PauseMenuWidget`, `CaughtWidget`) in the game mode Blueprint.
- Set the main menu and first level maps in the `GameInstance` Blueprint.
- Level references (`TargetDog`, `HouseAlarmSystem`, `TerritoryTrigger`, `TargetDisappeared`, `StopScaryEventZone`) are assigned on the placed actors in the level.
- Debug traces are controlled by `DogDebugTraceMode` (off by default).
