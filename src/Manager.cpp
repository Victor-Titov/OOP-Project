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

const vector<Project*>& Manager::getProjects() const
{
    return projects;
}
