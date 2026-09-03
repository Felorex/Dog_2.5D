\# Custom 2.5D Movement \& Object Interaction System (Unreal Engine C++ / Blueprints)

A more advanced iteration of the same first level from my 2D Dog Platformer prototype (https://github.com/Felorex/prototype-2D-platformer), rebuilt in Unreal Engine to explore custom 2.5D movement and object interaction — implemented from scratch instead of relying on the standard CharacterMovementComponent.

Custom character (Pawn) movement physics and interactive object (box) interaction mechanics in 2.5D space, implemented without the standard CharacterMovementComponent.

\## Video:



https://github.com/user-attachments/assets/e42f6fed-ef22-41dc-8758-de8d645ec504







\## Implemented Architectural Solutions \& Fixes:



\### 1. Y-Axis Stabilization for Character and Objects

\* Box locking: The interactive object (`AInteractiveBox`) is rigidly locked to its initial depth line (`OriginalY`) and fully isolated from any screen-depth displacement.

\* Dynamic Pawn approach: When grabbing an object, the dog automatically performs a micro-shift along Y toward the near edge of the box via FInterpTo interpolation, calculating the distance based on the head's dimensions (`TargetY = BoxY + DogHeadY`). On release, the character smoothly returns to the base 2.5D line.

\* Jump centering: When jumping onto the box from above (`IsJumping`), the target offset point is recalculated to the box's pure geometric center (`TargetY = Box->GetActorLocation().Y`), ensuring a perfect paw landing on the object.



\### 2. Character Collision Optimization \& X-Axis Alignment

\* Tick optimization: Heavy physics box-traces for wall detection have been completely removed from the per-frame update. X-axis alignment (`InteractMovementX`) is triggered strictly on events: when movement bools change (`bIsPushing` / `bIsPulling`) and forcibly on the first frame of an interaction click.

\* Texture clipping fix (Z-constraint): A geometric check of the character's height relative to the box's top has been implemented. Depth movement along Y (into the screen) is only allowed once the dog's paw bottom rises above the box's top edge (`GetBottomZ() > BoxTopZ`). This fully eliminated the bug where the character would clip into the box's side walls when jumping close to it.



\### 3. Engine Physics Integration \& Logic Leak Fixes

\* Wall collision speed sync: Stopping the box on collision between the dog and walls is implemented by intercepting the FHitResult structure from the AddActorWorldOffset(..., true) function. When the character's body hits a wall, its C++ VelocityX is zeroed out by the engine, and the box instantly stops along with it, resetting the block when input is reversed.

\* Mirrored collision restoration: Fixed a critical flag-leak bug. A mirrored reset of the IgnoreActorWhenMoving(Box, false) flag was added inside InteractReleased, restoring the box's collidability for the ground trace right after dragging ends.

\* Blueprint preset fix: The box's collision preset was switched from the finicky PhysicsBody to a solid BlockAllDynamic (with object type `WorldDynamic`), allowing the shared C++ ground trace (`CheckGrounded`) to correctly read the object's top as a proper floor.



\### 4. Smart Crouch \& 2.5D Camera Lock

\* Feet position getter: Implemented a custom GetFeetZ() function that dynamically returns the world-space Z coordinate of the paws based on body scale changes, simplifying and shortening the math checks throughout the code.

\* "Inverse" crouch logic: Implemented a concise memory reset for the box when landing on the floor (`if (GetBottomZ() < BoxTopZ)`), while the box reference is retained in memory when the dog lies down \*on top of\* the box, firmly locking the Y-axis.

\* Camera Y-lock: Inside the C++ Tick, a performant world-space Y-coordinate lock is applied to the Blueprint SpringArm component (`WorldLocation.Y = OriginalY`) based on a pointer saved in BeginPlay. The screen smoothly follows the character along the X and Z axes while fully ignoring depth-axis drift.



\### 5. Autonomous Item Physics \& Multi-Channel Interaction Scan

\* Multi-Channel Search Tool: Upgraded the shared interaction trace inside CanInteractWithObjects from a single hit to a vertically elongated BoxTraceMulti (dimensions: X=30, Y=30, Z=65, with a core Z-offset +5.0f). This allows the dog to scan the world for both boxes and small items simultaneously.

\* Loose Coupling & "Anti-Cheat" Raycast: Implemented a standalone TryTake impulse logic inside the AItemBone class. Upon a pick-up click, the bone fires a 60cm raycast strictly upward (ignoring the floor via a +15.0f Z-start offset). If a dynamic object (like a heavy box) is detected directly above, the bone rejects the pick-up and safely nullifies the character's focus pointer via ClearItemBone(), completely preventing players from grabbing hidden items through solid walls.

\* State Matrix Enforcement: Designed an else-driven memory block that perfectly handles interaction priorities. If the dog is moving a box, item detection shuts down; if an item is grabbed, the dog becomes "blind" to boxes. All pointers are instantly erased from memory the moment the object leaves the trace area, eliminating "phantom actions" at a distance.

\* Performance Optimization (Tick Sleep): Built a self-contained gravity loop inside the bone. Using GetActorBounds, the bone calculates its precise half-height in world space, enabling a +5.0f ground trace to accurately read the floor even when the bone mesh is rotated 90 degrees horizontally. Once the bone safely lands, its C++ velocity is zeroed, and its per-frame update is put to sleep (SetActorTickEnabled(false)) to save CPU cycles until the next interaction.
