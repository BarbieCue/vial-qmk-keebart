# 3w6 Pro – Keymap `rk` (Maus-Layer-Meldung an den Host)

Basis ist die Keymap `vial` dieses Forks (Upstream: [Keebart/vial-qmk-keebart](https://github.com/Keebart/vial-qmk-keebart),
Stand `63e2d62`). `config.h`, `rules.mk` und `vial.json` sind unverändert,
in `keymap.c` ist am Ende ein Hook ergänzt:

- Beim **Betreten** von Layer 2 (Maus-Layer) tippt die Tastatur **Strg+Alt+Shift+Super+M**.
- Beim **Verlassen** tippt sie **Strg+Alt+Shift+Super+N**.
- Gerade gehaltene Modifier (auch One-Shot) werden dafür kurz entfernt und danach wiederhergestellt.
- Gesendet wird nur von der Hälfte, die am USB hängt.

Eine GNOME-Shell-Extension kann darauf hören und z. B. die obere Leiste einfärben. Die Tastenbelegung selbst bleibt in Vial (EEPROM); die Vial-UID ist dieselbe wie bei der Original-Keymap.

## Bauen

```bash
git clone --recurse-submodules https://github.com/BarbieCue/vial-qmk-keebart.git
cd vial-qmk-keebart && git checkout vial
# Falls bereits ohne --recurse-submodules geklont wurde:
git submodule update --init --recursive

nix shell nixpkgs#qmk nixpkgs#gcc-arm-embedded nixpkgs#gnumake nixpkgs#git
qmk compile -kb keebart/3w6_pro -km vial    # Original-Keymap (Rückweg)
qmk compile -kb keebart/3w6_pro -km rk      # mit Hook

mkdir -p ~/3w6-firmware
cp .build/keebart_3w6_pro_vial.uf2 ~/3w6-firmware/3w6_pro_vial_original.uf2
cp .build/keebart_3w6_pro_rk.uf2   ~/3w6-firmware/3w6_pro_rk.uf2
```

Die Warnung `lto-wrapper: using serial compilation of 2 LTRANS jobs` ist harmlos.

## Vorher sichern (optional, empfohlen)

Eine Hälfte in den Bootloader bringen (siehe unten), dann:

```bash
nix shell nixpkgs#picotool -c sh -c 'sudo "$(command -v picotool)" save -a "$HOME/3w6-links-original.uf2"'
```

Dasselbe mit der anderen Hälfte (`3w6-rechts-original.uf2`).

## Flashen (jede Hälfte einzeln, dieselbe Datei)

1. Eine Hälfte per USB anschließen. Beim Anschließen darauf achten,
die äußere Taste der oberen Reihe gedrückt zu halten (betritt den BOOT-Modus)
2. Das Laufwerk `RPI-RP2` erscheint; `3w6_pro_rk.uf2` darauf kopieren. Die Hälfte startet neu.
3. Dasselbe mit der anderen Hälfte.
4. Falls das Layout zurückgesetzt wurde: in Vial die gespeicherte `.vil` laden.

## Zurück zur Original-Firmware

Der Bootloader liegt im ROM des RP2040, BOOT+RESET funktioniert immer.

1. `3w6_pro_vial_original.uf2` auf beide Hälften (wie oben).
2. Oder die Fertig-Firmware von <https://keebart.com/products/3w6>.
3. Oder die Sicherung: `3w6-links-original.uf2` bzw. `3w6-rechts-original.uf2` auf die jeweilige Hälfte
   (Kopieren auf `RPI-RP2` oder `picotool load -f <datei>`).
4. Notfalls den Flash löschen mit `flash_nuke.uf2` von Raspberry Pi, danach neu flashen.
