#include "Manager.h"

#include <algorithm>

static string priorityToString(Priority priority)
{
    switch (priority)
    {
    case Priority::Low:    return "Low";
    case Priority::Medium: return "Medium";
    case Priority::High:   return "High";
    }

    return "Unknown";
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

// Prints one readable line per project, in the order given.
static void printOrdered(ostream& out, const vector<const Project*>& ordered)
{
    for (const Project* project : ordered)
    {
        const Date& deadline = project->getDeadline();

        out << project->getName()
            << " | priority: " << priorityToString(project->getPriority())
            << " | deadline: " << deadline.day << '.'
            << deadline.month << '.' << deadline.year
            << '\n';
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
    projects.push_back(project);
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
