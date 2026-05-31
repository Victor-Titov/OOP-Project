# OOP-Project

A small console-based **project & task manager** written in C++17. It demonstrates
core object-oriented design: encapsulation, inheritance, polymorphism, and
polymorphic (de)serialization. Projects and their tasks are stored on disk and
managed through an arrow-key driven terminal menu.

> **Platform:** Windows. The interface uses `<conio.h>` (`_getch`) for arrow-key
> input and `cls` to clear the screen.

---

## Features

- **Projects** with name, description, deadline, priority and a derived status.
- **School projects** — a subclass of project that also carries a *class* field.
- **Tasks** inside projects, each with its own title, description, priority and status.
- **Arrow-key menu** to:
  - Print all projects (with their tasks tabulated underneath).
  - Add a project (plain or school) and add tasks to it.
  - Edit a task — only the actions valid for its current state are offered.
  - Cancel / restore a project.
  - Delete a project (with confirmation).
- **Persistence** — projects are loaded from `data/` at startup and written back on exit.
- **Polymorphic save/load** — each file records its type so the right class is rebuilt.

---

## Building

Requires a C++17 compiler (`g++`) and `make`.

```sh
make          # build ./oop_project.exe
make run      # build and run
make clean    # remove build artifacts and the executable
```

Each class is compiled separately into `build/`, and header dependencies are
tracked automatically (editing a header only rebuilds the classes that use it).

To build by hand instead:

```sh
g++ -std=c++17 -Iinclude main.cpp src/Task.cpp src/Project.cpp src/SchoolProject.cpp src/Manager.cpp -o oop_project.exe
```

---

## Usage

Run the program and navigate the menu:

- **Up / Down arrows** — move the selection.
- **Enter** — choose the highlighted option.
- **Esc** — go back / cancel the current step.

Main menu options:

| Option | Description |
| --- | --- |
| Print all projects | Lists every project with its tasks tabulated below. |
| Add project | Pick *Project* or *School project*, then enter its details. |
| Add task to project | Pick a project, then enter the task's details. |
| Edit task | Pick a project → task → action (start, finish, omit, restore, change priority). |
| Cancel project | Mark a project canceled (only non-completed, non-canceled projects are listed). |
| Restore project | Un-cancel a project (only canceled projects are listed). |
| Delete project | Permanently remove a project (asks for confirmation). |
| Exit | Save all projects to `data/` and quit. |

Text fields (names, descriptions, deadlines) are typed in; choices such as
priority and project type use the same arrow menu.

### Status model

A project's status is derived from its tasks:

- `Not started`, `In progress`, `Completed`, `Omitted` (all tasks omitted), or
  `Canceled` if the project has been canceled.

A task moves through `Not started → In progress → Completed`, can be `Omitted`
(and later **restored** to `Not started`), and its priority **cannot** be changed
once it is `Completed`. Cancelling is a whole-project action: the tasks keep their
own status, but a `(canceled)` tag is shown after each while the project is canceled.

---

## Project structure

```
OOP-Project/
├── include/            # headers
│   ├── defines.h       # Date, Priority/Status/ProjectType enums, shared helpers
│   ├── Task.h
│   ├── Project.h
│   ├── SchoolProject.h # : public Project
│   └── Manager.h
├── src/                # implementations
│   ├── Task.cpp
│   ├── Project.cpp
│   ├── SchoolProject.cpp
│   └── Manager.cpp
├── data/               # saved project files (one per project)
├── main.cpp            # the menu / entry point
└── Makefile
```

### Classes

- **`Task`** — a single unit of work; supports start / finish / omit / restore and
  priority changes, enforcing the rules above.
- **`Project`** — holds a list of tasks, assigns task ids, derives its status, and
  knows how to serialize itself (`operator<<`) and print a human-readable form
  (`pretty()`). Its destructor is `virtual` so subclasses can be deleted safely.
- **`SchoolProject : public Project`** — adds a *class* field, extending the header
  line and the serialized form via overridden virtual methods.
- **`Manager`** — owns the projects (`vector<Project*>`), assigns project ids,
  loads/saves the `data/` folder, deletes projects, and lists them by priority or
  deadline.

---

## Data format

Each project is stored in its own text file under `data/`. The first line is a
**type tag** that tells the loader which class to build.

```
<type>          0 = Project, 1 = SchoolProject
<id>
<name>
<description>
<day> <month> <year>
<priority>      0 = Low, 1 = Medium, 2 = High
<canceled>      0 or 1
<taskCount>
                (blank line)
<task 1>
                (blank line)
...
<schoolClass>   (only present when type = 1)
```

Each task block is:

```
<id>
<title>
<description>
<priority>      0 = Low, 1 = Medium, 2 = High
<status>        0 = Not started, 1 = In progress, 2 = Completed, 3 = Canceled, 4 = Omitted
```

On exit the `data/` folder is rewritten to mirror the current projects exactly
(files for deleted projects are removed). File names are derived from the project
name; the real name is stored inside the file, so renaming the file is harmless.
