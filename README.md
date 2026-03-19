# SONU Electronic Voting System

A standalone, file-based electronic voting system for the Student Organization of Nairobi University (SONU). The system handles voter registration, candidate registration across 11 official positions, per-position voting with session resumption, live vote tallying, and official results announcement with tie detection.

The program runs entirely on **one computer** and does **not require internet, servers, or external services**.

---

## Project Structure

This project follows **Modular Programming** principles. Each file has a single, clearly defined responsibility.

| File              | Description                                                                                         |
| ----------------- | --------------------------------------------------------------------------------------------------- |
| `voter.h`         | Shared header containing the `Voter` and `Candidate` structs and all function prototypes.           |
| `main.c`          | Entry point. Presents the main menu and dispatches choices to the appropriate module.               |
| `voter.c`         | Handles voter registration and duplicate checking via student ID scan.                              |
| `candidate.c`     | Handles candidate registration across all 11 SONU positions and displays registered candidates.    |
| `voting.c`        | Handles vote casting with bitmask-based per-position session tracking.                             |
| `results.c`       | Handles vote tallying and official results announcement with tie detection.                         |
| `utils.c`         | Shared utility layer — generic file I/O functions reusable across modules.                         |
| `voters.dat`      | Auto-generated binary file storing registered voter records.                                        |
| `candidates.dat`  | Auto-generated binary file storing registered candidate records and live vote counts.               |

> ⚠️ **Do not open `.dat` files in a text editor.** They are stored in raw binary format and will appear as unreadable characters.

---

## Data Structures

Both structures are defined in `voter.h` and shared across all modules.

### Voter
| Field        | Type       | Description                                                                                      |
| ------------ | ---------- | ------------------------------------------------------------------------------------------------ |
| `student_id` | `char[15]` | University student ID. Used as the primary key for duplicate checking and voter lookup.          |
| `name`       | `char[50]` | Full name of the voter.                                                                          |
| `has_voted`  | `int`      | Set to `1` only after all 11 positions have been voted. Prevents re-entry into the voting flow. |
| `votes_cast` | `int`      | Bitmask. Bit `p` is set immediately after the voter casts their vote for position `p` (1–11).   |

### Candidate
| Field        | Type       | Description                                                                              |
| ------------ | ---------- | ---------------------------------------------------------------------------------------- |
| `student_id` | `char[15]` | Candidate's student ID. Used for duplicate prevention and displayed in results.          |
| `name`       | `char[50]` | Full name of the candidate.                                                              |
| `position`   | `char[50]` | One of the 11 official SONU positions assigned at registration.                          |
| `votes`      | `int`      | Vote counter. Starts at `0`. Incremented live during voting and written back to disk.    |

---

## SONU Positions

The system supports all 11 official SONU electoral positions:

| # | Position                              |
|---|---------------------------------------|
| 1 | Chairman                              |
| 2 | Vice Chairman                         |
| 3 | Secretary General                     |
| 4 | Organizing Secretary                  |
| 5 | Secretary for Finance                 |
| 6 | Secretary for Academic Affairs        |
| 7 | Secretary for Catering and Accommodation |
| 8 | Secretary for Legal Affairs           |
| 9 | Secretary for Gender Affairs          |
| 10 | Secretary for Special Needs          |
| 11 | Campus/Faculty Representatives       |

---

## Features

### Voter Management
- Register a voter with Student ID and full name
- Duplicate prevention — a student ID cannot be registered twice
- Voting status tracked per voter

### Candidate Management
- Register a candidate for any of the 11 SONU positions
- Duplicate prevention — a student ID cannot register as candidate twice
- View all registered candidates with current vote counts

### Voting
- Vote for candidates across all 11 positions in a single session
- **Session resumption** — if the program is closed mid-vote, the session is saved and resumes from where it left off on next login
- Positions with no registered candidates are skipped automatically
- Invalid input is rejected with a re-prompt — no silent errors

### Results
- **Vote tally** — live per-position vote count for all candidates
- **Announce results** — declares winner per position, handles ties with re-vote recommendation, shows total votes cast

---

## How the Bitmask Works

The `votes_cast` field in the `Voter` struct uses individual bits to track which positions have been voted:

```
Bit 1  = Chairman
Bit 2  = Vice Chairman
...
Bit 11 = Campus/Faculty Representatives

Set:   voter.votes_cast |= (1 << p)
Check: voter.votes_cast &  (1 << p)
```

Each bit is written to disk **immediately** after its position is voted. If the program closes mid-session, no position can be voted twice on re-entry.

---

## Utility Layer (`utils.c`)

`utils.c` provides generic, struct-agnostic file functions shared across modules and reusable in other applications.

| Function          | Description                                                                    |
| ----------------- | ------------------------------------------------------------------------------ |
| `read_record()`   | Reads one record at a given index from a binary file using a direct byte seek. |
| `append_record()` | Appends a record to a binary file and returns the new record's positional ID.  |

> **Note:** Voter and candidate lookups use sequential `fread()` scans rather than `read_record()` because both are keyed by string student IDs, not positional integers.

---

## How to Compile and Run

All `.c` files must be compiled together.

### Step 1 — Open the terminal

In VS Code, press:
```
Ctrl + `
```

### Step 2 — Navigate to the project folder

```bash
cd your-project-folder
```

### Step 3 — Compile

```bash
gcc main.c voter.c candidate.c voting.c results.c utils.c -o voters
```

### Step 4 — Run

**Windows:**
```bash
.\voters.exe
```

**Linux / macOS:**
```bash
./voters
```

> ⚠️ If you modify either struct in `voter.h` (add or remove a field), delete `voters.dat` and `candidates.dat` before running again. The binary layout of existing records will not match the new struct size and will produce corrupted output.

---

## Menu Options

```
1. Register Voter
2. Register Candidate
3. View Candidates
4. Cast Vote
5. View Vote Tally
6. Announce Results
7. Exit
```

---

## Windows GCC Setup

If you see:
```
gcc is not recognized
```

GCC is not installed. Follow the steps below.

### Step 1 — Download MSYS2

Go to https://msys2.org and download `msys2-x86_64-latest.exe`. Run the installer with default settings.

### Step 2 — Install GCC

When the MSYS2 terminal opens, run:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```
Type `Y` when prompted and wait for installation to complete.

### Step 3 — Add GCC to PATH

1. Open **Start Menu** and search for **Environment Variables**
2. Click **Edit the system environment variables**
3. Click **Environment Variables**
4. Under **User variables**, select `Path` and click **Edit**
5. Click **New** and add:
```
C:\msys64\ucrt64\bin
```
6. Click **OK** on all windows.

### Step 4 — Restart VS Code

Close and reopen VS Code completely for the PATH change to take effect.

### Step 5 — Verify

```bash
gcc --version
```

You should see version information confirming GCC is ready.

---

## Important Notes

- **Binary data** — `.dat` files are not human-readable. Use the program's display functions to view stored records.
- **Standalone only** — the system is designed for a single machine. Sharing `.dat` files over a network drive is not supported and may cause data corruption.
- **No authentication** — voters are identified by student ID only. There is no PIN or password system.
- **Candidate cap** — the voting module supports up to 200 registered candidates. This is sufficient for a typical student election.