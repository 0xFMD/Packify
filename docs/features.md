# Features & Usage

---

## Options

| Option | Arguments          | Description                      |
| ------ | ------------------ | -------------------------------- |
| `-c`   | `<archive>`        | Create a new archive             |
| `-i`   | `<archive> <file>` | Insert a file into the archive   |
| `-x`   | `<archive> <file>` | Extract a file from the archive  |
| `-d`   | `<archive> <file>` | Delete a file from the archive   |
| `-r`   | `<archive> <file>` | Replace a file in the archive    |
| `-l`   | `<archive>`        | List all files in the archive    |
| `-n`   | `<archive>`        | Print the number of stored files |
| `-h`   |                    | Show all commands                |

---

### `-c` Create an archive
```bash
Arch -c test
# creates test.PA
```

### `-i` Insert a file
```bash
Arch -i backup.PA notes.txt
```

### `-l` List files
```bash
Arch -l backup.PA
```

### `-x` Extract a file
```bash
Arch -x backup.PA notes.txt
```

### `-d` Delete a file
```bash
Arch -d backup.PA notes.txt
```

### `-r` Replace a file
```bash
Arch -r backup.PA notes.txt
```

### `-n` Count files
```bash
Arch -n backup.PA
```

### `-h` Help
```bash
Arch -h
```
