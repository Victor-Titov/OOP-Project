#pragma once

#include "defines.h"
#include "Task.h"

#include <list>

class Project
{
    friend class Manager;

public:
    Project();
    Project(const string& name, const string& description,
            const Date& deadline, const Priority& priority);
    explicit Project(istream& in);
    virtual ~Project();

    void addTask(const Task& task);
    bool deleteTask(int id);
    bool cancel();
    bool restore();   // un-cancel the project
    bool increasePriority();
    bool decreasePriority();

    bool startTask(int id);
    bool finishTask(int id);
    bool omitTask(int id);
    bool restoreTask(int id);
    bool increaseTaskPriority(int id);
    bool decreaseTaskPriority(int id);

    // Writes the project into data/<fileName>, creating the data folder if needed.
    bool saveToFile(const string& fileName) const;

    int getId() const;
    const string& getName() const;
    const string& getDescription() const;
    const Date& getDeadline() const;
    Priority getPriority() const;
    Status getStatus() const;
    bool isCanceled() const;
    const list<Task>& getTasks() const;

    // Serializes the project (used for saving to files). Delegates to the
    // virtual write() so subclasses serialize their own fields too.
    friend ostream& operator<<(ostream& out, const Project& project);

    // Writes the type tag followed by the project's data. Virtual so a
    // subclass can append its extra fields after the base data.
    virtual void write(ostream& out) const;

    // Lightweight wrapper that streams a project in a human-readable
    // one-line form instead of the serialization format above.
    // Use as: out << project.pretty();
    struct Pretty
    {
        const Project& project;
    };
    Pretty pretty() const;

    // Writes the human-readable form: the project header line followed by
    // its tasks, tabulated below. operator<< for Pretty dispatches here.
    void printPretty(ostream& out) const;

protected:
    // Writes the one-line project header. Virtual so subclasses can append
    // their own fields to the header before the tasks are listed.
    virtual void printHeader(ostream& out) const;

    // Writes the common project fields (id, name, ..., tasks) without a
    // type tag, so subclasses can reuse it inside their own write().
    void writeBody(ostream& out) const;

private:
    Task* findTask(int id);

    int id = 0;
    const string name;
    const string description;
    const Date deadline;
    Priority priority;
    bool canceled = false;
    list<Task> tasks;
    int nextId = 1;
};

ostream& operator<<(ostream& out, const Project::Pretty& pretty);
