# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Unreal Engine 5.6 third-person action game with a combo-based melee combat system. The project combines the Enhanced Third Person template with custom C++ combat components and the MeleeComboVol2 asset pack.

**Key Facts:**
- Engine: Unreal Engine 5.6
- Module: `combined` (Runtime game module)
- Input: Enhanced Input system (modern UE5)
- Platform: Windows 64-bit development

## Build Commands

### Building from Command Line
```bash
# Build for Development Editor
"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" combinedEditor Win64 Development "c:\Users\liamo\GitHub\Capstone\combined\combined.uproject" -waitmutex

# Build for Development Game
"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" combined Win64 Development "c:\Users\liamo\GitHub\Capstone\combined\combined.uproject" -waitmutex

# Build for Shipping
"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" combined Win64 Shipping "c:\Users\liamo\GitHub\Capstone\combined\combined.uproject" -waitmutex
```

### Opening in Editor
Right-click `combined.uproject` and select "Generate Visual Studio project files" if .sln is missing, then open in Unreal Editor.

### Hot Reload
Changes to C++ code require compilation. Use the "Compile" button in the Unreal Editor or rebuild from command line/IDE.

## Architecture

### Combat System (C++ Core)

The custom combat system is implemented in `UCombatComponent` ([CombatComponent.h](Source/combined/CombatComponent.h) / [.cpp](Source/combined/CombatComponent.cpp)):

**Key Responsibilities:**
- Manages light/heavy attack execution
- Tracks combo state and progression (up to 4 light combos by default)
- Plays attack animation montages
- Handles combo window timing via animation notifies

**Attack Flow:**
1. `PerformLightAttack()` or `PerformHeavyAttack()` called (usually from Blueprint character)
2. Component plays corresponding `UAnimMontage` from configured arrays
3. Animation notifies (`AnimNotify_ComboWindowOpen/Close`) trigger combo windows
4. If player inputs during open window, `bShouldContinueCombo` queues next attack
5. On montage end, either continues combo or resets state

**State Machine:**
- `bIsAttacking`: Whether character is currently in attack animation
- `bComboWindowOpen`: Whether player can continue combo (set by anim notifies)
- `bShouldContinueCombo`: Queues next combo attack
- `CurrentComboIndex`: Which combo step (0-based)
- `CurrentAttackType`: Light or Heavy

### Animation Notify System

Two custom C++ animation notifies control combo timing:
- `UAnimNotify_ComboWindowOpen` ([AnimNotify_ComboWindowOpen.h/cpp](Source/combined/AnimNotify_ComboWindowOpen.h))
- `UAnimNotify_ComboWindowClose` ([AnimNotify_ComboWindowClose.h/cpp](Source/combined/AnimNotify_ComboWindowClose.h))

These are placed on animation montage timelines in the editor to mark when combo inputs can be accepted.

**UE 5.6 API Note:** These notifies use the modern `Notify()` signature with `FAnimNotifyEventReference` parameter. When creating new animation notifies, use:
```cpp
virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
```

### Blueprint Integration

**Main Blueprints:**
- `BP_ThirdPersonCharacter`: Player character (attach CombatComponent here)
- `BP_ThirdPersonGameMode`: Defines game rules and default pawn
- `BP_ThirdPersonPlayerController`: Routes enhanced input to character

**Enhanced Input Actions:**
- `IA_LightAttack`: Triggers light attack on CombatComponent
- `IA_Move`, `IA_Look`, `IA_Jump`: Standard third-person controls
- `IA_Sprint`, `IA_Crouch`, `IA_Equip`: Additional movement/interaction

**Asset Pack Integration:**
The `MeleeComboVol2` content includes a Blueprint-based `AC_MeleeCombo` component. The C++ `UCombatComponent` appears to be a custom refactored implementation - check which is actively used on the character.

### Module Dependencies

From [combined.Build.cs](Source/combined/combined.Build.cs):
```csharp
PublicDependencyModuleNames: Core, CoreUObject, Engine, InputCore
```

When adding new UE subsystems (Slate UI, Online Subsystem, etc.), update the dependency module names in Build.cs.

## Common Development Patterns

### Adding New Attacks

1. Create new `UAnimMontage` assets in Content Browser
2. Add `AnimNotify_ComboWindowOpen` and `AnimNotify_ComboWindowClose` to montage timeline
3. Add montage reference to `LightAttackMontages` or `HeavyAttackMontages` arrays on CombatComponent (via Blueprint editor)
4. Adjust `MaxLightComboCount` if extending light combo chain

### Creating New C++ Classes

1. Use Unreal Editor: Tools > New C++ Class
2. This auto-updates Build.cs and generates headers
3. Rebuild project from IDE or command line
4. Header files need `COMBINED_API` macro for module export: `class COMBINED_API UMyNewClass`

### Working with Enhanced Input

Input actions are data assets in `Content/Input/`:
- Bind in `IMC_Default` (Input Mapping Context)
- Subscribe in `BP_ThirdPersonCharacter` or controller
- Call C++ functions from Blueprint input event handlers

## Project History Context

This project was "combined" from multiple sources (see git history):
- Base: Enhanced Third Person template
- Added: MeleeComboVol2 asset pack
- Rebuilt: To fix frame rate issues from previous iteration
- Custom: C++ combat system to replace/augment Blueprint implementation

If you see duplicate systems (e.g., both `AC_MeleeCombo` and `UCombatComponent`), this is why. Check the active character Blueprint to determine which is currently in use.

## File Organization

```
Source/combined/          # All C++ gameplay code
Content/
  ├── Blueprints/         # Main character, game mode
  ├── EnhancedTPTemplateB/ # Third-person template base
  ├── MeleeComboVol2/     # Melee combat asset pack
  ├── Input/              # Enhanced Input actions/mappings
  └── UI/                 # User interface widgets
Config/                   # Project config (input, rendering, etc.)
```

## Tips

- **Performance:** Project was rebuilt due to frame rate issues - be mindful of expensive operations in Tick functions
- **Combo Timing:** Combo window timing is controlled by animation notify placement, not code timers
- **C++ vs Blueprint:** Combat logic is C++ for performance; high-level behavior and UI are Blueprints
- **Hot Reload Issues:** If hot reload fails, close editor and rebuild from command line before reopening
