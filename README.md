# SONU Electronic Voting System (Registration Module)

An efficient, file-based registration system designed for the Student Organization of Nairobi University (SONU) elections. This module allows for the secure registration of both students (voters) and electoral candidates, preventing duplicate entries using a binary data store.

## 📂 Project Structure

* **voter.h:** Header file containing the `Voter` and `Candidate` structures and function prototypes.
* **voter.c:** Main source code for registration logic, menus, and file I/O.
* **voters.dat:** Binary database file (auto-generated) where voter records are stored.
* **candidates.dat:** Binary database file (auto-generated) where candidate records are stored.
* **.gitignore:** Prevents compiled binaries (`.exe`) and data files (`.dat`) from being tracked by Git.

## 🛠️ Requirements

- A C compiler (e.g., **GCC**, Clang, or MSVC).
- Terminal or Command Prompt access.

## 🚀 Getting Started

### 1. Compilation
To compile the source code into an executable, run:

```bash
gcc voter.c -o voters.exe

```

### 2. Running the Application

Launch the registration module:

**Windows:**

```bash
voters.exe

```

**Linux/macOS:**

```bash
./voters

```

## 📝 Features Implemented

* [x] **Student Registration:** Capture Student ID and Full Name.
* [x] **Duplicate Prevention:** Checks `voters.dat` to ensure an ID isn't registered twice.
* [x] **Persistent Storage:** Data is saved in a binary format for efficiency and basic security.
* [x] **Voting Status:** Every new voter is initialized with a `has_voted` status of `0` (No).

## ⚠️ Important Notes

* **Binary Data:** The `voters.dat` file is not human-readable. To view the data, you must use the internal display functions or a hex editor.
* **Client-Client Architecture:** Ensure all clients have access to the same directory if sharing the `voters.dat` file over a network drive.

## 🤝 Contribution

1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/AmazingFeature`).
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4. Push to the branch (`git push origin feature/AmazingFeature`).
5. Open a Pull Request.

```

---

### How to create it via Terminal
If you want to create the file quickly without opening an editor, run this in your terminal:

**Windows (PowerShell):**
```powershell
New-Item README.md

```

**Linux/macOS:**

```bash
touch README.md

```

