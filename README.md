<p align="center">
  <img src="assets/packify-icon.svg" alt="Packify" width="120"/>
</p>

<h1 align="center">Packify</h1>
<p align="center">A command-line tool for bundling files into a single <code>.PA</code> archive.</p>

---

## Options

| Option | Arguments                   | Description                    |
| ------ | --------------------------- | ------------------------------ |
| `-c`   | `<archive>`                 | Create a new archive           |
| `-i`   | `<archive> <file>`          | Insert a file into the archive |
| `-d`   | `<archive> <file>`          | Delete a file from the archive |
| `-l`   | `<archive>`                 | List all files in the archive  |
| `-x`   | `<archive> <file>`          | Extract a file from the archive|
| `-h`   |                             | Show help reference            |
| `-n`   | `<archive>`                 | Number of files                |
| `-r`   | `<archive> <file> <new>`    | replace file                 |

---

## Examples

**Create an archive**

```bash
Packify -c example
# creates example.PA
```

**Insert a file**

```bash
Packify -i example.PA notes.txt
```

**List files**

```bash
Packify -l example.PA
```

**Extract a file**

```bash
Packify -x example.PA notes.txt
```

**Delete a file**

```bash
Packify -d example.PA notes.txt
```

**Replace a file**

```bash
Packify -r example.PA old.txt new.txt
```

**Count files**

```bash
Packify -n example.PA
```

**Help**

```bash
Packify -h
```

---

## Docs

- [File Format](docs/format.md)
- [Installation](docs/installation.md)
- [Contributing](CONTRIBUTING.md)

---

## License

See [LICENSE](LICENSE).
