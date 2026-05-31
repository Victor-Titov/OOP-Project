#include "Project.h"

#include <fstream>
#include <filesystem>

static Date readDate(istream& in)
{
    Date date{};
    in >> date.day >> date.month >> date.year;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    return date;
}

static list<Task> readTasks(istream& in)
{
    int count = readInt(in);
    readLine(in);   // consume the blank line after the project header

    list<Task> tasks;
    for (int i = 0; i < count; ++i)
    {
        tasks.emplace_back(in);
        readLine(in);   // consume the blank line after each task
    }

    return tasks;
}

Project::Project()
    : deadline{}, priority(Priority::Low)
{
}

Project::Project(const string& name, const string& description,
                 const Date& deadline, const Priority& priority)
    : name(name), description(description), deadline(deadline),
      priority(priority)
{
}

Project::Project(istream& in)
    : id(readInt(in)),
      name(readLine(in)),
      description(readLine(in)),
      deadline(readDate(in)),
      priority(static_cast<Priority>(readInt(in))),
      tasks(readTasks(in))
{
    for (const Task& task : tasks)
    {
        if (task.getId() >= nextId)
        {
            nextId = task.getId() + 1;
        }
    }
}

Project::~Project()
{
}

void Project::addTask(const Task& task)
{
    tasks.push_back(task);
    tasks.back().id = nextId++;
}

bool Project::deleteTask(int id)
{
    for (auto it = tasks.begin(); it != tasks.end(); ++it)
    {
        if (it->getId() == id)
        {
            tasks.erase(it);
            return true;
        }
    }

    return false;
}

Task* Project::findTask(int id)
{
    for (Task& task : tasks)
    {
        if (task.getId() == id)
        {
            return &task;
        }
    }

    return nullptr;
}

bool Project::startTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->start();
}

bool Project::finishTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->finish();
}

bool Project::cancelTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->cancel();
}

bool Project::omitTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->omit();
}

bool Project::increaseTaskPriority(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->increasePriority();
}

bool Project::decreaseTaskPriority(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->decreasePriority();
}

bool Project::cancel()
{
    if (getStatus() == Status::Completed)
    {
        return false;
    }

    for (Task& task : tasks)
    {
        task.cancel();
    }

    return true;
}

bool Project::increasePriority()
{
    if (priority == Priority::High)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) + 1);
    return true;
}

bool Project::decreasePriority()
{
    if (priority == Priority::Low)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) - 1);
    return true;
}

bool Project::saveToFile(const string& fileName) const
{
    filesystem::path dir = "data";

    error_code ec;
    filesystem::create_directories(dir, ec);

    ofstream out(dir / fileName);
    if (!out)
    {
        return false;
    }

    out << *this;
    return out.good();
}

int Project::getId() const
{
    return id;
}

const string& Project::getName() const
{
    return name;
}

const string& Project::getDescription() const
{
    return description;
}

const Date& Project::getDeadline() const
{
    return deadline;
}

Priority Project::getPriority() const
{
    return priority;
}

Status Project::getStatus() const
{
    if (tasks.empty())
    {
        return Status::NotStarted;
    }

    bool allDone = true;
    bool allNotStarted = true;
    bool allOmitted = true;
    bool hasCanceled = false;

    for (const Task& task : tasks)
    {
        if (task.getStatus() != Status::Completed &&
            task.getStatus() != Status::Omitted)
        {
            allDone = false;
        }
        if (task.getStatus() != Status::NotStarted)
        {
            allNotStarted = false;
        }
        if (task.getStatus() != Status::Omitted)
        {
            allOmitted = false;
        }
        if (task.getStatus() == Status::Canceled)
        {
            hasCanceled = true;
        }
    }

    if (allOmitted)
    {
        return Status::Canceled;
    }
    if (allDone)
    {
        return Status::Completed;
    }
    if (hasCanceled)
    {
        return Status::Canceled;
    }
    if (allNotStarted)
    {
        return Status::NotStarted;
    }

    return Status::InProgress;
}

const list<Task>& Project::getTasks() const
{
    return tasks;
}

ostream& operator<<(ostream& out, const Project& project)
{
    project.write(out);
    return out;
}

void Project::write(ostream& out) const
{
    out << static_cast<int>(ProjectType::Base) << '\n';
    writeBody(out);
}

void Project::writeBody(ostream& out) const
{
    out << id << '\n'
        << name << '\n'
        << description << '\n'
        << deadline.day << ' '
        << deadline.month << ' '
        << deadline.year << '\n'
        << static_cast<int>(priority) << '\n'
        << tasks.size() << '\n'
        << '\n';   // blank line after the project header

    for (const Task& task : tasks)
    {
        out << task << '\n';   // blank line after each task
    }
}

Project::Pretty Project::pretty() const
{
    return Pretty{*this};
}

void Project::printHeader(ostream& out) const
{
    out << '[' << id << "] " << name
        << " | priority: " << priorityToString(priority)
        << " | deadline: " << deadline.day << '.'
        << deadline.month << '.' << deadline.year;
}

void Project::printPretty(ostream& out) const
{
    printHeader(out);

    // List the tasks tabulated beneath the project header.
    for (const Task& task : tasks)
    {
        out << "\n\t[" << task.getId() << "] " << task.getTitle()
            << " | priority: " << priorityToString(task.getPriority())
            << " | status: " << statusToString(task.getStatus());
    }
}

ostream& operator<<(ostream& out, const Project::Pretty& pretty)
{
    pretty.project.printPretty(out);
    return out;
}
