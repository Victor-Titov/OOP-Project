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
    const vector<Project*>& projects = manager.getProjects();
    if (projects.empty())
    {
        clearScreen();
        cout << "There are no projects.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    // 1. Pick the project.
    vector<string> projectNames;
    for (const Project* project : projects)
    {
        projectNames.push_back(project->getName());
    }
    int projectIndex = runMenu("Pick a project:", projectNames);
    if (projectIndex < 0)
    {
        return;
    }
    Project* project = projects[projectIndex];

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
    vector<int> taskIds;
    for (const Task& task : tasks)
    {
        taskLabels.push_back(task.getTitle() + " (" + statusToString(task.getStatus()) + ")");
        taskIds.push_back(task.getId());
    }
    int taskIndex = runMenu("Pick a task:", taskLabels);
    if (taskIndex < 0)
    {
        return;
    }
    int id = taskIds[taskIndex];

    // 3. Pick what to do (no cancel: cancelling is a whole-project action).
    int action = runMenu("What do you want to do with the task:",
        {"Start", "Finish (complete)", "Omit",
         "Increase priority", "Decrease priority"});
    if (action < 0)
    {
        return;
    }

    bool ok = false;
    switch (action)
    {
    case 0: ok = project->startTask(id); break;
    case 1: ok = project->finishTask(id); break;
    case 2: ok = project->omitTask(id); break;
    case 3: ok = project->increaseTaskPriority(id); break;
    case 4: ok = project->decreaseTaskPriority(id); break;
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
    const vector<Project*>& projects = manager.getProjects();
    if (projects.empty())
    {
        clearScreen();
        cout << "There are no projects to cancel.\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    // Same project picker as adding a task.
    vector<string> names;
    for (const Project* project : projects)
    {
        names.push_back(project->getName());
    }

    int index = runMenu("Pick a project to cancel:", names);
    if (index < 0)
    {
        return;   // Escape cancels
    }

    bool cancelled = projects[index]->cancel();

    clearScreen();
    if (cancelled)
    {
        cout << "Cancelled project: " << projects[index]->getName() << '\n';
    }
    else
    {
        cout << "Could not cancel \"" << projects[index]->getName()
             << "\" (it is already completed).\n";
    }
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
            running = false;
            break;
        default:
            break;   // Escape on the main menu: just redraw
        }
    }

    manager.saveToFolder();   // rewrite the saved projects on exit
    return 0;
}
