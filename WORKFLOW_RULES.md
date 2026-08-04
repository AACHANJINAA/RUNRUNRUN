# WORKFLOW RULES

## Engine Version

- This project is currently based on `Unreal Engine 5.6`.
- Do not assume `5.8` behavior, APIs, include-order rules, or build defaults unless the project is explicitly upgraded.

## Debugging Rule

- In-game debugging, runtime verification, PIE checks, packaged-run checks, and gameplay confirmation must be performed by the user.
- Codex can prepare code, configs, logs, test instructions, and suspected fixes, but the user is the final runtime debugger for this project.

## Working Agreement

- Prefer implementing gameplay systems in C++ first.
- Use the current project only for the systems we actually need:
  - endless runner movement
  - obstacle spawning
  - repeating tile generation and cleanup
  - score, death, pause, and restart
- Add UI later. Do not block core gameplay work on menu UI.

## Current Direction

- This game is currently being developed as a `Temple Run`-style prototype.
- Main level should start gameplay immediately.
- The first playable prototype should use the Unreal default character setup before final art is integrated.
- Structure the prototype so that character meshes, obstacle meshes, and other art assets can be replaced easily later without rewriting gameplay logic.
- Art migration from the reference project happens after the gameplay loop is stable.

## Prototype Goal

- The core game must function as an endless runner with an infinitely repeating path.
- The player must be able to avoid obstacles through:
  - jumping
  - sliding
  - moving across three lanes
- The prototype should prioritize stable gameplay flow first, then animation polish, then final asset replacement.
