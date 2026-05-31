#pragma once

#include "Project.h"

#include <vector>

class Manager
{
public:
    Manager();
    ~Manager();

    // The manager owns the projects it is given and deletes them on
    // destruction; copying is disabled to avoid double frees.
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    // Takes ownership of the given project.
    void addProject(Project* project);

    // Loads every project file in the given folder, appending the loaded
    // projects to the current list. Returns how many were loaded.
    int loadFromFolder(const string& folder = "data");

    // Writes every project to the given folder. All files already in the
    // folder are removed first so it mirrors the current projects exactly.
    // Returns true if every project was written successfully.
    bool saveToFolder(const string& folder = "data") const;

    // Lists every project ordered by priority (High first).
    void listByPriority(ostream& out = cout) const;
    // Lists every project ordered by deadline (soonest first).
    void listByDeadline(ostream& out = cout) const;

    // Lists every task across all projects, ordered first by the owning
    // project's priority and then by the task's own priority (High first).
    void listTasksByPriority(ostream& out = cout) const;
    // Lists every task across all projects, ordered by the owning
    // project's deadline (soonest first); tasks have no deadline of their own.
    void listTasksByDeadline(ostream& out = cout) const;

    const vector<Project*>& getProjects() const;

private:
    vector<Project*> projects;
    int nextId = 1;
};
