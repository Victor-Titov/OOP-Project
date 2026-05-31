#include "Manager.h"
#include "SchoolProject.h"

#include <conio.h>
#include <cstdlib>
#include <limits>
#include <vector>

// Key codes returned by _getch() on Windows.
enum Key
{
    KeyEnter = 13,
    KeyEscape = 27,
    KeyArrowPrefixA = 0,
    KeyArrowPrefixB = 224,
    KeyUp = 72,
    KeyDown = 80
};

static void clearScreen()
{
    system("cls");
}

// Draws an arrow-navigable menu and returns the chosen index, or -1 if the
// user pressed Escape. Shared by the main menu and the project picker.
static int runMenu(const string& title, const vector<string>& options)
{
    const int count = static_cast<int>(options.size());
    if (count == 0)
    {
        return -1;
    }

    int selected = 0;
    while (true)
    {
        clearScreen();
        cout << title << '\n';
        cout << "Use the Up/Down arrows to move, Enter to select, Esc to go back.\n\n";

        for (int i = 0; i < count; ++i)
        {
            cout << (i == selected ? " > " : "   ") << options[i] << '\n';
        }

        int key = _getch();
        if (key == KeyArrowPrefixA || key == KeyArrowPrefixB)
        {
            key = _getch();
            if (key == KeyUp)
            {
                selected = (selected - 1 + count) % count;
            }
            else if (key == KeyDown)
            {
                selected = (selected + 1) % count;
            }
        }
        else if (key == KeyEnter)
        {
            return selected;
        }
        else if (key == KeyEscape)
        {
            return -1;
        }
    }
}

// Prompts for a priority using the same arrow menu; defaults to Low on cancel.
static Priority readPriority(const string& title)
{
    int choice = runMenu(title, {"Low", "Medium", "High"});
    if (choice < 0)
    {
        choice = 0;
    }
    return static_cast<Priority>(choice);
}

static void printAllProjects(const Manager& manager)
{
    clearScreen();
    cout << "All projects:\n\n";

    const vector<Project*>& projects = manager.getProjects();
    if (projects.empty())
    {
        cout << "  (no projects)\n";
    }
    for (const Project* project : projects)
    {
        cout << "  " << project->pretty() << '\n';
    }

    cout << "\nPress any key to return...";
    _getch();
}

static void addProject(Manager& manager)
{
    int type = runMenu("Select the project type:", {"Project", "School project"});
    if (type < 0)
    {
        return;   // Escape cancels
    }
    const bool isSchool = (type == 1);

    clearScreen();
    cout << "Add a new project\n\n";

    string name;
    string description;
    Date deadline{};

    cout << "Name: ";
    getline(cin, name);
    cout << "Description: ";
    getline(cin, description);
    cout << "Deadline (day month year): ";
    cin >> deadline.day >> deadline.month >> deadline.year;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string schoolClass;
    if (isSchool)
    {
        cout << "Class: ";
        getline(cin, schoolClass);
    }

    Priority priority = readPriority("Select the project priority:");

    if (isSchool)
    {
        manager.addProject(
            new SchoolProject(name, description, deadline, priority, schoolClass));
    }
    else
    {
        manager.addProject(new Project(name, description, deadline, priority));
    }
}

