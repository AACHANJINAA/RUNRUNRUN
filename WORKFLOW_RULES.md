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

- Main level should start gameplay immediately.
- Character should use Unreal default mannequin style first.
- Art migration from the reference project happens after the gameplay loop is stable.
