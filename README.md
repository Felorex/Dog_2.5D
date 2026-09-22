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

\## Latest Update: Dynamic NPC Booth AI (`ANpcDogPawn`)

Implemented a state-driven behavior matrix for autonomous NPC dogs, perfectly combining startup physical testing with performant state transitions:

\** 1. Delayed Physics Verification: In the constructor, the NPC dog is initialized in `EDogState::Alert`. This allows us to spawn the NPC slightly above the ground in the editor, ensuring that engine physics and gravity drop the dog onto the surface correctly upon game startup.
\** 2. Tick Sleep for Optimization: The home location check inside `Tick` runs strictly while the dog is still in the `Alert` state. The second the dog satisfies the landing and home conditions, the state switches, and the code blocks further `Tick` execution for this logic, saving CPU power.
\** 3. Sub-Centimeter Anti-Drift Home Trigger (`CheckHomeLocation`):
\* Calculates the absolute difference between the current location and the home point via `FMath::Abs(CurrentLoc - HomeX) <= 5.f` to prevent physical displacement bugs.
\* Once grounded (`IsGrounded`), it instantly fires a visual turn command to face left (`OnLookLeftVisual()`).
\* Directly invokes the encapsulated base-class method `OnCrouchPressed()`, cleanly shifting the NPC into `EDogState::Repose` and making the dog lie down inside its booth automatically.

\## Future Development Roadmap

@Player Target Tracking: Implement player tracking using `UGameplayStatics::GetPlayerCharacter` to feed coordinates to the NPC dog.
@State-Driven Pursuit (`EDogState::Chase`): Build a responsive chase behavior loop that updates the `AiMoveDirection` based on the player's position relative to the dog.
@Dynamic Aggro & Leash Zones: Add distance-based checks to make the dog stop chasing or return to its home booth (`EDogState::Repose`) if the player gets too far.