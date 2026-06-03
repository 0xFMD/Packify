# Installation

## Requirements

- GCC
- Make

---

## Build

Clone the repository and build with Make:

```bash
git clone https://github.com/0xFMD/Packify

make
```

---

## Run

```bash
./Packify -c example
./Packify -i example.PA file.txt
./Packify -l example.PA
```

---

## Clean

```bash
make clean
```

---

## Nix

If you have Nix installed, you can build without needing GCC or Make manually:

```bash
nix-build
```

```bash
./result/bin/Packify -c example
```
