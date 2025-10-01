# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is "1v1-Me", an Unreal Engine 5.6 arena combat game where two players fight in close-quarters sword combat. The game features reactive melee combat, dynamic arena obstacles, and movement mechanics like glide jumps and air combos.

## Project Structure

- **`capstone/`** - Main Unreal Engine project directory
  - **`Source/capstone/`** - C++ source code for the main game module
  - **`Content/`** - Unreal Engine assets and blueprints
    - **`MainContent/`** - Core game assets including character animations and input actions
    - **`Variant_Combat/`** - Combat system blueprints and AI implementation
    - **`Characters/`** - Character models and animations (Mannequins)
    - **`Fab/`** - Third-party assets (ARPG Ninja, Paragon Phase)
  - **`Config/`** - Unreal Engine configuration files
- **`docs/`** - Web documentation (HTML/CSS for project showcase)
- **`MyProject/`** - Secondary Unreal project (appears to be unused/experimental)

## Development Commands

### Building the Project
```bash
# Open the project in Unreal Editor
# Use the .uproject file: capstone/capstone.uproject

# Build from command line (if UE5 is in PATH)
UnrealBuildTool.exe capstone Win64 Development -Project="capstone/capstone.uproject"
```

### Key File Locations
- **Main project file**: `capstone/capstone.uproject`
- **Build configuration**: `capstone/Source/capstone/capstone.Build.cs`
- **Game targets**: `capstone/Source/capstone.Target.cs` and `capstone/Source/capstoneEditor.Target.cs`

## Architecture & Key Components

### Core Game Systems
- **Combat System**: Located in `Content/Variant_Combat/`
  - `BP_CombatCharacter.uasset` - Main character blueprint
  - `BP_CombatGameMode.uasset` - Game mode for combat
  - `BP_CombatPlayerController.uasset` - Player input handling
  - `ABP_Manny_Combat.uasset` - Combat animation blueprint

### Character & Animation
- Combat animations in `MainContent/Characters/Anims/Armed_Attack/`
- Equipment system with equip/unequip animations
- Animation montages for combo attacks (`AM_ComboAttack`, `AM_ChargedAttack`)

### AI System
- Enemy AI in `Variant_Combat/Blueprints/AI/`
- Uses Unreal's State Tree system (GameplayStateTree plugin enabled)
- AI controller: `BP_CombatAIController.uasset`
- Enemy spawning: `BP_Combat_EnemySpawner.uasset`

### C++ Module Structure
- Main module: `capstone` (Runtime module)
- Dependencies: Core, CoreUObject, Engine, InputCore
- Sample C++ actor: `MyActor.h/.cpp` (basic template)

### Content Organization
- Third-party assets organized under `Fab/` directory
- Main game content in `MainContent/` and `Variant_Combat/`
- Character assets use Unreal's Mannequin system
- Combat map: `Variant_Combat/Lvl_Combat.umap`

## Development Notes

- Engine Version: Unreal Engine 5.6
- Target Platform: Windows (based on build files)
- Project uses ModelingToolsEditorMode and GameplayStateTree plugins
- Combat system appears to be the primary focus with sword-based melee combat
- Movement system includes momentum-based mechanics on slippery surfaces
- Camera shake effects for combat feedback (`BP_CameraShake_Hit_Player`)

## Branch Information
- Current branch: ME-24-Sword-Trail-VFX (working on sword visual effects)
- Main branch: main

When working on this project, focus on the combat system in `Variant_Combat/` and character systems in `MainContent/`. The project emphasizes visceral, reactive combat with visual and audio feedback systems.