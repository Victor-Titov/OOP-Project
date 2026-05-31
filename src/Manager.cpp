#include "Manager.h"
#include "SchoolProject.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <cctype>

// Turns a project name into a safe file name stem (alphanumerics kept,
// everything else replaced with '_').
static string sanitize(const string& name)
{
    string result;

    for (char c : name)
    {
        if (isalnum(static_cast<unsigned char>(c)))
        {
            result += c;
        }
        else
        {
            result += '_';
        }
    }

    if (result.empty())
    {
        result = "project";
    }

    return result;
}

// Reads the leading type tag and builds the matching project subclass.
// The constructor then reads the rest of that project's data.
static Project* readProject(istream& in)
{
    int type = readInt(in);

    switch (type)
    {
    case static_cast<int>(ProjectType::School):
        return new SchoolProject(in);
    case static_cast<int>(ProjectType::Base):
    default:
        return new Project(in);
    }
}

// True when deadline a falls strictly before deadline b.
static bool earlier(const Date& a, const Date& b)
{
    if (a.year != b.year)
    {
        return a.year < b.year;
    }
    if (a.month != b.month)
    {
        return a.month < b.month;
    }
    return a.day < b.day;
}

// Prints one readable line per project, in the order given, using the
// project's own pretty stream output.
static void printOrdered(ostream& out, const vector<const Project*>& ordered)
{
    for (const Project* project : ordered)
    {
        out << project->pretty() << '\n';
    }
}

static vector<const Project*> toPointers(const vector<Project*>& projects)
{
    return vector<const Project*>(projects.begin(), projects.end());
}

// A task together with the project that owns it. Tasks carry no deadline
// of their own, so the project supplies both the deadline and the
// higher-level priority used when ordering tasks.
struct ProjectTask
{
    const Project* project;
    const Task* task;
};

static vector<ProjectTask> collectTasks(const vector<Project*>& projects)
{
    vector<ProjectTask> items;

    for (const Project* project : projects)
    {
        for (const Task& task : project->getTasks())
        {
            items.push_back({project, &task});
        }
    }

    return items;
}

Manager::Manager()
{
}

Manager::~Manager()
{
    for (Project* project : projects)
    {
        delete project;
    }
}

void Manager::addProject(Project* project)
{
    project->id = nextId++;
    projects.push_back(project);
}

bool Manager::deleteProject(int id)
{
    for (auto it = projects.begin(); it != projects.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            delete *it;
            projects.erase(it);
            return true;
        }
    }

    return false;
}

int Manager::loadFromFolder(const string& folder)
{
    filesystem::path dir = folder;

    error_code ec;
    int loaded = 0;

    for (const auto& entry : filesystem::directory_iterator(dir, ec))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        ifstream in(entry.path());
        if (!in)
        {
            continue;
        }

        Project* project = readProject(in);
        projects.push_back(project);

        // Loaded projects keep their stored id; advance past it so newly
        // added projects don't reuse an existing id.
        if (project->getId() >= nextId)
        {
            nextId = project->getId() + 1;
        }

        ++loaded;
    }

    return loaded;
}

bool Manager::saveToFolder(const string& folder) const
{
    filesystem::path dir = folder;

    error_code ec;
    filesystem::create_directories(dir, ec);

    // Remove the existing files so the folder mirrors the current projects.
    for (const auto& entry : filesystem::directory_iterator(dir, ec))
    {
        if (entry.is_regular_file())
        {
            filesystem::remove(entry.path(), ec);
        }
    }

    bool ok = true;
    set<string> usedNames;

    for (const Project* project : projects)
    {
        string stem = sanitize(project->getName());
        string fileName = stem + ".txt";

        // Keep names unique so projects sharing a name don't overwrite.
        int suffix = 1;
        while (usedNames.count(fileName) != 0)
        {
            fileName = stem + "_" + to_string(++suffix) + ".txt";
        }
        usedNames.insert(fileName);

        ofstream out(dir / fileName);
        if (!out)
        {
            ok = false;
            continue;
        }

        out << *project;
        if (!out.good())
        {
            ok = false;
        }
    }

    return ok;
}

void Manager::listByPriority(ostream& out) const
{
    vector<const Project*> ordered = toPointers(projects);

    sort(ordered.begin(), ordered.end(),
         [](const Project* a, const Project* b)
         {
             return a->getPriority() > b->getPriority();
         });

    printOrdered(out, ordered);
}

void Manager::listByDeadline(ostream& out) const
{
    vector<const Project*> ordered = toPointers(projects);

    sort(ordered.begin(), ordered.end(),
         [](const Project* a, const Project* b)
         {
             return earlier(a->getDeadline(), b->getDeadline());
         });

    printOrdered(out, ordered);
}

void Manager::listTasksByPriority(ostream& out) const
{
    vector<ProjectTask> items = collectTasks(projects);

    sort(items.begin(), items.end(),
         [](const ProjectTask& a, const ProjectTask& b)
         {
             // Project priority is the more important key; task priority
             // breaks ties within the same project priority.
             if (a.project->getPriority() != b.project->getPriority())
             {
                 return a.project->getPriority() > b.project->getPriority();
             }
             return a.task->getPriority() > b.task->getPriority();
         });

    for (const ProjectTask& item : items)
    {
        out << item.project->getName() << " / " << item.task->getTitle()
            << " | project priority: " << priorityToString(item.project->getPriority())
            << " | task priority: " << priorityToString(item.task->getPriority())
            << '\n';
    }
}

void Manager::listTasksByDeadline(ostream& out) const
{
    vector<ProjectTask> items = collectTasks(projects);

    sort(items.begin(), items.end(),
         [](const ProjectTask& a, const ProjectTask& b)
         {
             return earlier(a.project->getDeadline(), b.project->getDeadline());
         });

    for (const ProjectTask& item : items)
    {
        const Date& deadline = item.project->getDeadline();

        out << item.project->getName() << " / " << item.task->getTitle()
            << " | deadline: " << deadline.day << '.'
            << deadline.month << '.' << deadline.year
            << " | task priority: " << priorityToString(item.task->getPriority())
            << '\n';
    }
}

const vector<Project*>& Manager::getProjects() const
{
    return projects;
}