static void addTask(Manager& manager)
{
    const vector<Project*>& projects = manager.getProjects();
    if (projects.empty())
    {
        clearScreen();
        cout << "There are no projects yet. Add a project first.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    // Build the picker menu from the project names.
    vector<string> names;
    for (const Project* project : projects)
    {
        names.push_back(project->getName());
    }

    int index = runMenu("Pick a project to add a task to:", names);
    if (index < 0)
    {
        return;   // Escape cancels
    }

    clearScreen();
    cout << "Add a task to: " << projects[index]->getName() << "\n\n";

    string title;
    string description;

    cout << "Title: ";
    getline(cin, title);
    cout << "Description: ";
    getline(cin, description);

    Priority priority = readPriority("Select the task priority:");

    projects[index]->addTask(
        Task(title, description, priority, Status::NotStarted));
}

static void editTask(Manager& manager)
{
    // 1. Pick the project (completed and canceled projects are not editable).
    vector<Project*> editable;
    vector<string> projectNames;
    for (Project* project : manager.getProjects())
    {
        Status status = project->getStatus();
        if (status != Status::Completed && status != Status::Canceled)
        {
            editable.push_back(project);
            projectNames.push_back(project->getName());
        }
    }

    if (editable.empty())
    {
        clearScreen();
        cout << "There are no editable projects.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    int projectIndex = runMenu("Pick a project:", projectNames);
    if (projectIndex < 0)
    {
        return;
    }
    Project* project = editable[projectIndex];

    // 2. Pick the task (mapping the menu choice back to the task id).
    const list<Task>& tasks = project->getTasks();
    if (tasks.empty())
    {
        clearScreen();
        cout << "This project has no tasks.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    vector<string> taskLabels;
    vector<const Task*> taskPtrs;
    for (const Task& task : tasks)
    {
        taskLabels.push_back(task.getTitle() + " (" + statusToString(task.getStatus()) + ")");
        taskPtrs.push_back(&task);
    }
    int taskIndex = runMenu("Pick a task:", taskLabels);
    if (taskIndex < 0)
    {
        return;
    }

    const Task* task = taskPtrs[taskIndex];
    const int id = task->getId();
    const Status status = task->getStatus();
    const Priority prio = task->getPriority();

    // 3. Offer only the actions allowed in the task's current state
    // (no cancel: cancelling is a whole-project action).
    enum Action { Start, Finish, Omit, Restore, IncPriority, DecPriority };
    vector<string> actionLabels;
    vector<int> actionCodes;
    auto offer = [&](const string& label, Action code)
    {
        actionLabels.push_back(label);
        actionCodes.push_back(code);
    };

    if (status == Status::NotStarted)
    {
        offer("Start", Start);
    }
    if (status == Status::InProgress)
    {
        offer("Finish (complete)", Finish);
    }
    if (status == Status::NotStarted || status == Status::InProgress)
    {
        offer("Omit", Omit);
    }
    if (status == Status::Omitted)
    {
        offer("Restore (set to not started)", Restore);
    }
    if (status != Status::Completed && prio != Priority::High)
    {
        offer("Increase priority", IncPriority);
    }
    if (status != Status::Completed && prio != Priority::Low)
    {
        offer("Decrease priority", DecPriority);
    }

    if (actionLabels.empty())
    {
        clearScreen();
        cout << "No actions are available for this task in its current state.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    int choice = runMenu("What do you want to do with the task:", actionLabels);
    if (choice < 0)
    {
        return;
    }

    bool ok = false;
    switch (actionCodes[choice])
    {
    case Start:       ok = project->startTask(id); break;
    case Finish:      ok = project->finishTask(id); break;
    case Omit:        ok = project->omitTask(id); break;
    case Restore:     ok = project->restoreTask(id); break;
    case IncPriority: ok = project->increaseTaskPriority(id); break;
    case DecPriority: ok = project->decreaseTaskPriority(id); break;
    }

    clearScreen();
    cout << (ok ? "Done."
                : "That action is not allowed in the task's current state.")
         << '\n';
    cout << "\nPress any key to return...";
    _getch();
}

static void cancelProject(Manager& manager)
{
    // Only projects that are neither completed nor already canceled
    // can be canceled.
    vector<Project*> eligible;
    vector<string> names;
    for (Project* project : manager.getProjects())
    {
        if (!project->isCanceled() && project->getStatus() != Status::Completed)
        {
            eligible.push_back(project);
            names.push_back(project->getName());
        }
    }

    if (eligible.empty())
    {
        clearScreen();
        cout << "There are no projects to cancel.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    int index = runMenu("Pick a project to cancel:", names);
    if (index < 0)
    {
        return;   // Escape cancels
    }

    eligible[index]->cancel();

    clearScreen();
    cout << "Cancelled project: " << eligible[index]->getName() << '\n';
    cout << "\nPress any key to return...";
    _getch();
}

static void restoreProject(Manager& manager)
{
    // Only canceled projects can be restored.
    vector<Project*> eligible;
    vector<string> names;
    for (Project* project : manager.getProjects())
    {
        if (project->isCanceled())
        {
            eligible.push_back(project);
            names.push_back(project->getName());
        }
    }

    if (eligible.empty())
    {
        clearScreen();
        cout << "There are no canceled projects to restore.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    int index = runMenu("Pick a project to restore:", names);
    if (index < 0)
    {
        return;   // Escape cancels
    }

    eligible[index]->restore();

    clearScreen();
    cout << "Restored project: " << eligible[index]->getName() << '\n';
    cout << "\nPress any key to return...";
    _getch();
}

static void deleteProject(Manager& manager)
{
    const vector<Project*>& projects = manager.getProjects();
    if (projects.empty())
    {
        clearScreen();
        cout << "There are no projects to delete.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    vector<string> names;
    for (const Project* project : projects)
    {
        names.push_back(project->getName());
    }

    int index = runMenu("Pick a project to delete:", names);
    if (index < 0)
    {
        return;   // Escape cancels
    }

    // Capture before deleting, then confirm (deletion cannot be undone).
    int id = projects[index]->getId();
    string name = projects[index]->getName();

    int confirm = runMenu("Delete \"" + name + "\"? This cannot be undone.",
                          {"No", "Yes"});
    if (confirm != 1)
    {
        return;   // No or Escape
    }

    manager.deleteProject(id);

    clearScreen();
    cout << "Deleted project: " << name << '\n';
    cout << "\nPress any key to return...";
    _getch();
}

int main()
{
    Manager manager;
    manager.loadFromFolder();   // load the saved projects at startup

    const vector<string> options = {
        "Print all projects",
        "Add project",
        "Add task to project",
        "Edit task",
        "Cancel project",
        "Restore project",
        "Delete project",
        "Exit"};

    bool running = true;
    while (running)
    {
        switch (runMenu("=== Project Manager ===", options))
        {
        case 0:
            printAllProjects(manager);
            break;
        case 1:
            addProject(manager);
            break;
        case 2:
            addTask(manager);
            break;
        case 3:
            editTask(manager);
            break;
        case 4:
            cancelProject(manager);
            break;
        case 5:
            restoreProject(manager);
            break;
        case 6:
            deleteProject(manager);
            break;
        case 7:
            running = false;
            break;
        default:
            break;   // Escape on the main menu: just redraw
        }
    }

    manager.saveToFolder();   // rewrite the saved projects on exit
    return 0;
}
