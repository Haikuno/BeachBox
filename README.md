# BeachBox
An autorunner made using [raylib4Dreamcast](https://github.com/raylib4Consoles/raylib4Dreamcast) and [KallistiOS](https://github.com/KallistiOS/KallistiOS) for the SEGA Dreamcast.

![bbox](https://github.com/user-attachments/assets/60f56675-fcd1-4e42-83f7-5b270f00a1a5)

## Project details
This project is an example game for [KallistiOS](https://github.com/KallistiOS/KallistiOS).

It showcases [raylib4Dreamcast](https://github.com/raylib4Consoles/raylib4Dreamcast), [VMU animations](https://kos-docs.dreamcast.wiki/group__vmu__fb.html), [saving / loading](https://kos-docs.dreamcast.wiki/group__vfs__vmu.html), [BIOS save animation / eyecatch](https://mc.pp.se/dc/vms/fileheader.html), [KOS threading](https://kos-docs.dreamcast.wiki/group__kthreads.html), and more.

The project is made using C23.

## Objective of the game
The goal is to survive as long as you can, while collecting coins to spend on upgrades and unlockables.

There are two dimensions which the player can shift between by pressing the X button,
enabling you to collide with both coins and walls on the same dimension.

The green wall can only be passed by using the teleport power.


## Controls
DPAD / Thumbstick to move

A to jump

X to shift between dimensions

L to use the slowdown power

R to use the teleport power

## Thanks
- Falco Girgis for so much help
- raysan for making [raylib](https://github.com/raysan5/raylib)
- psxdev for [raylib4Consoles](https://github.com/raylib4Consoles)
