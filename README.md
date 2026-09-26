\# Custom 2.5D Movement \& Object Interaction System (Unreal Engine C++ / Blueprints)

A highly advanced iteration of my 2D Dog Platformer prototype (https://github.com/Felorex/prototype-2D-platformer), completely rebuilt in Unreal Engine to explore custom 2.5D movement, state-driven AI, and complex object interactions — implemented from scratch without relying on the standard CharacterMovementComponent.

\## Video:



https://github.com/user-attachments/assets/e42f6fed-ef22-41dc-8758-de8d645ec504




\## Architectural Core: Base Class Inheritance & Clean Separation

To eliminate code duplication and create a scalable foundation, the project has been refactored into a strict hierarchical C++ architecture:

\* `ABaseDogPawn` (The Foundation): Encapsulates shared movement physics, gravity checks (`IsGrounded`), core movement variables, and the unified Smart Crouch System (`OnCrouchPressed` / `OnCrouchReleased`).
\* Engine-Level Multi-Channel Collision Filtering: Refactored the interaction queries by implementing dedicated, low-overhead trace channels defined directly via DefaultEngine.ini:
\* `ECC_GameTraceChannel1` (`PickupObject`) — A specialized object type dedicated strictly to small lootable items (e.g., bones).
\* `ECC_GameTraceChannel2` (`InteractTrace`) — A custom visibility/trace channel optimized for high-performance scanning. 
Using these channels, the base class runs a focused `BoxTraceMulti` (X=30, Y=30, Z=65, Z-offset +5.0f). This completely bypasses heavy environment sweeps, ignoring static geometry, walls, and character meshes at the physics-engine level.
\* Player-Exclusive Box Interaction: All complex logic for grabbing, pushing, pulling, and alignment with heavy interactive boxes (`AInteractiveBox`) has been isolated and moved strictly into the Main Character class (`ADogPawn`). The NPC dogs remain "blind" to heavy boxes, ensuring proper game-logic decoupling.

\## Implemented Features & Core Physics Solutions

\### 1. Y-Axis Stabilization for Character and Objects

\* Box locking: The interactive object is rigidly locked to its initial depth line (`OriginalY`) and fully isolated from screen-depth displacement.
\* Dynamic Pawn approach (GG Only): When grabbing an object, the dog automatically performs a micro-shift along Y toward the near edge of the box via `FInterpTo` interpolation (`TargetY = BoxY + DogHeadY`). On release, it smoothly returns to the base 2.5D line.
\* Jump centering: When jumping onto the box from above (`IsJumping`), the target offset point is recalculated to the box's pure geometric center, ensuring a perfect paw landing.

\### 2. Character Collision Optimization & X-Axis Alignment

\* Tick optimization: Heavy physics box-traces for wall detection have been completely removed from the per-frame update. X-axis alignment (`InteractMovementX`) is triggered strictly on events: when movement bools change (`bIsPushing / bIsPulling`) and forcibly on the first frame of an interaction click.
\* Texture clipping fix (Z-constraint): Depth movement along Y (into the screen) is only allowed once the dog's paw bottom rises above the box's top edge (`GetBottomZ() > BoxTopZ`). This fully eliminated the bug where the character would clip into the box's side walls when jumping close to it.

\### 3. Engine Physics Integration & Logic Leak Fixes

\* Wall collision speed sync: Stopping the box on collision between the dog and walls is implemented by intercepting the `FHitResult` structure from the `AddActorWorldOffset(..., true)` function. When the character's body hits a wall, its C++ `VelocityX` is zeroed out by the engine, and the box instantly stops along with it.
\* Mirrored collision restoration: Fixed a critical flag-leak bug by adding a mirrored reset of the `IgnoreActorWhenMoving(Box, false)` flag inside `InteractReleased`, restoring the box's collidability for the ground trace right after dragging ends.
\* Blueprint preset fix: The box's collision preset was switched to `BlockAllDynamic` (with object type `WorldDynamic`), allowing the shared C++ ground trace (`CheckGrounded`) to correctly read the object's top as a proper floor.

\### 4. Smart Crouch Reusability & 2.5D Camera Lock

\* Feet position getter: Implemented a custom `GetFeetZ()` function that dynamically returns the world-space Z coordinate of the paws based on body scale changes, simplifying math checks across the entire codebase.
\* Encapsulated Crouch Logic: The core `OnCrouchPressed()` function handles all physical checks (calculating box edges vs dog bottom height). It is decoupled from direct input, allowing both the player and AI controllers to safely invoke it.
\* Camera Y-lock: Inside the C++ `Tick`, a performant world-space Y-coordinate lock is applied to the Blueprint `SpringArm` component (`WorldLocation.Y = OriginalY`) based on a pointer saved in `BeginPlay`.

\### 5. Autonomous Item Physics & "Anti-Cheat" Raycast

\* Loose Coupling & Raycast Validation: Implemented standalone `TryTake` impulse logic inside the `AItemBone` class. Upon a pick-up click, the bone fires a 60cm raycast strictly upward. If a dynamic object (like a heavy box) is detected directly above, the bone rejects the pick-up, preventing players from grabbing hidden items through solid walls.
\* State Matrix Enforcement: Designed an else-driven memory block that perfectly handles interaction priorities. If the dog is moving a box, item detection shuts down; if an item is grabbed, the dog becomes "blind" to boxes.
\* Performance Optimization (Tick Sleep): Using `GetActorBounds`, the bone calculates its precise half-height in world space, enabling a +5.0f ground trace to accurately read the floor even when rotated 90 degrees horizontally. Once the bone lands, its C++ velocity is zeroed, and its per-frame update is put to sleep (`SetActorTickEnabled(false)`) to save CPU cycles.

\### 6. Advanced Item Pickup & Mouth Attachment System

\* State-Driven Toggle Interaction: Rewrote the interaction button mapping on the `Started` event layer to eliminate complex nested variables. A robust C++ input toggle evaluates `bIsTakingItem` to cleanly switch between pickup and dropping sequences.
\* Encapsulated Physics Suspension: When picked up, the bone completely shuts down its own simulation (`SetSimulatePhysics(false)`) and responses (`ECollisionEnabled::NoCollision`), becoming non-intrusive to the dog's movement.
\* Hierarchical Socket Component Binding: Created a dedicated `USceneComponent` (`MouthAttachPoint`) attached directly to the dog's head collision. Items are dynamically nested into this component using `FAttachmentTransformRules::SnapToTarget`, allowing the item mesh to automatically mirror head movements.
\* Sub-Millimeter Floating-Point Synchronization Fix: Fixed a critical bug where physics-based interaction with heavy boxes would seize or stutter due to timeline rounding errors (returning Z=0.0 or a sign-flipped -0 value instead of the factory default Z=0.6). Adjusting the timeline's baseline targets to a rigid 0.6 value successfully eliminated collision desynchronization.

\### 2D NPC Dog AI System (`ANpcDogPawn`)

Technical documentation for a robust, highly optimized, and scalable Finite State Machine (FSM) governing a guard dog NPC in a 2D platformer game built with Unreal Engine. The system is designed using clean state isolation and strict transition triggers to completely prevent frame-by-frame event flooding inside the `Tick` loop.

\## Implemented FSM Architecture (EDogState)

The NPC AI is managed via a central `switch` dispatcher located inside `Tick()`, neatly isolating the logic and execution bounds of each individual behavior phase:

\** 1. `EDogState::InitHome` (Blind Startup Walk): 
\* Upon game startup or initial scene spawn, the dog initializes in this state to walk back into its booth completely blind to the player. The global detection radar is ignored during this phase, preventing premature aggression or broken initialization animations.
\** 2. `EDogState::Repose` (Booth Slumber/Rest): 
\* The dog rests inside its doghouse with structural movement forced to zero. A proximity-based trigger zone (`TerritoryTrigger`) is continuously monitored. Crossing this boundary triggers the detection radar and immediately kicks the dog into a chase.
\** 3. `EDogState::Chase` (Active Pursuit): 
\* Executes responsive dynamic movement toward the player character (`ChaseMovement()`). The dog continues running until it reaches its physical leash limit defined by `IsAtLeashEdge()` or the player disappears behind a hiding spot.
\** 4. `EDogState::Barking` (Static Leash Leaning): 
\* Triggers strictly when the dog is at its max leash distance. It executes the barking visual logic exactly once using an internal boolean latch (`IsBarkingVisual`), cleanly preventing high-frequency Blueprint node execution.
\** 5. `EDogState::Alert` (Vigilance & Reaction Delay Switchboard): 
\* Acts as the central tactical "traffic light" of the AI system, operating two independent time counters to evaluate whether the dog should go home or double back for the player.

\## Tactical Branching & Interception Logic

\** Isolated Vigilance Switchboard (`StartToAlert`)
When in the `Alert` state, the dog’s behavior branches into two strictly mutually exclusive execution paths depending on the player's visibility:
\* Target Hidden (Player is behind the bush): The system counts the elapsed time via `AlertTimer`. If the player remains hidden for more than 3.0 seconds, the dog wipes its active memory clean (`PlayerTarget = nullptr`, `PlayerFounded = false`) and steps away into `EDogState::ReturnToDoghouse`.
\* Target Revealed (Player intercepts the dog's path home): If the dog was walking home but the player suddenly emerges from behind the hiding spot, the radar catches the player and drops the dog back into `Alert`. The dog abruptly halts, rotates visually to face the player, and waits/stares for exactly 1.0 second. Once this delay expires, if the dog is still far from the leash edge, it snaps into `EDogState::Chase` to renew pursuit.

\## High-Priority Absolute Bite (`Biting`)
The physical contact verification (`CheckBiting()`) is evaluated outside the core state machine `switch` block, establishing absolute priority. Regardless of the current state, close contact with the main character instantly forces a bite event, freezing momentum and scaring the player (`SetIsScared(true)`). Due to an integrated fear cooldown check, the dog instantly resumes its normal pursuit cycle without structural looping glitches.

\## Sub-Centimeter Anti-Drift Home Return (`ReturnToHome`)
Arrival verification is handled accurately via `FMath::Abs(CurrentLoc - HomeX) <= 5.f` to neutralize minor sliding physics. Upon reaching the threshold and confirming the ground state (`IsGrounded`), the dog kills its velocity, turns to face left (`OnLookLeftVisual()`), flushes residual player flags out of memory, and seats itself neatly into the resting posture.


\## Future Development Roadmap

Tick Sleep Optimization: Integrate `SetActorTickEnabled(false)` while in `EDogState::Repose`, forcing the system to sleep until the background interaction sensor wakes up the main thread component upon player breach.
- Vertical Platform Constraints (Box Checking): Expand the state logic to evaluate the player's location along the Z-axis, enabling the dog to stop right beneath platforms/crates, keeping the target cornered from below while continuously barking.
