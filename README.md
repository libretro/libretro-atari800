# libretro-atari800

Libretro port of the [Atari800](https://atari800.github.io/) emulator —
Atari 8-bit computers (400/800/600XL/800XL/130XE/XEGS) and the Atari 5200
console.

The bundled Atari800 core is version **5.2.0**. The upstream emulator is
maintained by the Atari800 Development Team (see `atari800/DOC/CREDITS`)
and is released under the GPL (see `atari800/COPYING`).

## Controls

Four controllers are supported when the emulated system is set to 400/800
— useful for games such as *M.U.L.E.*; see the
[Atari 4+ player list](http://www.atarimania.com/faq-atari-400-800-xl-xe-what-games-support-4-or-more-simultaneous-players_93.html).

### Atari 8-bit mode

| Button | Function |
| ------ | -------- |
| D-Pad  | Joystick direction |
| B      | Fire 1 |
| A      | Fire 2 |
| Y      | Space / Fire 3 |
| X      | Return |
| L      | Option (CONSOL) |
| R      | Atari800 menu |
| L2     | Esc |
| R2     | Help |
| L3     | Virtual keyboard toggle |
| Select | Select (CONSOL) |
| Start  | Start (CONSOL) |

### Atari 5200 mode

| Button | Function |
| ------ | -------- |
| D-Pad  | Joystick (digital) |
| B      | Fire 1 |
| A      | Fire 2 |
| Y      | Numpad `#` |
| X      | Numpad `*` |
| L      | Numpad `0` |
| R      | Numpad `1` |
| L2     | Numpad `2` |
| R2     | Numpad `3` |
| L3     | Numpad `7` |
| R3     | Virtual keyboard toggle |
| Select | Pause |
| Start  | Start |

## BIOS / firmware

The core uses the optional ROM files listed in `atari800_libretro.info`
(`5200.rom`, `ATARIBAS.ROM`, `ATARIOSA.ROM`, `ATARIOSB.ROM`, `ATARIXL.ROM`,
`BB01R4_OS.ROM`, `XEGAME.ROM`). When real ROMs are not present, the
bundled AltirraOS replacement ROMs are used.

## Building

```sh
make -f Makefile
```

Standard libretro `platform=` overrides apply
(`platform=unix`, `platform=win`, `platform=wiiu`, `platform=ps2`, ...).

## Links

- Libretro core: <https://github.com/libretro/libretro-atari800>
- Upstream Atari800 emulator: <https://atari800.github.io/>
- Upstream documentation: see `atari800/DOC/` (`README`, `INSTALL`,
  `USAGE`, `FAQ`, `NEWS`, `BUGS`, `TODO`)
